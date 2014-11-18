#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "types.h"
#include "instruction_types.h"
#include "utility/IToString.h"
#include <memory>

class Instruction;
using StrongInstructionPtr = std::shared_ptr<Instruction>;

class Instruction
  : public util::IToString
{
private:
  Instruction();

public:
  static StrongInstructionPtr decode(UWord instruction);

  InstructionName name;
  FunctionalUnitType type;  
  Byte rd;
  Byte rs1;
  Byte rs2;
  UWord immediate;

  virtual std::string toString() const override;
};

#endif
