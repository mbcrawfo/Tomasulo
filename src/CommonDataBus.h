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

  bool set(const ReservationStationID& source, const RegisterID& dest,
    Data value);
  void writeAndClear();
  void notifyAll();

  void addListener(ReservationStation* rs);
  void removeListener(ReservationStation* rs);
};

#endif
