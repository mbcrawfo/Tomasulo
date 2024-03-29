#ifndef __INSTRUCTION_TYPES_H__
#define __INSTRUCTION_TYPES_H__

#include "types.h"
#include <ostream>
#include <functional>

enum class FunctionalUnitType
{
  None,
  Integer,
  Trap,
  Branch,
  Memory,
  FloatingPoint
};

enum class InstructionEncodingType
{
  Itype,
  Rtype,
  Jtype
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
  DIV
};

/**
* Look up the type of an instruction.
*/
extern FunctionalUnitType getInstructionType(InstructionName name);

/**
 * Look up the name/mnemonic of an instruction given its opcode and function
 * code.
 */
extern InstructionName getName(Byte opcode, Byte funcode = 0);

/**
* Look up the encoding of an instruction given its opcode.
*/
extern InstructionEncodingType getEncodingType(Byte opcode);

std::ostream& operator<<(std::ostream& os, FunctionalUnitType type);
std::ostream& operator<<(std::ostream& os, InstructionEncodingType type);
std::ostream& operator<<(std::ostream& os, InstructionName name);

struct FunctionalUnitTypeHash
{
  std::size_t operator()(FunctionalUnitType type) const
  {
    return std::hash<std::size_t>()(static_cast<std::size_t>(type));
  }
};

#endif
