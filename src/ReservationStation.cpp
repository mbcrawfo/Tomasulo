#include "ReservationStation.h"

ReservationStationArg::ReservationStationArg(Data data)
  : read(true), data(data), source()
{
}

ReservationStationArg::ReservationStationArg(const ReservationStationID& source)
  : read(false), data(), source(source)
{
}

bool ReservationStationArg::ready() const
{
  return read;
}

Data ReservationStationArg::getData() const
{
  return data;
}

void ReservationStationArg::notify(const ReservationStationID& rsid, Data d)
{
  if (rsid == source)
  {
    data = d;
    read = true;
  }
}

bool ReservationStation::ready() const
{
  return arg1.ready() && arg2.ready();
}
