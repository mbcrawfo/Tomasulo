#ifndef __INTEGERINSTRUCTION_H__
#define __INTEGERINSTRUCTION_H__

#include "instructions/Instruction.h"
#include "log.h"

class IntegerInstruction
  : public Instruction
{
public:
  IntegerInstruction(const InstructionArgs& args);

  virtual Result write() override;

protected:
  virtual void performExecute() override;
};

#endif
