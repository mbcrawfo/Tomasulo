#include "Memory.h"
#include "log.h"
#include "utility/stream_manip.h"
#include <string>
#include <cassert>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <cctype>
#include <sstream>

static const std::string TAG = "memory";
static const std::string HEX_DIGIT = "0123456789abcdefABCDEF";

bool Memory::loadFromFile(Memory& mem, const std::string& filename)
{
  std::ifstream file(filename.c_str(), std::ios::in);
  if (!file)
  {
    logger->error(TAG, "Unable to open file " + filename);
    return false;
  }

  std::size_t count = 0;
  logger->debug(TAG, "Loading from file " + filename);
  while (!file.eof())
  {
    std::string line;
    std::getline(file, line);
    //logger->verbose(TAG, "Read line \"" + line + "\"");

    // strip comments
    auto commentIdx = line.find_first_of('#');
    if (commentIdx != std::string::npos)
    {
      line.resize(commentIdx);
    }
    
    // relevant data in the line is
    // [32 bit address]: [bytes...]
    // A line without a colon is skipped
    auto colon = line.find(':');
    if (colon == std::string::npos)
    {
      continue;
    }
        
    std::istringstream is;

    // pull the address
    Address addr;
    is.str(line.substr(0, colon));
    is >> std::hex >> addr;
    //logger->verbose(TAG) << "Address: " << util::hex<Address> << addr;

    // find the hex data string after the colon
    auto start = line.find_first_of(HEX_DIGIT, colon + 1);
    auto end = line.find_last_of(HEX_DIGIT);
    is = std::istringstream(line.substr(start, end - start + 1));
    //logger->verbose(TAG, is.str());

    ByteBuffer buffer;    
    buffer.reserve((end - start) / 2);
    while (is.rdbuf()->in_avail() > 0)
    {
      std::string str;
      std::stringstream temp;
      // must use an integer type because streams will treat *any* char type 
      // as an ascii character rather than an integer
      UWord byte;

      // pull 2 characters from the stream and convert them through a 
      // temporary stream buffer
      // why can't you pull them directly from the stream?  no idea, but if you 
      // try to do that it ignores the width directive
      is.width(2);
      is >> str;
      temp << str;
      temp >> std::hex >> byte;
      buffer.push_back(static_cast<Byte>(byte));
      //logger->verbose(TAG) << "Byte: " << util::hex<Byte> << byte;
    }

    count += buffer.size();
    mem.write(addr, buffer);
    logger->debug(TAG) << "Writing " << buffer.size() << " bytes to "
      << util::hex<Address> << addr;
  }

  logger->debug(TAG) << "Read in " << count << " bytes";
  return true;
}

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

ByteBuffer Memory::read(Address addr, UWord bytes) const
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
  Data t;
  t.uw = readUWord(addr);
  return t.w;
}

UWord Memory::readUWord(Address addr) const
{
  assert(addr + sizeof(UWord) - 1 < size());
  Data t;
  auto start = mem.begin() + addr;
  auto end = start + sizeof(UWord);
  std::reverse_copy(start, end, t.b);
  return t.uw;
}

float Memory::readFloat(Address addr) const
{
  static_assert(sizeof(float) == sizeof(UWord), "word size mismatch");
  assert(addr + sizeof(float) - 1 < size());
  Data t;
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
  Data t;
  t.w = w;
  writeUWord(addr, t.uw);
}

void Memory::writeUWord(Address addr, UWord uw)
{
  assert(addr + sizeof(UWord) - 1 < size());
  Data t;
  t.uw = uw;
  std::reverse_copy(t.b, t.b + sizeof(UWord), mem.begin() + addr);
}

void Memory::writeFloat(Address addr, float f)
{
  static_assert(sizeof(float) == sizeof(UWord), "word size mismatch");
  assert(addr + sizeof(float) - 1 < size());
  Data t;
  t.f = f;
  writeUWord(addr, t.uw);
}

void Memory::dump(Address addr, UWord bytes) const
{
  assert(bytes > 0);
  assert(addr + bytes < size());

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
