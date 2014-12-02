#include "Tomasulo.h"
#include "log.h"
#include "instructions/Instruction.h"
#include <cassert>
#include <string>

static const std::string TAG = "Tomasulo";

// machine configuration
static const int GPR_REGISTERS = 32;
static const int FPR_REGISTERS = 32;

Tomasulo::Tomasulo(MemoryPtr memory, bool verbose)
  : verbose(verbose),
    iFactory(),
    halted(false),
    stallIssue(false),
    clockCounter(0),
    instructionsInProgress(0),
    pc(0),
    memory(memory),
    registerFile(),
    renameRegFile()
{
  assert(memory != nullptr);
  renameRegFile[RegisterID::NONE] = ReservationStationID::NONE;  
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

  while (!halted || instructionsInProgress > 0)
  {
    ++clockCounter;

    write();
    execute();
    if (!halted)
    {
      UWord rawInstruction = memory->readUWord(pc);
      InstructionPtr instr = iFactory.decode(rawInstruction);
      assert(instr);

      if (issue(instr) && !stallIssue)
      {
        pc += 4;
      }
    }
  }
}

bool Tomasulo::issue(InstructionPtr instr)
{
  // check for a halt
  if (instr->getName() == InstructionName::TRAP && instr->getImmediate() == 0)
  {
    halted = true;
    return true;
  }

  return true;
}

void Tomasulo::execute()
{
  
}

void Tomasulo::write()
{

}
