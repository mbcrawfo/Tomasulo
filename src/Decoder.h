#ifndef __DECODE_H__
#define __DECODE_H__

#include "instructions/Instruction.h"

class Decoder
{
public:
  static StrongInstructionPtr decode(UWord instruction);

private:
  static void decodeItype(UWord instruction, Instruction& out);
  static void decodeRtype(UWord instruction, Instruction& out);
  static void decodeJtype(UWord instruction, Instruction& out);
  static void setRegisterTypes(Instruction& out);
};

#endif
