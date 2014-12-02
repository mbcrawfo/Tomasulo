#ifndef __TOMASULO_H__
#define __TOMASULO_H__

#include "types.h"
#include "Memory.h"
#include "RegisterID.h"
#include "ReservationStationID.h"
#include "instructions/InstructionFactory.h"
#include "instructions/Instruction.h"
#include "RegisterFile.h"
#include <unordered_map>

class Tomasulo
{
private:
  // general
  bool verbose;
  InstructionFactory iFactory;
  // machine state
  bool halted;
  bool stallIssue;
  std::size_t clockCounter;
  std::size_t instructionsInProgress;
  Address pc;
  // components
  MemoryPtr memory;
  RegisterFile registerFile;
  std::unordered_map<RegisterID, ReservationStationID> renameRegFile;

public:
  explicit Tomasulo(MemoryPtr memory, bool verbose = false);

  bool isHalted() const;
  std::size_t clocks() const;

  void run(Address entryPoint = 0);

private:
  bool issue(InstructionPtr instr);
  void execute();
  void write();
};

#endif
