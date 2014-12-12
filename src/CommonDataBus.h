#ifndef __COMMONDATABUS_H__
#define __COMMONDATABUS_H__

#include "types.h"
#include "RegisterID.h"
#include "ReservationStationID.h"
#include "RegisterFile.h"
#include "RenameRegisterFile.h"
#include <list>

class CommonDataBus;
using CommonDataBusPtr = Pointer<CommonDataBus>;

class CommonDataBus
{
private:
  bool used;
  bool idleThisCycle;
  ReservationStation* source;
  ReservationStationID sourceID;
  RegisterID destID;
  Data value;
  RegisterFilePtr registers;
  RenameRegisterFilePtr renameRegisters;
  std::list<ReservationStation*> listeners;
  std::list<ReservationStation*> rejected;

public:
  explicit CommonDataBus(RegisterFilePtr registers,
    RenameRegisterFilePtr renameRegisters);
  CommonDataBus& operator=(CommonDataBus&) = delete;

  /**
   * Attempts to write the result value of src to the data bus.  If multiple 
   * writes occur in the same cycle the oldest write (based on the cycle when 
   * the instruction began executing) will be written.  When the CDB value is 
   * committed the reservation station whose value was written will be notified.
   */
  void write(ReservationStation* src);

  /**
   * If a value was written to the CDB, notifies the source that its write was 
   * accepted, notifies all listeners of the written value and its source, then 
   * commits the value to the register file.
   */
  void commit();

  void dumpState() const;

  void addListener(ReservationStation* rs);

private:
  void notifyListeners();
};

#endif
