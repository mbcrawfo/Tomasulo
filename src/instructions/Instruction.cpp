#include "Instruction.h"
#include "log.h"

#include <string>

static const std::string TAG = "Instruction";

InstructionName Instruction::getName() const
{
  return name;
}

FunctionalUnitType Instruction::getType() const
{
  return type;
}

RegisterID Instruction::getRd() const
{
  return rd;
}

RegisterID Instruction::getRs1() const
{
  return rs1;
}

RegisterID Instruction::getRs2() const
{
  return rs2;
}

UWord Instruction::getImmediate() const
{
  return immediate;
}

WriteAction Instruction::getWriteAction() const
{
  return WriteAction::None;
}

std::ostream& operator<<(std::ostream& os, const Instruction& data)
{
  os << data.name << " rd=" << data.rd << " rs1=" << data.rs1 << " rs2="
    << data.rs2 << " imm=" << data.immediate;
  return os;
}
