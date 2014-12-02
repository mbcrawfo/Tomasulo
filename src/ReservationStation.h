#ifndef __RESERVATIONSTATION_H__
#define __RESERVATIONSTATION_H__

#include "types.h"
#include "ReservationStationID.h"
#include "RegisterFile.h"
#include "RenameRegisterFile.h"
#include "instructions/Instruction.h"
#include "Memory.h"

class ReservationStation;
using ReservationStationPtr = Pointer<ReservationStation>;

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
  RegisterFile& registers;
  RenameRegisterFile& renameRegisters;
  Memory& memory;
  std::size_t& pc;
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

  void notify(const ReservationStationID& rsid, Data value);

private:
  void setArgSources();
};

#endif
