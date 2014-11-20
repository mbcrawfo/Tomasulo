#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "types.h"
#include "RegisterID.h"
#include "ReservationStationID.h"
#include "instruction_types.h"
#include "utility/IToString.h"
#include <memory>

class Instruction;
using StrongInstructionPtr = std::shared_ptr<Instruction>;

class Instruction
  : public util::IToString
{
private:
  InstructionName name;
  FunctionalUnitType type;
  bool writesToCDB;
  RegisterID rd;
  RegisterID rs1;
  RegisterID rs2;
  UWord immediate;

public:  
   
  virtual std::string toString() const override;

  virtual void execute() = 0;
  virtual void write() = 0;

private:
  friend class Decoder;
  Instruction();
};

#endif
