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

  /**
   * Some branches need to pretend that their destination is a register even 
   * though they don't officially have a destination...
   */
  virtual RegisterID getDest() const override;

  virtual Data execute(Data arg1, Data arg2) const override;
  virtual WriteAction getWriteAction() const override;

  /**
   * Get the address of the PC if the branch was not taken.
   */
  Address getNextInstruction() const;

  /**
   * Get the address of the PC if the branch was taken.
   */
  Address getTarget() const;

private:
  Word signExtend16() const;
  Word signExtend24() const;
};

#endif
