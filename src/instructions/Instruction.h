#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "types.h"
#include "RegisterID.h"
#include "ReservationStationID.h"
#include "instructions/instruction_types.h"
#include "instructions/InstructionData.h"
#include <memory>

struct InstructionArgs
{
  ReservationStationID rsid;
  InstructionData data;
  Data arg1;
  ReservationStationID arg1Source;
  Data arg2;
  ReservationStationID arg2Source;
};

class Instruction
{
private:  
  bool arg1Ready;
  const ReservationStationID arg1Source;  
  bool arg2Ready;
  const ReservationStationID arg2Source;
  std::size_t executeCycles;

protected:  
  Data arg1;
  Data arg2;
  Data result;

public:
  const InstructionData data;
  const ReservationStationID rsid;

  struct Result
  {
    enum Dest
    {
      None,
      CDB,
      PC
    };

    Dest dest;
    ReservationStationID source;
    RegisterID reg;
    Data value;
  };

  Instruction() = delete;
  Instruction(std::size_t executeCycles, const InstructionArgs& args);
  Instruction(const Instruction&) = delete;
  Instruction& operator=(const Instruction&) = delete;

  void notify(const ReservationStationID& id, Data data);
  bool argsReady() const;

  bool execute();
  virtual Result write() = 0;

protected:
  virtual void performExecute() = 0;
};

using StrongInstructionPtr = std::shared_ptr<Instruction>;

#endif
