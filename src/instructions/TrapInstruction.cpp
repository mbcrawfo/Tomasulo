#include "TrapInstruction.h"
#include "log.h"
#include <string>
#include <iostream>
#include <cassert>

static const std::string TAG = "TrapInstruction";

TrapInstruction::TrapInstruction(MemoryPtr memory, RegisterFilePtr registers)
  : Instruction(),
   memory(memory),
   registers(registers)
{
  assert(memory != nullptr);
  assert(registers != nullptr);
}

Data TrapInstruction::execute(Data arg1, Data arg2) const
{
  (void) arg2;

  switch (getImmediate())
  {
  case 1:
    std::cout << arg1.w << std::flush;
    break;

  case 2:
    std::cout << arg1.f << std::flush;
    break;

  case 3:
    std::cout << memory->readString(arg1.uw) << std::flush;
    break;

  default:
    logger->error(TAG) << "Unknown immediate action " << getImmediate();
  }

  return Data();
}

WriteAction TrapInstruction::getWriteAction() const
{
  return WriteAction::None;
}
