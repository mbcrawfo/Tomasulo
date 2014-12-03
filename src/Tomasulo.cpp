#include "Tomasulo.h"
#include "log.h"
#include "instructions/Instruction.h"
#include <cassert>
#include <string>

static const std::string TAG = "Tomasulo";

// machine configuration
static const int GPR_REGISTERS = 32;
static const int FPR_REGISTERS = 32;

static const int INTEGER_CYCLES = 1;
static const int INTEGER_STATIONS = 8;
static const int INTEGER_UNITS = 3;

static const int TRAP_CYCLES = 1;
static const int TRAP_STATIONS = 4;
static const int TRAP_UNITS = 1;

static const int MEMORY_CYCLES = 2;
static const int MEMORY_STATIONS = 8;
static const int MEMORY_UNITS = 1;

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

  // create all functional units
  ReservationStationDependencies deps(
    registerFile, renameRegisterFile, memory, pc, commonDataBus
    );
  functionalUnits[FunctionalUnitType::Integer] = 
    FunctionalUnitPtr(
      new FunctionalUnitManager(
        FunctionalUnitType::Integer, false, INTEGER_CYCLES, 
        INTEGER_STATIONS, INTEGER_UNITS, deps
        )
    );
  functionalUnits[FunctionalUnitType::Trap] =
    FunctionalUnitPtr(
      new FunctionalUnitManager(
        FunctionalUnitType::Trap, true, TRAP_CYCLES,
        TRAP_STATIONS, TRAP_UNITS, deps
        )
    );
  functionalUnits[FunctionalUnitType::Memory] =
    FunctionalUnitPtr(
      new FunctionalUnitManager(
        FunctionalUnitType::Memory, true, MEMORY_CYCLES, 
        MEMORY_STATIONS, MEMORY_UNITS, deps
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
    logger->info(TAG) << "****CLOCK CYCLE " << clockCounter << " BEGIN****";

    write();    
    execute();    
    if (!halted && !stallIssue)
    {
      UWord rawInstruction = memory->readUWord(pc);
      InstructionPtr instr = instructionFactory->decode(rawInstruction);
      assert(instr);

      if (issue(instr))
      {
        pc += 4;
      }
      else
      {
        logger->debug(TAG, "Issue failed, PC not modified");
      }
    }
    else
    {
      logger->debug(TAG, "Issue stalled");
    }

    logger->debug(TAG, "**CYCLE CLOSURE**");
    commonDataBus->notifyAll();
    commonDataBus->writeAndClear();
    advanceStates();
    logger->info(TAG) << "****CLOCK CYCLE " << clockCounter << " END****\n";
  }
}

bool Tomasulo::issue(InstructionPtr instr)
{
  logger->debug(TAG, "**ISSUE BEGIN**");
  bool result;

  // check for a halt
  if (instr->getName() == InstructionName::TRAP && instr->getImmediate() == 0)
  {
    halted = true;
    logger->info(TAG, "Halting when instructions complete");
    result = true;
  }
  else
  {
    result = functionalUnits[instr->getType()]->issue(instr);    
  }

  logger->debug(TAG, "**ISSUE END**");
  return result;
}

void Tomasulo::execute()
{
  logger->debug(TAG, "**EXECUTE BEGIN**");
  for (auto fu : functionalUnits)
  {
    fu.second->execute();
  }
  logger->debug(TAG, "**EXECUTE END**");
}

void Tomasulo::write()
{
  logger->debug(TAG, "**WRITE BEGIN**");
  for (auto fu : functionalUnits)
  {
    fu.second->write();
  }
  logger->debug(TAG, "**WRITE END**");
}

void Tomasulo::advanceStates()
{
  for (auto fu : functionalUnits)
  {
    fu.second->advanceStates();
  }
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
