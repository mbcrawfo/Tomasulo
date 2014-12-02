#ifndef __INTEGERINSTRUCTION_H__
#define __INTEGERINSTRUCTION_H__

#include "instructions/Instruction.h"

class IntegerInstruction
  : public Instruction
{
public:
  virtual Data execute(Data arg1, Data arg2) const override;
  virtual WriteAction getWriteAction() const override;
};

#endif
