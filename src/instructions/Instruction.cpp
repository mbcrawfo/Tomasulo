#include "Instruction.h"
#include "log.h"

#include <string>

static const std::string TAG = "Instruction";

Instruction::Instruction(std::size_t executeCycles, const InstructionArgs& args)
  : arg1Ready(false),
    arg1Source(args.arg1Source),    
    arg2Ready(false),
    arg2Source(args.arg2Source),
    executeCycles(executeCycles),
    arg1(args.arg1),
    arg2(args.arg2),
    result(),
    data(data),
    rsid(args.rsid)
{
  if (arg1Source == ReservationStationID::NONE)
  {
    arg1Ready = true;
  }
  if (arg2Source == ReservationStationID::NONE)
  {
    arg2Ready = true;
  }
}

void Instruction::notify(const ReservationStationID& id, Data data)
{
  if (arg1Source == id)
  {
    arg1 = data;
    arg1Ready = true;
  }
  if (arg2Source == id)
  {
    arg2 = data;
    arg2Ready = true;
  }
}

bool Instruction::argsReady() const
{
  return arg1Ready && arg2Ready;
}

bool Instruction::execute()
{
  executeCycles--;
  if (executeCycles > 0)
  {
    return false;
  }
  else
  {
    performExecute();
    return true;
  }
}
