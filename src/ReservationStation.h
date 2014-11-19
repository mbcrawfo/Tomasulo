#ifndef __RESERVATIONSTATION_H__
#define __RESERVATIONSTATION_H__

#include "ReservationStationID.h"
#include "Instruction.h"
#include "instruction_types.h"
#include <memory>

class ReservationStationArg
{ 
private:
  bool read;
  Data data;
  ReservationStationID source;

public:
  ReservationStationArg() = delete;
  explicit ReservationStationArg(Data data);
  explicit ReservationStationArg(const ReservationStationID& source);

  bool ready() const;
  Data getData() const;
  void notify(const ReservationStationID& rsid, Data d);
};

struct ReservationStation
{
  StrongInstructionPtr instruction;
  Data result;
  ReservationStationArg arg1;
  ReservationStationArg arg2;

  bool ready() const;
};

using StrongReservationStationPtr = std::shared_ptr<ReservationStation>;

#endif
