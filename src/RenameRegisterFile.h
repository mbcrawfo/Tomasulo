#ifndef __RENAMEREGISTERFILE_H__
#define __RENAMEREGISTERFILE_H__

#include "types.h"
#include "RegisterID.h"
#include "ReservationStationID.h"
#include <unordered_map>

class RenameRegisterFile;
using RenameRegisterFilePtr = Pointer<RenameRegisterFile>;

class RenameRegisterFile
{
private:
  std::unordered_map<RegisterID, ReservationStationID, RegisterIDHash> 
    renameRegisters;

public:
  RenameRegisterFile();

  void rename(const RegisterID& reg, const ReservationStationID& rsid);
  void clearRename(const RegisterID& reg);
  void clearRename(const ReservationStationID& rsid);

  ReservationStationID getRenaming(const RegisterID& reg) const;
  RegisterID getReverseRename(const ReservationStationID& rsid) const;
};

#endif
