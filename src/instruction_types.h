#ifndef __INSTRUCTION_TYPES_H__
#define __INSTRUCTION_TYPES_H__

#include "types.h"

enum class InstructionType
{
  Integer,
  Trap,
  Branch,
  Memory,
  FloatingPoint
};

enum class InstructionEncodingType
{
  Itype,
  RType,
  JType
};

enum class InstructionName
{
  // Jtypes
  J, 
  JAL,

  // Itypes
  BEQZ,
  ADDI,
  TRAP,
  JR,
  JALR,
  LW,
  LF,
  SW,
  SF,

  // Rtypes
  NOP,
  ADD,
  SUB,
  AND,
  OR,
  XOR,
  MOVF,
  MOVFP2I,
  MOVI2FP,
  ADDF,
  SUBF,
  MULTF,
  DIVF,
  CVTF2I,
  CVTI2F,
  MULT,
  DIV,
};

extern InstructionEncodingType getEncodingType(Byte opcode);
extern InstructionName getName(Byte opcode, Byte funcode = 0);
extern InstructionType getInstructionType(InstructionName name);

#endif
