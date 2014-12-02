#ifndef __INSTRUCTIONFACTORY_H__
#define __INSTRUCTIONFACTORY_H__

#include "instructions/Instruction.h"

class InstructionFactory
{
private:
  UWord instruction;
  InstructionName name;
  InstructionEncodingType encodingType;
  FunctionalUnitType fuType;
  InstructionPtr result;

public:
  InstructionFactory() = default;

  InstructionPtr decode(UWord rawInstruction);

private:
  void createInstruction();
  void decodeItype();
  void decodeRtype();
  void decodeJtype();
  void setRegisterTypes();
};

#endif
