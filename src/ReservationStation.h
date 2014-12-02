#ifndef __RESERVATIONSTATION_H__
#define __RESERVATIONSTATION_H__

#include "types.h"
#include "ReservationStationID.h"
#include "RegisterFile.h"
#include "RenameRegisterFile.h"
#include "instructions/Instruction.h"
#include "Memory.h"
#include "CommonDataBus.h"

enum class ReservationStationState
{
  Idle,
  WaitingForArgs,
  ReadyToExecute,
  Executing,
  ExecutionComplete,
  Writing,
  WriteComplete
};

struct ReservationStationDependencies
{
  ReservationStationDependencies(RegisterFilePtr registers,
    RenameRegisterFilePtr renameRegisters,
    MemoryPtr memory,
    std::size_t& pc,
    CommonDataBusPtr cdb
    );
  ReservationStationDependencies& operator=(ReservationStationDependencies&) 
    = delete;

  RegisterFilePtr registers;
  RenameRegisterFilePtr renameRegisters;
  MemoryPtr memory;
  std::size_t& pc;
  CommonDataBusPtr cdb;
};

class ReservationStation
{
private:  
  const ReservationStationID id;
  ReservationStationDependencies deps;
  InstructionPtr instruction;
  ReservationStationState state;

  const std::size_t executeCycles;
  std::size_t executeCyclesRemaining;
  Data arg1;
  bool arg1Ready;
  ReservationStationID arg1Source;
  Data arg2;
  bool arg2Ready;
  ReservationStationID arg2Source;
  Data result;

public:
  ReservationStation() = delete;
  ReservationStation(const ReservationStationID& id, 
    const std::size_t executeCycles, ReservationStationDependencies deps);
  ReservationStation& operator=(ReservationStation&) = delete;

  ReservationStationID getID() const;
  ReservationStationState getState() const;

  void setInstruction(InstructionPtr instr);
  void clearInstruction();

  void setIsExecuting();
  void execute();
  void setIsWriting();
  void write();

  bool notify(const ReservationStationID& rsid, Data value);

private:
  void setArgSources();
};

#endif
