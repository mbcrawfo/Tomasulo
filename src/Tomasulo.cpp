#include "Tomasulo.h"
#include "log.h"
#include "instructions/Instruction.h"
#include "utility/stream_manip.h"
#include <cassert>
#include <string>
#include <iostream>
#include <iomanip>

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

  logger->debug(TAG) << "Executing from address "
    << util::hex<Address> << entryPoint << "\n";

  while (!halted || !functionalUnitsIdle())
  {
    ++clockCounter;
    logger->info(TAG) << "****CLOCK CYCLE " << clockCounter << " BEGIN****";
    
    advanceInstructions();            
    issue();
    execute();
    write();

    dumpState();        
    logger->info(TAG) << "****CLOCK CYCLE " << clockCounter << " END****\n";
  }
}

void Tomasulo::issue()
{
  logger->debug(TAG, "**ISSUE BEGIN**");  

  if (!halted && !stallIssue)
  {
    bool advancePC = true;
    UWord rawInstruction = memory->readUWord(pc);
    InstructionPtr instruction = instructionFactory->decode(rawInstruction);
    assert(instruction);

    logger->debug(TAG) << "Decoded " << *instruction;

    // check for a halt
    if (instruction->getName() == InstructionName::TRAP
      && instruction->getImmediate() == 0)
    {
      halted = true;
      logger->info(TAG, "Halting when issued instructions are completed");
      advancePC = false;
    }
    else
    {
      advancePC = functionalUnits[instruction->getType()]->issue(instruction);
    }

    if (advancePC)
    {
      pc += 4;
    }
    else
    {
      logger->debug(TAG, "PC was not incremented");
    }
  }
  else
  {
    logger->debug(TAG, "Issue stalled");
  }  

  logger->debug(TAG, "**ISSUE END**");
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
  commonDataBus->commit();
  logger->debug(TAG, "**WRITE END**");
}

void Tomasulo::advanceInstructions()
{
  for (auto fu : functionalUnits)
  {
    fu.second->advanceInstructions();
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

void Tomasulo::dumpState() const
{
  if (!verbose)
  {
    return;
  }

  std::cout << "\nClock cycle: " << std::dec << clockCounter << std::endl;
  std::cout << "\t" << "PC=" << util::hex<Address> << pc << std::endl;
  std::cout << "\t" << "Issue Stalled=" << (stallIssue ? "Y" : "N") << std::endl;
  std::cout << "\t" << "Halted=" << (halted ? "Y" : "N") << std::endl;

  for (auto fu : functionalUnits)
  {
    fu.second->dumpState();
  }

  commonDataBus->dumpState();
  dumpRegisters();
}

void Tomasulo::dumpRegisters() const
{
  RegisterID reg = RegisterID::R0;

  std::cout << "R0-R7: ";
  for (std::size_t i = 0; i < 8; i++)
  {
    reg.index = i;
    auto rename = renameRegisterFile->getRenaming(reg);
    if (rename == ReservationStationID::NONE)
    {
      std::cout << util::hex<UWord> << registerFile->read(reg).uw << " ";
    }
    else
    {
      std::cout << rename << " ";
    }
  }
  std::cout << std::endl;

  reg.type = RegisterType::FPR;
  std::cout << "F0-F7: ";
  for (std::size_t i = 0; i < 8; i++)
  {
    reg.index = i;
    auto rename = renameRegisterFile->getRenaming(reg);
    if (rename == ReservationStationID::NONE)
    {
      std::cout << registerFile->read(reg).f << " ";
    }
    else
    {
      std::cout << rename << " ";
    }
  }
  std::cout << std::endl;
}
