#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "types.h"
#include "RegisterID.h"
#include "instructions/instruction_types.h"
#include <ostream>

class Instruction;
using InstructionPtr = Ptr<Instruction>;

enum class WriteAction
{
  None,
  Register,
  PC,
  Memory
};

class Instruction
{
private:
  InstructionName name;
  FunctionalUnitType type;
  RegisterID rd;
  RegisterID rs1;
  RegisterID rs2;
  UWord immediate;

public:
  Instruction() = default;

  InstructionName getName() const;
  FunctionalUnitType getType() const;
  RegisterID getRd() const;
  RegisterID getRs1() const;
  RegisterID getRs2() const;
  UWord getImmediate() const;

  virtual Data execute(Data arg1, Data arg2) const = 0;
  virtual WriteAction getWriteAction() const;

  friend class InstructionFactory;
  friend std::ostream& operator<<(std::ostream& os, const Instruction& data);
};

#endif
