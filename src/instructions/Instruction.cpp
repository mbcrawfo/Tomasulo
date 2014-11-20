#include "Instruction.h"
#include "log.h"
#include "utility/stream_manip.h"
#include <string>
#include <sstream>

static const std::string TAG = "instruction";

std::string Instruction::toString() const
{
  std::ostringstream os;
  os << name << " rd=" << rd << " rs1=" << rs1 << " rs2=" << rs2 
    << " imm=" << immediate;
  return os.str();
}

Instruction::Instruction()
  : name(InstructionName::NOP), type(FunctionalUnitType::Integer),
  rd(RegisterID::NONE), rs1(RegisterID::NONE), rs2(RegisterID::NONE),
  immediate(0xffffffff)
{}
