#include "instructions/MemoryInstruction.h"
#include "log.h"
#include <string>
#include <cassert>

static const std::string TAG = "MemoryInstruction";

MemoryInstruction::MemoryInstruction(MemoryPtr memory)
  : memory(memory)
{
  assert(memory != nullptr);
}

RegisterID MemoryInstruction::getDest() const
{
  switch (getName())
  {
  case InstructionName::SF:
  case InstructionName::SW:
    return RegisterID::NONE;

  default:
    return rd;
  }
}

RegisterID MemoryInstruction::getArg2() const
{
  switch (getName())
  {
  case InstructionName::SF:
  case InstructionName::SW:
    return rd;

  default:
    return rs2;
  }
}

Data MemoryInstruction::execute(Data arg1, Data arg2) const
{
  (void) arg2;
  Data result;
  // calculate the effective address
  result.uw = arg1.uw + signExtendImmediate();

  switch (getName())
  {
  case InstructionName::LF:
  case InstructionName::LW:
    result.uw = memory->readUWord(result.uw);
    break;

  case InstructionName::SF:
  case InstructionName::SW:
    break;

  default:
    logger->error(TAG) << "Unknown instruction " << getName();
    break;
  }

  return result;
}

WriteAction MemoryInstruction::getWriteAction() const
{
  switch (getName())
  {
  case InstructionName::LF:
  case InstructionName::LW:
    return WriteAction::Register;

  case InstructionName::SF:
  case InstructionName::SW:
    return WriteAction::Memory;

  default:
    logger->error(TAG) << "Unknown instruction " << getName();
    return WriteAction::None;
  }
}

Word MemoryInstruction::signExtendImmediate() const 
{
  return static_cast<Word>(static_cast<HalfWord>(getImmediate()));
}
