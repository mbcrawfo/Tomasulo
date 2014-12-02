#ifndef __INSTRUCTIONFACTORY_H__
#define __INSTRUCTIONFACTORY_H__

#include "instructions/Instruction.h"
#include "Memory.h"
#include "RegisterFile.h"

class InstructionFactory;
using InstructionFactoryPtr = Pointer<InstructionFactory>;

class InstructionFactory
{
private:
  MemoryPtr memory;
  RegisterFilePtr registers;

  UWord instruction;
  InstructionName name;
  InstructionEncodingType encodingType;
  FunctionalUnitType fuType;
  InstructionPtr result;

public:
  explicit InstructionFactory(MemoryPtr memory, RegisterFilePtr registers);

  InstructionPtr decode(UWord rawInstruction);

private:
  void createInstruction();
  void decodeItype();
  void decodeRtype();
  void decodeJtype();
  void setRegisterTypes();
};

#endif
