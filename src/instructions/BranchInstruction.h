#ifndef __BRANCHINSTRUCTION_H__
#define __BRANCHINSTRUCTION_H__

#include "instructions/Instruction.h"

class BranchInstruction
  : public Instruction
{
private:
  Address nextInstruction;
  mutable Address target;

public:
  explicit BranchInstruction(Address nextInstruction);

  virtual RegisterID getDest() const override;
  virtual Data execute(Data arg1, Data arg2) const override;
  virtual WriteAction getWriteAction() const override;

  Address getNextInstruction() const;
  Address getTarget() const;

private:
  Word signExtend16() const;
  Word signExtend24() const;
};

#endif
