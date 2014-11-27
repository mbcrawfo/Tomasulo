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
    halted(false),
    stallIssue(false),
    clockCounter(0),
    instructionsInProgress(0),
    pc(0),
    decoder(),
    memory(memory),
    archRegFile(),
    renameRegFile()
{
  assert(memory != nullptr);

  Data data = { 0 };
  archRegFile[RegisterID::NONE] = data;
  renameRegFile[RegisterID::NONE] = ReservationStationID::NONE;
  for (int i = 0; i < GPR_REGISTERS; ++i)
  {
    RegisterID gpr = { RegisterType::GPR, i };
    archRegFile[gpr] = data;
    renameRegFile[gpr] = ReservationStationID::NONE;
  }
  for (int i = 0; i < FPR_REGISTERS; ++i)
  {
    RegisterID fpr = { RegisterType::FPR, i };
    archRegFile[fpr] = data;
    renameRegFile[fpr] = ReservationStationID::NONE;
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

  while (!halted || instructionsInProgress > 0)
  {
    ++clockCounter;

    write();
    execute();
    if (!halted)
    {
      UWord rawInstruction = memory->readUWord(pc);
      InstructionData data = decoder.decode(rawInstruction);
      if (issue(data) && !stallIssue)
      {
        pc += 4;
      }
    }
  }
}

bool Tomasulo::issue(const InstructionData& data)
{
  // check for a halt
  if (data.name == InstructionName::TRAP && data.immediate == 0)
  {
    halted = true;
    return true;
  }

  Data arg1{{ 0 }};
  ReservationStationID arg1Source = renameRegFile[data.rs1];
  Data arg2{{ 0 }};
  ReservationStationID arg2Source = renameRegFile[data.rs2];
}

void Tomasulo::execute()
{
  
}

void Tomasulo::write()
{

}
