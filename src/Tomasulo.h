#ifndef __TOMASULO_H__
#define __TOMASULO_H__

#include "types.h"
#include "Memory.h"

class Tomasulo
{
private:
  bool verbose;
  bool halted;
  std::size_t clockCounter;
  Address pc;
  MemoryPtr memory;

public:
  explicit Tomasulo(MemoryPtr memory, bool verbose = false);

  bool isHalted() const;
  std::size_t clocks() const;

  void run(Address entryPoint = 0);
};

#endif
