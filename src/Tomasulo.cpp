#include "Tomasulo.h"
#include "Instruction.h"
#include <cassert>

Tomasulo::Tomasulo(MemoryPtr memory, bool verbose)
  : verbose(verbose), halted(false), clockCounter(0), memory(memory)
{
  assert(memory != nullptr);
}

bool Tomasulo::isHalted() const
{
  return halted;
}

std::size_t Tomasulo::clocks() const
{
  return clockCounter;
}

void Tomasulo::run(Address entryPoint)
{
  pc = entryPoint;

  while (!halted)
  {
    clockCounter++;

    UWord rawInstruction = memory->readUWord(pc);
    Instruction instruction = Instruction::decode(rawInstruction);
    if (instruction.name == InstructionName::NOP)
    {
      halted = true;
    }

    pc += 4;
  }
}
