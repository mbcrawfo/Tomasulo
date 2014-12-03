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
  ReservationStationID source;
  RegisterID dest;
  Data value;  
  RegisterFilePtr registers;
  RenameRegisterFilePtr renameRegisters;
  std::list<ReservationStation*> listeners;

public:
  explicit CommonDataBus(RegisterFilePtr registers,
    RenameRegisterFilePtr renameRegisters);
  CommonDataBus& operator=(CommonDataBus&) = delete;

  bool write(const ReservationStationID& source, const RegisterID& dest,
    Data value);
  void commit();

  void dumpState() const;

  void addListener(ReservationStation* rs);
  void removeListener(ReservationStation* rs);

private:
  void notifyAll();
};

#endif
