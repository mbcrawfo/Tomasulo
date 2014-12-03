#include "FloatingPointInstruction.h"
#include "log.h"
#include <string>

static const std::string TAG = "FloatingPointInstruction";

Data FloatingPointInstruction::execute(Data arg1, Data arg2) const
{
  Data result = { 0 };

  switch (getName())
  {
  case InstructionName::ADDF:
    result.f = arg1.f + arg2.f;
    break;

  case InstructionName::SUBF:
    result.f = arg1.f - arg2.f;
    break;

  case InstructionName::MULTF:
    result.f = arg1.f * arg2.f;
    break;

  case InstructionName::DIVF:
    result.f = arg1.f / arg2.f;
    break;

  case InstructionName::MULT:
    result.w = arg1.w * arg2.w;
    break;

  case InstructionName::DIV:
    result.w = arg1.w / arg2.w;
    break;

  case InstructionName::CVTF2I:
    result.w = static_cast<Word>(arg1.f);
    break;

  case InstructionName::CVTI2F:
    result.f = static_cast<float>(arg1.w);
    break;

  default:
    logger->error(TAG) << "Unknown instruction " << getName();
    break;
  }

  return result;
}

WriteAction FloatingPointInstruction::getWriteAction() const
{
  return WriteAction::Register;
}
