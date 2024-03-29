#ifndef __RESERVATIONSTATIONID_H__
#define __RESERVATIONSTATIONID_H__

#include "instructions/instruction_types.h"
#include <ostream>
#include <functional>

/**
 * Identifies a reservation station by type and index.
 */
struct ReservationStationID
{
  static const ReservationStationID NONE;

  FunctionalUnitType type;
  std::size_t index;
};

std::ostream& operator<<(std::ostream& os, const ReservationStationID& rsid);
bool operator==(const ReservationStationID& lhs, 
  const ReservationStationID& rhs);
bool operator!=(const ReservationStationID& lhs,
  const ReservationStationID& rhs);

struct ReservationStationIDHash
{
  std::size_t operator()(const ReservationStationID& rsid) const
  {
    auto typeHash = std::hash<std::size_t>()(static_cast<std::size_t>(rsid.type));
    auto indexHash = std::hash<std::size_t>()(rsid.index);
    return typeHash ^ indexHash;
  }
};

#endif
