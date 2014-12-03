#ifndef __FUNCTIONALUNIT_H__
#define __FUNCTIONALUINT_H__

#include "types.h"
#include "ReservationStation.h"
#include "instructions/Instruction.h"
#include <list>

class FunctionalUnitManager;
using FunctionalUnitPtr = Pointer<FunctionalUnitManager>;

class FunctionalUnitManager
{
private:
  using ReservationStationList = std::list<ReservationStationPtr>;

  FunctionalUnitType type;
  bool executeInOrder;
  const std::size_t numExecuteUnits;
  ReservationStationList allStations;
  ReservationStationList idleStations;
  ReservationStationList issuedStations;
  ReservationStationList executingStations;
  ReservationStationList writingStations;

public:
  FunctionalUnitManager(FunctionalUnitType type, 
    bool executeInOrder,
    std::size_t executeCycles,
    std::size_t numStations,
    std::size_t numExecuteUnits,
    ReservationStationDependencies& deps);
  FunctionalUnitManager& operator=(FunctionalUnitManager&);

  bool idle() const;

  bool issue(InstructionPtr instruction);
  void execute();
  void write();
  void advanceInstructions();
  void dumpState() const;

private:
  bool executeUnitsAvailable();
  void inOrderAdvance();
  void outOfOrderAdvance();
};

#endif
