#ifndef __TOMASULO_H__
#define __TOMASULO_H__

#include "types.h"
#include "Memory.h"
#include "RegisterID.h"
#include "ReservationStationID.h"
#include "Decoder.h"
#include "instructions/InstructionData.h"
#include <unordered_map>

class Tomasulo
{
private:
  bool verbose;
  bool halted;
  bool stallIssue;
  std::size_t clockCounter;
  std::size_t instructionsInProgress;
  Address pc;
  Decoder decoder;
  MemoryPtr memory;
  std::unordered_map<RegisterID, Data> archRegFile;
  std::unordered_map<RegisterID, ReservationStationID> renameRegFile;

public:
  explicit Tomasulo(MemoryPtr memory, bool verbose = false);

  bool isHalted() const;
  std::size_t clocks() const;

  void run(Address entryPoint = 0);

private:
  bool issue(const InstructionData& data);
  void execute();
  void write();
};

#endif
