#include "Instruction.h"
#include "log.h"
#include "utility/stream_manip.h"
#include <string>
#include <sstream>

static const std::string TAG = "instruction";

static void decodeItype(UWord instruction, Instruction& out);
static void decodeRtype(UWord instruction, Instruction& out);
static void decodeJtype(UWord instruction, Instruction& out);

Instruction::Instruction()
  : name(InstructionName::NOP), type(FunctionalUnitType::Integer),
    rd(0xff), rs1(0xff), rs2(0xff), immediate(0xffffffff)
{
}

StrongInstructionPtr Instruction::decode(UWord instruction)
{
  StrongInstructionPtr out(new Instruction);

  //logger->debug(TAG) << "Decoding " << util::hex<UWord> << instruction;
  switch (getEncodingType(instruction >> (31 - 5)))
  {
  case InstructionEncodingType::Itype:
    decodeItype(instruction, *out);
    break;

  case InstructionEncodingType::Rtype:
    decodeRtype(instruction, *out);
    break;

  case InstructionEncodingType::Jtype:
    decodeJtype(instruction, *out);
    break;
  }

  return out;
}

std::string Instruction::toString() const
{
  std::ostringstream os;
  os << name << " rd=" << static_cast<int>(rd) << " rs1="
    << static_cast<int>(rs1) << " rs2=" << static_cast<int>(rs2) 
    << " imm=" << immediate;
  return os.str();
}

void decodeItype(UWord instruction, Instruction& out)
{
  //logger->debug(TAG, "Decoding as Itype");
  out.name = getName(instruction >> (31 - 5));
  out.type = getInstructionType(out.name);
  out.rd = (instruction >> (31 - 15)) & 0x1f;
  out.rs1 = (instruction >> (31 - 10)) & 0x1f;
  out.rs2 = 0xff;
  out.immediate = instruction & 0xffff;
}

void decodeRtype(UWord instruction, Instruction& out)
{
  //logger->debug(TAG, "Decoding as Rtype");
  out.name = getName(instruction >> (31 - 5), instruction & 0x3f);
  out.type = getInstructionType(out.name);
  out.rd = (instruction >> (31 - 20)) & 0x1f;
  out.rs1 = (instruction >> (31 - 10)) & 0x1f;
  out.rs2 = (instruction >> (31 - 15)) & 0x1f;
  out.immediate = 0xffffffff;
}

void decodeJtype(UWord instruction, Instruction& out)
{
  //logger->debug(TAG, "Decoding as Jtype");
  out.name = getName(instruction >> (31 - 5));
  out.type = getInstructionType(out.name);
  out.rd = 0xff;
  out.rs1 = 0xff;
  out.rs2 = 0xff;
  out.immediate = instruction & 0x07ffffff;
}
