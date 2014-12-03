#include "ReservationStation.h"
#include "log.h"
#include "utility/stream_manip.h"
#include <string>
#include <cassert>
#include <iostream>

static const std::string TAG = "ReservationStation";

ReservationStationDependencies::ReservationStationDependencies(
  RegisterFilePtr registers, 
  RenameRegisterFilePtr renameRegisters, 
  MemoryPtr memory, 
  Address& pc,
  CommonDataBusPtr cdb)
  : registers(registers),
    renameRegisters(renameRegisters),
    memory(memory),
    pc(pc),
    cdb(cdb)
{
}

ReservationStation::ReservationStation(const ReservationStationID& id,
  const std::size_t executeCycles, ReservationStationDependencies& deps)
  : id(id),
    deps(deps),
    instruction(nullptr),
    state(ReservationStationState::Idle),
    executeCycles(executeCycles),
    startClock(0),
    executeCyclesRemaining(0),
    arg1(),
    arg1Ready(false),
    arg1Source(ReservationStationID::NONE),
    arg2(),
    arg2Ready(false),
    arg2Source(ReservationStationID::NONE),
    result()
{
}

ReservationStationID ReservationStation::getID() const
{
  return id;
}

ReservationStationState ReservationStation::getState() const
{
  return state;
}

std::size_t ReservationStation::getStartClock() const
{
  return startClock;
}

RegisterID ReservationStation::getDest() const
{
  return instruction->getDest();
}

Data ReservationStation::getResult() const
{
  return result;
}

void ReservationStation::setInstruction(InstructionPtr instr, std::size_t clock)
{
  instruction = instr;
  logger->debug(TAG) << id << " was issued " << instruction->getName();

  startClock = clock;
  executeCyclesRemaining = executeCycles;
  arg1.uw = 0;
  arg1Ready = false;
  arg1Source = ReservationStationID::NONE;
  arg2.uw = 0;
  arg2Ready = false;
  arg2Source = ReservationStationID::NONE;
  result.uw = 0;

  setArgSources();
  if (arg1Ready && arg2Ready)
  {
    state = ReservationStationState::ReadyToExecute;
    logger->debug(TAG) << id << " has read all arguments";
  }
  else
  {
    deps.cdb->addListener(this);
    state = ReservationStationState::WaitingForArgs;
  }

  if (instruction->getDest() != RegisterID::NONE &&
    instruction->getDest() != RegisterID::R0)
  {
    deps.renameRegisters->rename(instruction->getDest(), id);
  }
}

void ReservationStation::clearInstruction()
{
  deps.renameRegisters->clearRename(id);
  instruction = InstructionPtr();
  state = ReservationStationState::Idle;
  logger->debug(TAG) << id << " cleared";
}

void ReservationStation::setIsExecuting()
{
  state = ReservationStationState::Executing;
  logger->debug(TAG) << id << " moved to execute stage";
}

void ReservationStation::execute()
{
  executeCyclesRemaining--;
  if (executeCyclesRemaining == 0)
  {
    result = instruction->execute(arg1, arg2);
    state = ReservationStationState::ExecutionComplete;
    logger->debug(TAG) << id << " completed execution";
  }
  else
  {
    logger->debug(TAG) << id << " has " << executeCyclesRemaining
      << " cycles left";
  }
}

void ReservationStation::setIsWriting()
{
  state = ReservationStationState::Writing;
  logger->debug(TAG) << id << " moved to write stage";
}

void ReservationStation::write()
{
  switch (instruction->getWriteAction())
  {
  case WriteAction::None:
    state = ReservationStationState::WriteComplete;
    break;

  case WriteAction::Register:
    deps.cdb->write(this);
    break;

  case WriteAction::PC:
    //deps.pc = result.uw;
    //state = ReservationStationState::WriteComplete;
    break;

  case WriteAction::Memory:
    deps.memory->writeUWord(result.uw, arg2.uw);
    state = ReservationStationState::WriteComplete;
    logger->debug(TAG) << id << " wrote value " << util::hex<UWord> << arg2.uw
      << " to address " << util::hex<UWord> << result.uw;
    break;
  }
}

void ReservationStation::dumpState() const
{
  switch (state)
  {
  case ReservationStationState::Idle:
    //std::cout << "\t" << id << ": idle" << std::endl;
    break;

  case ReservationStationState::WaitingForArgs:
    std::cout << "\t" << id << ": " << instruction->getName() 
      << ", waiting for ";
    if (!arg1Ready)
    {
      std::cout << arg1Source;
    }
    if (!arg1Ready && !arg2Ready)
    {
      std::cout << " and ";
    }
    if (!arg2Ready)
    {
      std::cout << arg2Source;
    }
    std::cout << std::endl;
    break;

  case ReservationStationState::ReadyToExecute:  
    std::cout << "\t" << id << ": " << instruction->getName()
      << ", ready to execute" << std::endl;
    break;

  case ReservationStationState::Executing:
  case ReservationStationState::ExecutionComplete:
    std::cout << "\t" << id << ": " << instruction->getName()
      << ", executing" << std::endl;
    break;

  case ReservationStationState::Writing:
  case ReservationStationState::WriteComplete:
    std::cout << "\t" << id << ": " << instruction->getName()
      << ", writing" << std::endl;
    break;
      
  default:
    break;
  }
}

bool ReservationStation::notifyDataBus(const ReservationStationID& rsid,
  Data value)
{
  if (!arg1Ready && rsid == arg1Source)
  {
    arg1 = value;
    arg1Ready = true;
    arg1Source = ReservationStationID::NONE;
    logger->debug(TAG) << id << " captured " << instruction->getArg1() << "=" 
      << util::hex<UWord> << arg1.uw << " from " << rsid << " via CDB";
  }
  if (!arg2Ready && rsid == arg2Source)
  {
    arg2 = value;
    arg2Ready = true;
    arg2Source = ReservationStationID::NONE;
    logger->debug(TAG) << id << " captured " << instruction->getArg2() << "=" 
      << util::hex<UWord> << arg2.uw << " from " << rsid << " via CDB";
  }

  if (arg1Ready && arg2Ready)
  {
    state = ReservationStationState::ReadyToExecute;
    logger->debug(TAG) << id << " has read all arguments";
    return true;
  }

  return false;
}

void ReservationStation::notifyWriteAccepted()
{
  assert(state == ReservationStationState::Writing);
  state = ReservationStationState::WriteComplete;
}

void ReservationStation::setArgSources()
{
  auto rs1 = instruction->getArg1();
  if (rs1 == RegisterID::NONE)
  {
    arg1Ready = true;
  }
  else
  {
    auto rename = deps.renameRegisters->getRenaming(rs1);
    if (rename != ReservationStationID::NONE)
    {
      arg1Source = rename;
      logger->debug(TAG) << id << " is waiting to read " << rs1 
        << " from " << rename;
    }
    else
    {
      arg1 = deps.registers->read(rs1);
      arg1Ready = true;
      logger->debug(TAG) << id << " read " << rs1 << "=" 
        << util::hex<UWord> << arg1.uw;
    }
  }

  auto rs2 = instruction->getArg2();
  if (rs2 == RegisterID::NONE)
  {
    arg2Ready = true;
  }
  else
  {
    auto rename = deps.renameRegisters->getRenaming(rs2);
    if (rename != ReservationStationID::NONE)
    {
      arg2Source = rename;
      logger->debug(TAG) << id << " is waiting to read " << rs2 
        << " from " << rename;
    }
    else
    {
      arg2 = deps.registers->read(rs2);
      arg2Ready = true;
      logger->debug(TAG) << id << " read " << rs2 << "=" 
        << util::hex<UWord> << arg2.uw;
    }
  }
}
