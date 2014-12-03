#ifndef __MEMORYINSTRUCTION_H__
#define __MEMORYINSTRUCTION_H__

#include "instructions/Instruction.h"
#include "Memory.h"

class MemoryInstruction
  : public Instruction
{
private:
  MemoryPtr memory;

public:
  explicit MemoryInstruction(MemoryPtr memory);

  // This is derpy, but stores have no "destination" and the value to be stored 
  // (rd) is considered arg2.  Avoids having to rewrite the whole reservation 
  // station register renaming system.
  virtual RegisterID getDest() const override;
  virtual RegisterID getArg2() const override;

  // Also derpy, but stores return the destination address as the execute 
  // result, while loads return the loaded value.
  virtual Data execute(Data arg1, Data arg2) const override;
    
  virtual WriteAction getWriteAction() const override;

private:
  Word signExtendImmediate() const;
};

#endif
