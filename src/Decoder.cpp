#include "Decoder.h"
#include "log.h"
#include "utility/stream_manip.h"
#include <string>

static const std::string TAG = "Decoder";

InstructionData Decoder::decode(UWord rawInstruction)
{
  instruction = rawInstruction;
  out = {};

  logger->verbose(TAG) << "Decoding " << util::hex<UWord> << instruction;
  switch (getEncodingType(instruction >> (31 - 5)))
  {
  case InstructionEncodingType::Itype:
    decodeItype();
    break;

  case InstructionEncodingType::Rtype:
    decodeRtype();
    break;

  case InstructionEncodingType::Jtype:
    decodeJtype();
    break;
  }

  setRegisterTypes();
  return out;
}

void Decoder::decodeItype()
{
  logger->verbose(TAG, "Decoding as Itype");
  out.name = getName(instruction >> (31 - 5));
  out.type = getInstructionType(out.name);
  out.rd.index = (instruction >> (31 - 15)) & 0x1f;
  out.rs1.index = (instruction >> (31 - 10)) & 0x1f;
  out.rs2 = RegisterID::NONE;
  out.immediate = instruction & 0xffff;
}

void Decoder::decodeRtype()
{
  logger->verbose(TAG, "Decoding as Rtype");
  out.name = getName(instruction >> (31 - 5), instruction & 0x3f);
  out.type = getInstructionType(out.name);
  out.rd.index = (instruction >> (31 - 20)) & 0x1f;
  out.rs1.index = (instruction >> (31 - 10)) & 0x1f;
  out.rs2.index = (instruction >> (31 - 15)) & 0x1f;
  out.immediate = 0xffffffff;
}

void Decoder::decodeJtype()
{
  logger->verbose(TAG, "Decoding as Jtype");
  out.name = getName(instruction >> (31 - 5));
  out.type = getInstructionType(out.name);
  out.rd = RegisterID::NONE;
  out.rs1 = RegisterID::NONE;
  out.rs2 = RegisterID::NONE;
  out.immediate = instruction & 0x07ffffff;
}

void Decoder::setRegisterTypes()
{
  switch (out.name)
  {
    // trap is a special case
  case InstructionName::TRAP:
    if (out.immediate == 2)
    {
      out.rs1.type = RegisterType::FPR;
    }
    else
    {
      out.rs1.type = RegisterType::GPR;
    }
    break;

    // GPR rs1 only
  case InstructionName::BEQZ:
  case InstructionName::JR:
  case InstructionName::JALR:
    out.rs1.type = RegisterType::GPR;
    break;

    // GPR rd and rs1
  case InstructionName::ADDI:
  case InstructionName::LW:
  case InstructionName::SW:
    out.rd.type = RegisterType::GPR;
    out.rs1.type = RegisterType::GPR;
    break;

    // FPR rd, GPR rs1
  case InstructionName::LF:
  case InstructionName::SF:
  case InstructionName::MOVI2FP:
    out.rd.type = RegisterType::FPR;
    out.rs1.type = RegisterType::GPR;
    break;

    // GPR rd, FPR rs1
  case InstructionName::MOVFP2I:
    out.rd.type = RegisterType::GPR;
    out.rs1.type = RegisterType::FPR;
    break;

    // FPR rd and rs1
  case InstructionName::MOVF:
  case InstructionName::CVTF2I:
  case InstructionName::CVTI2F:
    out.rd.type = RegisterType::FPR;
    out.rs1.type = RegisterType::FPR;
    break;

    // 3x GPR
  case InstructionName::ADD:
  case InstructionName::SUB:
  case InstructionName::AND:
  case InstructionName::OR:
  case InstructionName::XOR:
    out.rd.type = RegisterType::GPR;
    out.rs1.type = RegisterType::GPR;
    out.rs2.type = RegisterType::GPR;
    break;

    // 3x FPR
  case InstructionName::ADDF:
  case InstructionName::SUBF:
  case InstructionName::MULTF:
  case InstructionName::DIVF:
  case InstructionName::MULT:
  case InstructionName::DIV:
    out.rd.type = RegisterType::FPR;
    out.rs1.type = RegisterType::FPR;
    out.rs2.type = RegisterType::FPR;
    break;
  }
}
