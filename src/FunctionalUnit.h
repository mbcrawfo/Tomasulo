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
  using RSList = std::list<ReservationStationPtr>;

  FunctionalUnitType type;
  bool executeInOrder;
  RSList idleStations;
  RSList issuedStations;
  RSList executingStations;
  RSList writingStations;

public:
  FunctionalUnit(FunctionalUnitType type, 
    bool executeInOrder,
    std::size_t executeCycles,
    std::size_t numStations,
    ReservationStationDependencies& deps);

  bool idle() const;

  bool issue(InstructionPtr instruction);
  void execute();
  void write();
  void advanceStates();

private:
  void inOrderAdvance();
  void outOfOrderAdvance();
};

#endif
