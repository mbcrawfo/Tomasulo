#ifndef __TYPES_H__
#define __TYPES_H__

#include <cstdint>
#include <vector>

using Address = uint32_t;
using Byte = uint8_t;
using HalfWord = int16_t;
using UHalfWord = uint16_t;
using Word = int32_t;
using UWord = uint32_t;
using ByteBuffer = std::vector<Byte>;

union Data
{
  Byte b[4];
  HalfWord h[2];
  UHalfWord uh[2];
  Word w;
  UWord uw;
  float f;
};

#endif
