#ifndef __FUNCTIONALUNIT_H__
#define __FUNCTIONALUINT_H__

#include "types.h"
#include "ReservationStation.h"
#include "instructions/Instruction.h"
#include <list>

class FunctionalUnit;
using FunctionalUnitPtr = Pointer<FunctionalUnit>;

class FunctionalUnit
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
  FunctionalUnit(FunctionalUnitType type, 
    bool executeInOrder,
    std::size_t executeCycles,
    std::size_t numStations,
    std::size_t numExecuteUnits,
    ReservationStationDependencies& deps);
  FunctionalUnit& operator=(FunctionalUnit&);

  bool idle() const;

  bool issue(InstructionPtr instruction, std::size_t clock);
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
