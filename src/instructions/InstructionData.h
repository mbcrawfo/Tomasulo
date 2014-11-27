#ifndef __INSTRUCTIONDATA_H__
#define __INSTRUCTIONDATA_H__

#include "types.h"
#include "instructions/instruction_types.h"
#include "RegisterID.h"
#include <ostream>

/**
 * Holds the raw source data for an instruction.
 */
struct InstructionData
{
  InstructionName name;
  FunctionalUnitType type;
  RegisterID rd;
  RegisterID rs1;
  RegisterID rs2;
  UWord immediate;
};

inline std::ostream& operator<<(std::ostream& os, const InstructionData& data)
{
  os << data.name << " rd=" << data.rd << " rs1=" << data.rs1 << " rs2=" 
    << data.rs2 << " imm=" << data.immediate;
  return os;
}

#endif
