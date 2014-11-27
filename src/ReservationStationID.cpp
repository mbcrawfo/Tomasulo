#include "ReservationStationID.h"

const ReservationStationID ReservationStationID::NONE {
  static_cast<FunctionalUnitType>(-1), 
  static_cast<std::size_t>(-1)
};

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
