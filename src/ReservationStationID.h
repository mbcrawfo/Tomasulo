#ifndef __RESERVATIONSTATIONID_H__
#define __RESERVATIONSTATIONID_H__

#include "instruction_types.h"
#include <ostream>

/**
 * Identify a reservation station by type and index.
 */
struct ReservationStationID
{
  FunctionalUnitType type;
  std::size_t index;
};

std::ostream& operator<<(std::ostream& os, const ReservationStationID& rsid);
bool operator==(const ReservationStationID& lhs, 
  const ReservationStationID& rhs);
bool operator!=(const ReservationStationID& lhs,
  const ReservationStationID& rhs);

namespace std
{
template<>
struct hash<ReservationStationID>
{
  std::size_t operator()(const ReservationStationID& rsid)
  {
    auto typeHash = std::hash<FunctionalUnitType>()(rsid.type);
    auto indexHash = std::hash<std::size_t>()(rsid.index);
    return typeHash ^ indexHash;
  }
};
}

#endif
