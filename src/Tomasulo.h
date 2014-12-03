#ifndef __TOMASULO_H__
#define __TOMASULO_H__

#include "types.h"
#include "Memory.h"
#include "instructions/InstructionFactory.h"
#include "RegisterFile.h"
#include "RenameRegisterFile.h"
#include "CommonDataBus.h"
#include "FunctionalUnit.h"
#include <unordered_map>

class Tomasulo
{
private:
  // general
  bool verbose;
  InstructionFactoryPtr instructionFactory;
  // machine state
  bool halted;
  bool stallIssue;
  std::size_t clockCounter;
  Address pc;
  // components
  MemoryPtr memory;
  RegisterFilePtr registerFile;
  RenameRegisterFilePtr renameRegisterFile;
  CommonDataBusPtr commonDataBus;
  std::unordered_map<FunctionalUnitType, FunctionalUnitPtr, FunctionalUnitTypeHash>
    functionalUnits;

public:
  explicit Tomasulo(MemoryPtr memory, bool verbose = false);

  bool isHalted() const;
  std::size_t clocks() const;

  void run(Address entryPoint = 0);

private:
  void issue();
  void execute();
  void write();
  void advanceInstructions();
  bool functionalUnitsIdle() const;
  void dumpState() const;
  void dumpRegisters() const;
};

#endif
