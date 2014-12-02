#ifndef __RESERVATIONSTATION_H__
#define __RESERVATIONSTATION_H__

#include "ReservationStationID.h"
#include "RegisterFile.h"

class ReservationStation
{
private:
  ReservationStationID id;
  RegisterFile& registers;

public:
  ReservationStation() = delete;
  ReservationStation(const ReservationStationID& id, 
    RegisterFile& registers);

  ReservationStationID getID() const;


};

#endif
