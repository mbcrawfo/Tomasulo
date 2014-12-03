#include "RenameRegisterFile.h"
#include "log.h"
#include <string>
#include <iostream>

static const std::string TAG = "RenameRegisterFile";

RenameRegisterFile::RenameRegisterFile()
  : renameRegisters()
{
}

void RenameRegisterFile::rename(const RegisterID& reg, 
  const ReservationStationID& rsid)
{
  auto itr = renameRegisters.find(reg);
  if (itr != renameRegisters.end())
  {
    logger->warning(TAG) << "Overwriting rename of "
      << reg << " -> " << itr->second << " to "
      << reg << " -> " << rsid;
  }
  else
  {
    logger->debug(TAG) << "Renaming " << reg << " -> " << rsid;
  }

  renameRegisters[reg] = rsid;  
}

void RenameRegisterFile::clearRename(const RegisterID& reg)
{
  auto itr = renameRegisters.find(reg);
  if (itr != renameRegisters.end())
  {
    logger->debug(TAG) << "Clearing renaming of " << reg 
      << " (was " << itr->second << ")";
    renameRegisters.erase(itr);
  }
}

void RenameRegisterFile::clearRename(const ReservationStationID& rsid)
{
  clearRename(getReverseRename(rsid));
}

ReservationStationID RenameRegisterFile::getRenaming(
  const RegisterID& reg) const
{
  auto itr = renameRegisters.find(reg);
  return itr == renameRegisters.end() ?
    ReservationStationID::NONE : itr->second;
}

RegisterID RenameRegisterFile::getReverseRename(
  const ReservationStationID& rsid) const
{
  for (auto entry : renameRegisters)
  {
    if (entry.second == rsid)
    {
      return entry.first;
    }
  }

  return RegisterID::NONE;
}
