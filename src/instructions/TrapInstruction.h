#ifndef __TRAPINSTRUCTION_H__
#define __TRAPINSTRUCTION_H__

#include "types.h"
#include "instructions/Instruction.h"
#include "Memory.h"
#include "RegisterFile.h"

class TrapInstruction
  : public Instruction
{
private:
  MemoryPtr memory;
  RegisterFilePtr registers;

public:
  TrapInstruction(MemoryPtr memory, RegisterFilePtr registers);

  virtual Data execute(Data arg1, Data arg2) const override;
  virtual WriteAction getWriteAction() const override;
};

#endif
