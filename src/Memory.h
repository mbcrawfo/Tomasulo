#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "types.h"
#include "Exceptions.h"
#include <string>
#include <memory>

/**
 * A byte accessible block of memory.
 */
class Memory
{
private:
  ByteBuffer mem;

public:
  /**
   * Create a memory block that holds size bytes.  Size is rounded up to be a 
   * multiple of the word size.
   */
  Memory(UWord size);
  Memory(const Memory&) = delete;
  Memory& operator=(const Memory&) = delete;

  /**
   * Return the number of bytes in the memory.
   */
  std::size_t size() const;

  /**
   * Clears the full memory to zero.
   */
  void clear();

  /**
   * Reads a chunk of memory, either as a raw buffer or a specific type.
   */
  ByteBuffer read(Address addr, UWord bytes) const;
  Byte readByte(Address addr) const;
  Word readWord(Address addr) const;
  UWord readUWord(Address addr) const;
  float readFloat(Address addr) const;
  std::string readString(Address addr) const;

  /**
   * Writes a buffer or type to memory.
   */
  void write(Address addr, const ByteBuffer& bytes);
  void writeByte(Address addr, Byte b);
  void writeWord(Address addr, Word w);
  void writeUWord(Address addr, UWord uw);
  void writeFloat(Address addr, float f);

  /**
   * Dumps memory values to the logging system.  Address will be rounded down 
   * and bytes rounded up so that the output is always done in multiples of 
   * full words.
   */
  void dump(Address addr, std::size_t bytes) const;
};

using MemoryPtr = std::shared_ptr<Memory>;

#endif
