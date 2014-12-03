#include "FunctionalUnitManager.h"
#include "log.h"
#include <string>
#include <cassert>
#include <algorithm>

static const std::string TAG = "FunctionalUnitManager";

FunctionalUnitManager::FunctionalUnitManager(FunctionalUnitType type, 
  bool executeInOrder,
  std::size_t executeCycles,
  std::size_t numStations,
  std::size_t numExecuteUnits,
  ReservationStationDependencies& deps)
  : type(type),
    executeInOrder(executeInOrder),
    numExecuteUnits(numExecuteUnits),
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

bool FunctionalUnitManager::idle() const
{
  return issuedStations.empty() && executingStations.empty() 
    && writingStations.empty();
}

bool FunctionalUnitManager::issue(InstructionPtr instruction)
{
  assert(instruction != nullptr);

  if (idleStations.empty())
  {
    logger->debug(TAG) << type << " stations full, cannot issue " 
      << instruction->getName();
    return false;
  }

  auto rs = idleStations.front();
  idleStations.pop_front();
  rs->setInstruction(instruction);
  issuedStations.push_back(rs);
  return true;
}

void FunctionalUnitManager::execute()
{
  for (auto rs : executingStations)
  {
    rs->execute();
  }
}

void FunctionalUnitManager::write()
{
  for (auto rs : writingStations)
  {
    rs->write();
  }
}

void FunctionalUnitManager::advanceStates()
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

bool FunctionalUnitManager::executeUnitsAvailable()
{
  return (executingStations.size() + writingStations.size()) < numExecuteUnits;
}

void FunctionalUnitManager::inOrderAdvance()
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

    if (!executeUnitsAvailable())
    {
      auto func = [&](ReservationStationPtr rs) {
        if (rs->getState() == ReservationStationState::ReadyToExecute)
        {
          logger->debug(TAG) << type << " execute units full, " << rs->getID()
            << " waiting";
        }
      };
      std::for_each(issuedStations.begin(), issuedStations.end(), func);      
      break;
    }

    issuedStations.pop_front();
    rs->setIsExecuting();
    executingStations.push_back(rs);
  }
}

void FunctionalUnitManager::outOfOrderAdvance()
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
      if (!executeUnitsAvailable())
      {
        logger->debug(TAG) << type << " execute units full, " << rs->getID()
          << " waiting";
        return false;
      }
      rs->setIsExecuting();
      executingStations.push_back(rs);
      return true;
    }
    return false;
  };
  issuedStations.remove_if(pred);  
}
