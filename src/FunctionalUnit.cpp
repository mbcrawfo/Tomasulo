#include "FunctionalUnit.h"
#include "log.h"
#include <string>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <iomanip>

static const std::string TAG = "FunctionalUnit";

FunctionalUnit::FunctionalUnit(FunctionalUnitType type, 
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
    auto rs = ReservationStationPtr(
      new ReservationStation(id, executeCycles, deps)
      );
    allStations.push_back(rs);
    idleStations.push_back(rs);
  }
}

bool FunctionalUnit::idle() const
{
  return issuedStations.empty() && executingStations.empty() 
    && writingStations.empty();
}

bool FunctionalUnit::issue(InstructionPtr instruction, std::size_t clock)
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
  rs->setInstruction(instruction, clock);
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

void FunctionalUnit::advanceInstructions()
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

void FunctionalUnit::dumpState() const
{
  auto idle = idleStations.size();
  auto used = issuedStations.size() + executingStations.size() 
    + writingStations.size();
  auto unitsUsed = used - issuedStations.size();
  std::cout << type << " Functional Unit" << std::endl;
  std::cout << "\t" << "Stations: " << std::dec << used << " in use, "
    << idle << " idle" << std::endl;
  std::cout << "\t" << "ExecuteUnits: " << unitsUsed << " in use, " 
    << (numExecuteUnits - unitsUsed) << " idle" << std::endl;

  for (auto rs : allStations)
  {
    rs->dumpState();
  }
}

bool FunctionalUnit::executeUnitsAvailable()
{
  return (executingStations.size() + writingStations.size()) < numExecuteUnits;
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

void FunctionalUnit::outOfOrderAdvance()
{
  // move from execute to write
  auto execPred = [&](ReservationStationPtr rs) {
    if (rs->getState() == ReservationStationState::ExecutionComplete)
    {
      rs->setIsWriting();
      writingStations.push_back(rs);
      return true;
    }
    return false;
  };
  executingStations.remove_if(execPred);

  // move from issued to execute
  auto issuePred = [&](ReservationStationPtr rs) {
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
  issuedStations.remove_if(issuePred);
}
