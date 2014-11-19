#include "Tomasulo.h"
#include "log.h"
#include "Instruction.h"
#include <cassert>
#include <string>

static const std::string TAG = "Tomasulo";

// machine configuration
static const int GPR_REGISTERS = 32;
static const int FPR_REGISTERS = 32;

Tomasulo::Tomasulo(MemoryPtr memory, bool verbose)
  : verbose(verbose),
    halted(false),
    clockCounter(0),
    memory(memory),
    registers()
{
  assert(memory != nullptr);

  Data data = { 0 };
  for (int i = 0; i < GPR_REGISTERS; ++i)
  {
    registers[{RegisterType::GPR, i}] = data;
  }
  for (int i = 0; i < FPR_REGISTERS; ++i)
  {
    registers[{RegisterType::FPR, i}] = data;
  }
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
    StrongInstructionPtr instruction = Instruction::decode(rawInstruction);
    assert(instruction != nullptr);
    logger->debug(TAG) << "Decoded " << *instruction;

    if (instruction->name == InstructionName::NOP)
    {
      halted = true;
    }

    pc += 4;
  }
}
