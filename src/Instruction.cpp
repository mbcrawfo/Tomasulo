#include "Instruction.h"
#include "log.h"
#include "utility/stream_manip.h"
#include <string>

static const std::string TAG = "instruction";

static void decodeItype(UWord instruction, Instruction& out);
static void decodeRtype(UWord instruction, Instruction& out);
static void decodeJtype(UWord instruction, Instruction& out);

Instruction Instruction::decode(UWord instruction)
{
  Instruction out;

  logger->debug(TAG) << "Decoding " << util::hex<UWord> << instruction;
  switch (getEncodingType(instruction >> (31 - 5)))
  {
  case InstructionEncodingType::Itype:
    decodeItype(instruction, out);
    break;

  case InstructionEncodingType::RType:
    decodeRtype(instruction, out);
    break;

  case InstructionEncodingType::JType:
    decodeJtype(instruction, out);
    break;
  }

  return out;
}

void decodeItype(UWord instruction, Instruction& out)
{
  logger->debug(TAG, "Decoding as Itype");
  out.name = getName(instruction >> (31 - 5));
  out.type = getInstructionType(out.name);
  out.rd = (instruction >> (31 - 15)) & 0x1f;
  out.rs1 = (instruction >> (31 - 10)) & 0x1f;
  out.rs2 = 0;
  out.immediate = instruction & 0xffff;
}

void decodeRtype(UWord instruction, Instruction& out)
{
  logger->debug(TAG, "Decoding as Rtype");
  out.name = getName(instruction >> (31 - 5), instruction & 0x3f);
  out.type = getInstructionType(out.name);
  out.rd = (instruction >> (31 - 20)) & 0x1f;
  out.rs1 = (instruction >> (31 - 10)) & 0x1f;
  out.rs2 = (instruction >> (31 - 15)) & 0x1f;
  out.immediate = 0;
}

void decodeJtype(UWord instruction, Instruction& out)
{
  logger->debug(TAG, "Decoding as Jtype");
  out.name = getName(instruction >> (31 - 5));
  out.type = getInstructionType(out.name);
  out.rd = 0;
  out.rs1 = 0;
  out.rs2 = 0;
  out.immediate = instruction & 0x07ffffff;
}
