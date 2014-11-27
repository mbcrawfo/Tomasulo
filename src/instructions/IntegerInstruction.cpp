#include "instructions/IntegerInstruction.h"

IntegerInstruction::IntegerInstruction(const InstructionArgs& args)
  : Instruction(args)
{}

Instruction::Result IntegerInstruction::write()
{
  Result temp;
  temp.dest = data.name == InstructionName::NOP ?
    Result::Dest::None : Result::Dest::CDB;
  temp.source = rsid;
  temp.reg = data.rd;
  temp.value = result;
  return temp;
}

void IntegerInstruction::performExecute()
{
  switch (data.name)
  {
  case InstructionName::ADDI:
    result.w = arg1.w + static_cast<Word>(static_cast<int16_t>(data.immediate));
    break;

  case InstructionName::NOP:
    break;

  case InstructionName::ADD:
    result.w = arg1.w + arg2.w;
    break;

  case InstructionName::SUB:
    result.w = arg1.w * arg2.w;
    break;

  case InstructionName::AND:
    result.w = arg1.w & arg2.w;
    break;

  case InstructionName::OR:
    result.w = arg1.w | arg2.w;
    break;

  case InstructionName::XOR:
    result.w = arg1.w ^ arg2.w;
    break;

  case InstructionName::MOVF:
  case InstructionName::MOVFP2I:
  case InstructionName::MOVI2FP:
    result = arg1;
    break;

  default:
    logger->error("IntegerInstruction") << "Unknown instruction " << data.name;
    break;
  }
}