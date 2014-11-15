#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "types.h"
#include "instruction_types.h"

class Instruction
{
private:
  Instruction() = default;

public:
  static Instruction decode(UWord instruction);

  InstructionName name;
  InstructionType type;  
  Byte rd;
  Byte rs1;
  Byte rs2;
  UWord immediate;
};

#endif
