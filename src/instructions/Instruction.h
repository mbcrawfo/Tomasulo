#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "types.h"
#include "RegisterID.h"
#include "instructions/instruction_types.h"
#include <ostream>

class Instruction;
using InstructionPtr = Pointer<Instruction>;

enum class WriteAction
{
  None,
  Register,
  PC,
  PC_R31,
  Memory
};

/**
 * The base class for all instructions in tomasulo.  Is extended for each 
 * functional unit type.
 */
class Instruction
{
private:
  InstructionName name;
  FunctionalUnitType type;
  UWord immediate;

protected:
  RegisterID rd;
  RegisterID rs1;
  RegisterID rs2;  

public:
  Instruction() = default;

  InstructionName getName() const;
  FunctionalUnitType getType() const;
  virtual RegisterID getDest() const;
  virtual RegisterID getArg1() const;
  virtual RegisterID getArg2() const;
  UWord getImmediate() const;

  /**
   * Perform the execute action for this instruction and return its result.
   */
  virtual Data execute(Data arg1, Data arg2) const = 0;

  /**
   * Get the write stage action for this instruction (default: none).
   */
  virtual WriteAction getWriteAction() const;

  friend class InstructionFactory;
  friend std::ostream& operator<<(std::ostream& os, const Instruction& data);
};

#endif
