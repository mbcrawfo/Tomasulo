#include "BranchInstruction.h"
#include "log.h"
#include <string>

static const std::string TAG = "BranchInstruction";

BranchInstruction::BranchInstruction(Address nextInstruction)
  : Instruction(),
    nextInstruction(nextInstruction),
    target(0)
{
}

RegisterID BranchInstruction::getDest() const
{
  switch (getName())
  {
  case InstructionName::JAL:
  case InstructionName::JALR:
    return { RegisterType::GPR, 31 };

  default:
    return RegisterID::NONE;
  }
}

Data BranchInstruction::execute(Data arg1, Data arg2) const
{
  (void) arg2;

  Data result = { 0 };

  switch (getName())
  {
  case InstructionName::BEQZ:
    target = nextInstruction + signExtend16();
    result.uw = arg1.w == 0 ? target : nextInstruction;
    break;

  case InstructionName::J:
    target = nextInstruction + signExtend24();
    result.uw = target;
    break;

  case InstructionName::JR:
    target = arg1.uw;
    result.uw = target;
    break;

  case InstructionName::JAL:
    target = nextInstruction + signExtend24();
    result.uw = target;
    break;

  case InstructionName::JALR:
    target = arg1.uw;
    result.uw = target;
    break;

  default:
    logger->error(TAG) << "Unknown instruction " << getName();
    break;
  }

  return result;
}

WriteAction BranchInstruction::getWriteAction() const
{
  switch (getName())
  {
  case InstructionName::JAL:
  case InstructionName::JALR:
    return WriteAction::PC_R31;

  default:
    return WriteAction::PC;
  }
}

Address BranchInstruction::getNextInstruction() const
{
  return nextInstruction;
}

Address BranchInstruction::getTarget() const
{
  return target;
}

Word BranchInstruction::signExtend16() const
{
  return static_cast<Word>(static_cast<HalfWord>(getImmediate()));
}

Word BranchInstruction::signExtend24() const
{
  UWord immediate = getImmediate() << 6;    
  return static_cast<Word>(immediate) >> 6;
}

