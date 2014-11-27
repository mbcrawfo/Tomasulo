#ifndef __DECODE_H__
#define __DECODE_H__

#include "instructions/InstructionData.h"
#include "types.h"

class Decoder
{
  UWord instruction;
  InstructionData out;

public:
  InstructionData decode(UWord rawInstruction);

private:
  void decodeItype();
  void decodeRtype();
  void decodeJtype();
  void setRegisterTypes();
};

#endif
