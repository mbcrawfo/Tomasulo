#include "Instruction.h"
#include "log.h"
#include "utility/stream_manip.h"
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

RegisterID Instruction::getDest() const
{
  return rd;
}

RegisterID Instruction::getArg1() const
{
  return rs1;
}

RegisterID Instruction::getArg2() const
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
    << data.rs2 << " imm=" << util::hex<UWord> << data.immediate;
  return os;
}
