#include "InstructionFactory.h"
#include "log.h"
#include "instructions/IntegerInstruction.h"
#include "instructions/TrapInstruction.h"
#include "utility/stream_manip.h"
#include <string>
#include <cassert>

static const std::string TAG = "InstructionFactory";

InstructionFactory::InstructionFactory(MemoryPtr memory,
  RegisterFilePtr registers)
  : memory(memory),
    registers(registers),
    instruction(),
    name(),
    encodingType(),
    fuType(),
    result()
{
  assert(memory != nullptr);
  assert(registers != nullptr);
}

InstructionPtr InstructionFactory::decode(UWord rawInstruction)
{
  instruction = rawInstruction;
  logger->verbose(TAG) << "Decoding " << util::hex<UWord> << instruction;

  Byte opcode = instruction >> (31 - 5);
  Byte funcode = instruction & 0x3f;
  encodingType = getEncodingType(opcode);  
  
  switch (encodingType)
  {
  case InstructionEncodingType::Itype:
    name = getName(opcode);
    fuType = getInstructionType(name);
    createInstruction();
    decodeItype();
    break;

  case InstructionEncodingType::Rtype:
    name = getName(opcode, funcode);
    fuType = getInstructionType(name);
    createInstruction();
    decodeRtype();
    break;

  case InstructionEncodingType::Jtype:
    name = getName(opcode);
    fuType = getInstructionType(name);
    createInstruction();
    decodeJtype();
    break;
  }

  setRegisterTypes();
  return result;
}

void InstructionFactory::createInstruction()
{
  switch (fuType)
  {
  case FunctionalUnitType::Integer:
    result = InstructionPtr(new IntegerInstruction);
    break;

  case FunctionalUnitType::Trap:
    result = InstructionPtr(new TrapInstruction(memory, registers));
    break;

  default:
    logger->error(TAG) << fuType << " not implemented";
    return;
  }

  result->name = name;
  result->type = fuType;
}

void InstructionFactory::decodeItype()
{
  logger->verbose(TAG, "Decoding as Itype");
  result->rd.index = (instruction >> (31 - 15)) & 0x1f;
  result->rs1.index = (instruction >> (31 - 10)) & 0x1f;
  result->rs2 = RegisterID::NONE;
  result->immediate = instruction & 0xffff;
}

void InstructionFactory::decodeRtype()
{
  logger->verbose(TAG, "Decoding as Rtype");
  result->rd.index = (instruction >> (31 - 20)) & 0x1f;
  result->rs1.index = (instruction >> (31 - 10)) & 0x1f;
  result->rs2.index = (instruction >> (31 - 15)) & 0x1f;
  result->immediate = 0xffffffff;
}

void InstructionFactory::decodeJtype()
{
  logger->verbose(TAG, "Decoding as Jtype");
  result->rd = RegisterID::NONE;
  result->rs1 = RegisterID::NONE;
  result->rs2 = RegisterID::NONE;
  result->immediate = instruction & 0x07ffffff;
}

void InstructionFactory::setRegisterTypes()
{
  switch (result->name)
  {
    // trap is a special case
  case InstructionName::TRAP:
    result->rd = RegisterID::NONE;
    if (result->immediate == 2)
    {
      result->rs1.type = RegisterType::FPR;
    }
    else
    {
      result->rs1.type = RegisterType::GPR;
    }
    break;

    // GPR rs1 only
  case InstructionName::BEQZ:
  case InstructionName::JR:
  case InstructionName::JALR:
    result->rs1.type = RegisterType::GPR;
    break;

    // GPR rd and rs1
  case InstructionName::ADDI:
  case InstructionName::LW:
  case InstructionName::SW:
    result->rd.type = RegisterType::GPR;
    result->rs1.type = RegisterType::GPR;
    break;

    // FPR rd, GPR rs1
  case InstructionName::LF:
  case InstructionName::SF:
  case InstructionName::MOVI2FP:
    result->rd.type = RegisterType::FPR;
    result->rs1.type = RegisterType::GPR;
    break;

    // GPR rd, FPR rs1
  case InstructionName::MOVFP2I:
    result->rd.type = RegisterType::GPR;
    result->rs1.type = RegisterType::FPR;
    break;

    // FPR rd and rs1
  case InstructionName::MOVF:
  case InstructionName::CVTF2I:
  case InstructionName::CVTI2F:
    result->rd.type = RegisterType::FPR;
    result->rs1.type = RegisterType::FPR;
    break;

    // 3x GPR
  case InstructionName::ADD:
  case InstructionName::SUB:
  case InstructionName::AND:
  case InstructionName::OR:
  case InstructionName::XOR:
    result->rd.type = RegisterType::GPR;
    result->rs1.type = RegisterType::GPR;
    result->rs2.type = RegisterType::GPR;
    break;

    // 3x FPR
  case InstructionName::ADDF:
  case InstructionName::SUBF:
  case InstructionName::MULTF:
  case InstructionName::DIVF:
  case InstructionName::MULT:
  case InstructionName::DIV:
    result->rd.type = RegisterType::FPR;
    result->rs1.type = RegisterType::FPR;
    result->rs2.type = RegisterType::FPR;
    break;
  }
}