#include "Tomasulo.h"
#include "log.h"
#include "instructions/Instruction.h"
#include <cassert>
#include <string>

static const std::string TAG = "Tomasulo";

// machine configuration
static const int GPR_REGISTERS = 32;
static const int FPR_REGISTERS = 32;
static const int INTEGER_STATIONS = 8;
static const int INTEGER_CYCLES = 1;
static const int TRAP_STATIONS = 4;
static const int TRAP_CYCLES = 1;

Tomasulo::Tomasulo(MemoryPtr memory, bool verbose)
  : verbose(verbose),
    instructionFactory(nullptr),
    halted(false),
    stallIssue(false),
    clockCounter(0),
    pc(0),
    memory(memory),
    registerFile(nullptr),
    renameRegisterFile(nullptr),
    commonDataBus(nullptr),
    functionalUnits()
{
  assert(memory != nullptr);

  registerFile = RegisterFilePtr(
    new RegisterFile(GPR_REGISTERS, FPR_REGISTERS)
    );
  renameRegisterFile = RenameRegisterFilePtr(new RenameRegisterFile);
  commonDataBus = CommonDataBusPtr(
    new CommonDataBus(registerFile, renameRegisterFile)
    );
  instructionFactory = InstructionFactoryPtr(
    new InstructionFactory(memory, registerFile)
    );

  ReservationStationDependencies deps(
    registerFile, renameRegisterFile, memory, pc, commonDataBus
    );

  functionalUnits[FunctionalUnitType::Integer] = 
    FunctionalUnitPtr(
      new FunctionalUnit(
        FunctionalUnitType::Integer, false, INTEGER_CYCLES, 
        INTEGER_STATIONS, deps
        )
    );
  functionalUnits[FunctionalUnitType::Trap] =
    FunctionalUnitPtr(
      new FunctionalUnit(
        FunctionalUnitType::Trap, true, TRAP_CYCLES,
        TRAP_STATIONS, deps
        )
    );
}

bool Tomasulo::isHalted() const
{
  return halted;
}

std::size_t Tomasulo::clocks() const
{
  return clockCounter;
}

void Tomasulo::run(Address entryPoint)
{
  pc = entryPoint;

  while (!halted || !functionalUnitsIdle())
  {
    ++clockCounter;

    logger->info(TAG) << "Cycle " << clockCounter << " write";
    write();
    logger->info(TAG) << "Cycle " << clockCounter << " execute";
    execute();
    logger->info(TAG) << "Cycle " << clockCounter << " issue";
    if (!halted)
    {
      UWord rawInstruction = memory->readUWord(pc);
      InstructionPtr instr = instructionFactory->decode(rawInstruction);
      assert(instr);

      if (issue(instr) && !stallIssue)
      {
        pc += 4;
      }
    }
  }
}

bool Tomasulo::issue(InstructionPtr instr)
{
  // check for a halt
  if (instr->getName() == InstructionName::TRAP && instr->getImmediate() == 0)
  {
    halted = true;
    logger->info(TAG, "Halting when instructions complete");
    return true;
  }

  return functionalUnits[instr->getType()]->issue(instr);
}

void Tomasulo::execute()
{
  for (auto fu : functionalUnits)
  {
    fu.second->execute();
  }
}

void Tomasulo::write()
{
  for (auto fu : functionalUnits)
  {
    fu.second->write();
  }
  commonDataBus->notifyAll();
  commonDataBus->writeAndClear();
}

bool Tomasulo::functionalUnitsIdle() const
{
  for (auto fu : functionalUnits)
  {
    if (!fu.second->idle())
    {
      return false;
    }
  }

  return true;
}
