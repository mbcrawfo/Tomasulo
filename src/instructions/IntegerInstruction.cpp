#include "instructions/IntegerInstruction.h"
#include "log.h"
#include <string>

static const std::string TAG = "IntegerInstruction";

Data IntegerInstruction::execute(Data arg1, Data arg2) const
{
  Data result = { 0 };

  switch (getName())
  {
  case InstructionName::ADDI:
    result.w = arg1.w + static_cast<Word>(static_cast<int16_t>(getImmediate()));
    break;

  case InstructionName::NOP:
    break;

  case InstructionName::ADD:
    result.w = arg1.w + arg2.w;
    break;

  case InstructionName::SUB:
    result.w = arg1.w - arg2.w;
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
    logger->error(TAG) << "Unknown instruction " << getName();
    break;
  }

  return result;
}

WriteAction IntegerInstruction::getWriteAction() const
{
  return getName() == InstructionName::NOP ?
    WriteAction::None : WriteAction::Register;
}