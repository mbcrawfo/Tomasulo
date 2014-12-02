#include "Memory.h"
#include "log.h"
#include "utility/stream_manip.h"
#include <string>
#include <cassert>
#include <algorithm>

// verify some assumptions about type sizes
static_assert(sizeof(Byte) == 1, "Unexpected byte size");
static_assert(sizeof(Word) == sizeof(UWord), "word size mismatch");
static_assert(sizeof(float) == sizeof(UWord), "word size mismatch");

static const std::string TAG = "memory";

Memory::Memory(UWord size)
  : mem(size + (size % sizeof(Word)), 0)
{
  logger->debug(TAG) 
    << "Initialized " << size + (size % sizeof(Word)) << " bytes";
}

std::size_t Memory::size() const
{
  return mem.size();
}

void Memory::clear()
{
  std::fill(mem.begin(), mem.end(), 0);
}

ByteBuffer Memory::read(Address addr, UWord bytes) const
{
  if (addr + bytes >= size())
  {
    throw InvalidAddressException(addr, bytes, size());
  }

  auto start = mem.begin() + addr;
  auto end = start + bytes;
  return ByteBuffer(start, end);
}

Byte Memory::readByte(Address addr) const
{  
  if (addr + sizeof(Byte) >= size())
  {
    throw InvalidAddressException(addr, sizeof(Byte), size());
  }

  return mem[addr];
}

Word Memory::readWord(Address addr) const
{  
  if (addr + sizeof(Word) >= size())
  {
    throw InvalidAddressException(addr, sizeof(Word), size());
  }

  Data t;
  t.uw = readUWord(addr);
  return t.w;
}

UWord Memory::readUWord(Address addr) const
{
  if (addr + sizeof(UWord) >= size())
  {
    throw InvalidAddressException(addr, sizeof(UWord), size());
  }

  Data t;
  auto start = mem.begin() + addr;
  auto end = start + sizeof(UWord);
  std::reverse_copy(start, end, t.b);
  return t.uw;
}

float Memory::readFloat(Address addr) const
{    
  if (addr + sizeof(float) >= size())
  {
    throw InvalidAddressException(addr, sizeof(float), size());
  }

  Data t;
  t.uw = readUWord(addr);
  return t.f;
}

std::string Memory::readString(Address addr) const
{
  auto end = std::find(mem.begin() + addr, mem.end(), '\0');
  if (end == mem.end())
  {
    logger->error(TAG) << "End of string not found for address "
      << util::hex<Address> << addr;
  }

  return std::string(mem.begin() + addr, end);
}

void Memory::write(Address addr, const ByteBuffer& bytes)
{
  if (addr + bytes.size() >= size())
  {
    throw InvalidAddressException(addr, bytes.size(), size());
  }

  std::copy(bytes.begin(), bytes.end(), mem.begin() + addr);
}

void Memory::writeByte(Address addr, Byte b)
{
  if (addr + sizeof(Byte) >= size())
  {
    throw InvalidAddressException(addr, sizeof(Byte), size());
  }

  mem[addr] = b;
}

void Memory::writeWord(Address addr, Word w)
{
  if (addr + sizeof(Word) >= size())
  {
    throw InvalidAddressException(addr, sizeof(Word), size());
  }

  Data t;
  t.w = w;
  writeUWord(addr, t.uw);
}

void Memory::writeUWord(Address addr, UWord uw)
{
  if (addr + sizeof(UWord) >= size())
  {
    throw InvalidAddressException(addr, sizeof(UWord), size());
  }

  Data t;
  t.uw = uw;
  std::reverse_copy(t.b, t.b + sizeof(UWord), mem.begin() + addr);
}

void Memory::writeFloat(Address addr, float f)
{
  if (addr + sizeof(float) >= size())
  {
    throw InvalidAddressException(addr, sizeof(float), size());
  }

  Data t;
  t.f = f;
  writeUWord(addr, t.uw);
}

void Memory::dump(Address addr, std::size_t bytes) const
{
  if (addr + bytes >= size())
  {
    throw InvalidAddressException(addr, bytes, size());
  }

  auto offset = addr % sizeof(UWord);
  if (offset > 0)
  {
    addr -= offset;
    bytes += offset;
  }

  logger->debug(TAG) << "Dump " << bytes << " bytes from "
    << util::hex<Address> << addr;
  for (std::size_t i = 0; i < bytes; i += 4)
  {
    UWord uw = readUWord(addr + i);
    logger->debug(TAG)
      << util::hex<Address> << addr + i << ": "
      << util::hex<UWord> << uw;
  }
  logger->debug(TAG, "End dump");
}
