#ifndef __FLOATINGPOINTINSTRUCTION_H__
#define __FLOATINGPOINTINSTRUCTION_H__

#include "instructions/Instruction.h"

class FloatingPointInstruction
  : public Instruction
{
public:
  virtual Data execute(Data arg1, Data arg2) const override;
  virtual WriteAction getWriteAction() const override;
};

#endif
