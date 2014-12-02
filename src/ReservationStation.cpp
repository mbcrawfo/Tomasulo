#include "ReservationStation.h"
#include "log.h"
#include "utility/stream_manip.h"
#include <string>
#include <cassert>

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

void ReservationStation::setInstruction(InstructionPtr instr)
{
  instruction = instr;
  logger->debug(TAG) << id << " beginning " << *instruction;

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
    logger->debug(TAG) << id << " has all args";
  }
  else
  {
    deps.cdb->addListener(this);
    state = ReservationStationState::WaitingForArgs;
  }

  if (instruction->getRd() != RegisterID::NONE &&
    instruction->getRd() != RegisterID::R0)
  {
    deps.renameRegisters->rename(instruction->getRd(), id);
  }
}

void ReservationStation::clearInstruction()
{
  instruction = InstructionPtr();
  state = ReservationStationState::Idle;
  logger->debug(TAG) << id << " cleared";
}

void ReservationStation::setIsExecuting()
{
  state = ReservationStationState::Executing;
  logger->debug(TAG) << id << " advanced to executing";
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
}

void ReservationStation::setIsWriting()
{
  state = ReservationStationState::Writing;
  logger->debug(TAG) << id << " advanced to writing";
}

void ReservationStation::write()
{
  switch (instruction->getWriteAction())
  {
  case WriteAction::None:
    state = ReservationStationState::WriteComplete;
    break;

  case WriteAction::Register:
    if (deps.cdb->set(id, instruction->getRd(), result))
    {
      state = ReservationStationState::WriteComplete;
    }
    break;

  case WriteAction::PC:
    //deps.pc = result.uw;
    //state = ReservationStationState::WriteComplete;
    break;

  case WriteAction::Memory:
    break;
  }
}

bool ReservationStation::notify(const ReservationStationID& rsid, Data value)
{
  if (!arg1Ready && rsid == arg1Source)
  {
    arg1 = value;
    arg1Ready = true;
    arg1Source = ReservationStationID::NONE;
    logger->debug(TAG) << id << " captured rs1=" << util::hex<UWord> << arg1.uw
      << " from " << rsid;
  }
  if (!arg2Ready && rsid == arg2Source)
  {
    arg2 = value;
    arg2Ready = true;
    arg2Source = ReservationStationID::NONE;
    logger->debug(TAG) << id << " captured rs2=" << util::hex<UWord> << arg2.uw
      << " from " << rsid;
  }

  if (arg1Ready && arg2Ready)
  {
    state = ReservationStationState::ReadyToExecute;
    logger->debug(TAG) << id << " has all args";
    return true;
  }

  return false;
}

void ReservationStation::setArgSources()
{
  auto rs1 = instruction->getRs1();
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
      logger->debug(TAG) << "Sourcing rs1 from " << rename;
    }
    else
    {
      arg1 = deps.registers->read(rs1);
      arg1Ready = true;
      logger->debug(TAG) << "Read rs1=" << util::hex<UWord> << arg1.uw
        << " from " << rs1;
    }
  }

  auto rs2 = instruction->getRs2();
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
      logger->debug(TAG) << "Sourcing rs2 from " << rename;
    }
    else
    {
      arg2 = deps.registers->read(rs2);
      arg2Ready = true;
      logger->debug(TAG) << "Read rs2=" << util::hex<UWord> << arg2.uw
        << " from " << rs1;
    }
  }
}
