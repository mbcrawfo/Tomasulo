#ifndef __REGISTERFILE_H__
#define __REGISTERFILE_H__

#include "types.h"
#include "RegisterID.h"
#include <unordered_map>

class RegisterFile;
using RegisterFilePtr = Pointer<RegisterFile>;

class RegisterFile
{
private:
  std::unordered_map<RegisterID, Data> registers;

public:
  RegisterFile() = delete;
  RegisterFile(std::size_t numGPR, std::size_t numFPR);

  Data read(const RegisterID& reg) const;
  void write(const RegisterID& reg, Data data);
};

#endif
