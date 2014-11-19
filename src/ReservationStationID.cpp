#include "ReservationStationID.h"

std::ostream& operator<<(std::ostream& os, const ReservationStationID& rsid)
{
  os << rsid.type << rsid.index;
  return os;
}

bool operator==(const ReservationStationID& lhs,
  const ReservationStationID& rhs)
{
  return lhs.type == rhs.type && lhs.index == rhs.index;
}

bool operator!=(const ReservationStationID& lhs,
  const ReservationStationID& rhs)
{
  return !(lhs == rhs);
}
