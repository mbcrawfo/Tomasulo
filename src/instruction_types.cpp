#include "instruction_types.h"
#include "log.h"
#include <string>

static const std::string TAG = "instruction_types";

InstructionEncodingType getEncodingType(Byte opcode)
{
  switch (opcode)
  {
  case 0:
  case 1:
    return InstructionEncodingType::RType;

  case 2:
  case 3:
    return InstructionEncodingType::JType;

  default:
    return InstructionEncodingType::Itype;
  }
}

InstructionName getName(Byte opcode, Byte funcode)
{
  switch (opcode)
  {
  case 0:
    switch (funcode)
    {
    case 0:
      return InstructionName::NOP;
    case 32:
      return InstructionName::ADD;
    case 34:
      return InstructionName::SUB;
    case 36:
      return InstructionName::AND;
    case 37:
      return InstructionName::OR;
    case 38:
      return InstructionName::XOR;
    case 50:
      return InstructionName::MOVF;
    case 52:
      return InstructionName::MOVFP2I;
    case 53:
      return InstructionName::MOVI2FP;

    default:
      break;
    }

  case 1:
    switch (funcode)
    {
    case 0:
      return InstructionName::ADDF;
    case 1:
      return InstructionName::SUBF;
    case 2:
      return InstructionName::MULTF;
    case 3:
      return InstructionName::DIVF;
    case 9:
      return InstructionName::CVTF2I;
    case 12:
      return InstructionName::CVTI2F;
    case 14:
      return InstructionName::MULT;
    case 15:
      return InstructionName::DIV;

    default:
      break;
    }

  case 2:
    return InstructionName::J;
  case 3:
    return InstructionName::JAL;
  case 4:
    return InstructionName::BEQZ;
  case 8:
    return InstructionName::ADDI;
  case 17:
    return InstructionName::TRAP;
  case 18:
    return InstructionName::JR;
  case 19:
    return InstructionName::JALR;
  case 35:
    return InstructionName::LW;
  case 38:
    return InstructionName::LF;
  case 43:
    return InstructionName::SW;
  case 46:
    return InstructionName::SF;

  default:
    break;
  }

  logger->error(TAG) << "Unknown instruction opcode "
    << static_cast<int>(opcode) << " funcode " << static_cast<int>(funcode);
  return InstructionName::NOP;
}

// probably incomplete, since assignment doesn't cover all opcodes
InstructionType getInstructionType(InstructionName name)
{
  switch (name)
  {
  case InstructionName::ADDI:
  case InstructionName::NOP:
  case InstructionName::ADD:
  case InstructionName::SUB:
  case InstructionName::AND:
  case InstructionName::OR:
  case InstructionName::XOR:
  case InstructionName::MOVF:
  case InstructionName::MOVFP2I:
  case InstructionName::MOVI2FP:
    return InstructionType::Integer;

  case InstructionName::TRAP:
    return InstructionType::Trap;

  case InstructionName::BEQZ:
  case InstructionName::J:
  case InstructionName::JR:
  case InstructionName::JAL:
  case InstructionName::JALR:
    return InstructionType::Branch;

  case InstructionName::LW:
  case InstructionName::LF:
  case InstructionName::SW:
  case InstructionName::SF:
    return InstructionType::Memory;

  case InstructionName::ADDF:
  case InstructionName::SUBF:
  case InstructionName::MULTF:
  case InstructionName::DIVF:
  case InstructionName::MULT:
  case InstructionName::DIV:
  case InstructionName::CVTF2I:
  case InstructionName::CVTI2F:
    return InstructionType::FloatingPoint;

  default:
    break;
  }

  logger->error(TAG, "Unknown instruction name");
  return InstructionType::Integer;
}
