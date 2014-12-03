#include "FunctionalUnit.h"
#include "log.h"
#include <string>
#include <cassert>

static const std::string TAG = "FunctionalUnit";

FunctionalUnit::FunctionalUnit(FunctionalUnitType type, 
  bool executeInOrder,
  std::size_t executeCycles,
  std::size_t numStations, 
  ReservationStationDependencies& deps)
  : type(type),
    executeInOrder(executeInOrder),
    idleStations(),
    issuedStations(),
    executingStations(),
    writingStations()
{
  for (std::size_t i = 0; i < numStations; i++)
  {
    ReservationStationID id = { type, i };
    idleStations.push_back(      
      ReservationStationPtr(new ReservationStation(id, executeCycles, deps))
      );
  }
}

bool FunctionalUnit::idle() const
{
  return issuedStations.empty() && executingStations.empty() 
    && writingStations.empty();
}

bool FunctionalUnit::issue(InstructionPtr instruction)
{
  assert(instruction != nullptr);

  if (idleStations.empty())
  {
    logger->debug(TAG) << type << " full, cannot issue " 
      << instruction->getName();
    return false;
  }

  auto rs = idleStations.front();
  idleStations.pop_front();
  rs->setInstruction(instruction);
  issuedStations.push_back(rs);
  return true;
}

void FunctionalUnit::execute()
{
  for (auto rs : executingStations)
  {
    rs->execute();
  }
}

void FunctionalUnit::write()
{
  for (auto rs : writingStations)
  {
    rs->write();
  }
}

void FunctionalUnit::advanceStates()
{
  // retire completed
  auto writePred = [&](ReservationStationPtr rs) {
    if (rs->getState() == ReservationStationState::WriteComplete)
    {
      rs->clearInstruction();
      idleStations.push_back(rs);
      return true;
    }

    return false;
  };
  writingStations.remove_if(writePred);

  if (executeInOrder)
  {
    inOrderAdvance();
  }
  else
  {
    outOfOrderAdvance();
  }
}

void FunctionalUnit::inOrderAdvance()
{ 
  // move from execute to write
  while (!executingStations.empty())
  {
    auto rs = executingStations.front();
    if (rs->getState() != ReservationStationState::ExecutionComplete)
    {
      break;
    }

    executingStations.pop_front();
    rs->setIsWriting();
    writingStations.push_back(rs);
  }

  // move from issued to execute
  while (!issuedStations.empty())
  {
    auto rs = issuedStations.front();
    if (rs->getState() != ReservationStationState::ReadyToExecute)
    {
      break;
    }

    issuedStations.pop_front();
    rs->setIsExecuting();
    executingStations.push_back(rs);
  }
}

void FunctionalUnit::outOfOrderAdvance()
{
  // move from execute to write
  auto pred2 = [&](ReservationStationPtr rs) {
    if (rs->getState() == ReservationStationState::ExecutionComplete)
    {
      rs->setIsWriting();
      writingStations.push_back(rs);
      return true;
    }
    return false;
  };
  executingStations.remove_if(pred2);

  // move from issued to execute
  auto pred = [&](ReservationStationPtr rs) {
    if (rs->getState() == ReservationStationState::ReadyToExecute)
    {
      rs->setIsExecuting();
      executingStations.push_back(rs);
      return true;
    }
    return false;
  };
  issuedStations.remove_if(pred);  
}
