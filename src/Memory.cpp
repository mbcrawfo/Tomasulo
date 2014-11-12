#include "Memory.h"
#include "log.h"
#include <string>
#include <cassert>
#include <algorithm>

static const std::string TAG = "memory";

Memory::Memory(std::size_t size)
  : mem(size, 0)
{
  logger->stream(LogLevel::Debug, TAG) << "Initialized " << size
    << " bytes";
}

std::size_t Memory::size() const
{
  return mem.size();
}

ByteBuffer Memory::read(Address addr, std::size_t bytes) const
{
  assert(bytes > 0);
  assert(addr + bytes - 1 < size());

  auto start = mem.begin() + addr;
  auto end = start + bytes;
  return ByteBuffer(start, end);
}

Byte Memory::readByte(Address addr) const
{
  assert(addr < size());
  return mem[addr];
}

Word Memory::readWord(Address addr) const
{
  static_assert(sizeof(Word) == sizeof(UWord), "word size mismatch");
  assert(addr + sizeof(Word) - 1 < size());
  DataType t;
  t.uw = readUWord(addr);
  return t.w;
}

UWord Memory::readUWord(Address addr) const
{
  assert(addr + sizeof(UWord) - 1 < size());
  DataType t;
  auto start = mem.begin() + addr;
  auto end = start + sizeof(UWord);
  std::reverse_copy(start, end, t.b);
  return t.uw;
}

float Memory::readFloat(Address addr) const
{
  static_assert(sizeof(float) == sizeof(UWord), "word size mismatch");
  assert(addr + sizeof(float) - 1 < size());
  DataType t;
  t.uw = readUWord(addr);
  return t.f;
}

void Memory::write(Address addr, const ByteBuffer& bytes)
{
  assert(bytes.size() > 0);
  assert(addr + bytes.size() - 1 < size());
  std::copy(bytes.begin(), bytes.end(), mem.begin() + addr);
}

void Memory::writeByte(Address addr, Byte b)
{
  assert(addr < size());
  mem[addr] = b;
}

void Memory::writeWord(Address addr, Word w)
{
  static_assert(sizeof(Word) == sizeof(UWord), "word size mismatch");
  assert(addr + sizeof(Word) - 1 < size());
  DataType t;
  t.w = w;
  writeUWord(addr, t.uw);
}

void Memory::writeUWord(Address addr, UWord uw)
{
  assert(addr + sizeof(UWord) - 1 < size());
  DataType t;
  t.uw = uw;
  std::reverse_copy(t.b, t.b + sizeof(UWord), mem.begin() + addr);
}

void Memory::writeFloat(Address addr, float f)
{
  static_assert(sizeof(float) == sizeof(UWord), "word size mismatch");
  assert(addr + sizeof(float) - 1 < size());
  DataType t;
  t.f = f;
  writeUWord(addr, t.uw);
}
