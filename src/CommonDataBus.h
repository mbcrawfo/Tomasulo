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

  void write(ReservationStation* src);
  void commit();

  void dumpState() const;

  void addListener(ReservationStation* rs);

private:
  void notifyListeners();
};

#endif
