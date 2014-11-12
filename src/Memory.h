#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "types.h"

class Memory
{
private:
  ByteBuffer mem;

public:
  Memory(std::size_t size);
  Memory(const Memory&) = delete;
  Memory& operator=(const Memory&) = delete;

  std::size_t size() const;

  ByteBuffer read(Address addr, std::size_t bytes) const;
  Byte readByte(Address addr) const;
  Word readWord(Address addr) const;
  UWord readUWord(Address addr) const;
  float readFloat(Address addr) const;

  void write(Address addr, const ByteBuffer& bytes);
  void writeByte(Address addr, Byte b);
  void writeWord(Address addr, Word w);
  void writeUWord(Address addr, UWord uw);
  void writeFloat(Address addr, float f);
};

#endif
