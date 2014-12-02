#include "FunctionalUnit.h"
#include <cassert>

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
  if (executeInOrder)
  {
    inOrderExecute();
  }
  else
  {
    outOfOrderExecute();
  }
}

void FunctionalUnit::write()
{
  auto pred = [&](ReservationStationPtr rs) {
    if (rs->getState() == ReservationStationState::WriteComplete)
    {
      rs->clearInstruction();
      idleStations.push_back(rs);
      return true;
    }

    return false;
  };
  writingStations.remove_if(pred);

  for (auto rs : writingStations)
  {
    rs->write();
  }
}

void FunctionalUnit::inOrderExecute()
{
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

  for (auto rs : executingStations)
  {
    rs->execute();
  }

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
}

void FunctionalUnit::outOfOrderExecute()
{
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

  auto pred2 = [&](ReservationStationPtr rs) {
    rs->execute();
    if (rs->getState() == ReservationStationState::ExecutionComplete)
    {
      rs->setIsWriting();
      writingStations.push_back(rs);
      return true;
    }
    return false;
  };
  executingStations.remove_if(pred2);
}
