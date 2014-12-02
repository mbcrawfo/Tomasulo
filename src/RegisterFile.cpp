#include "RegisterFile.h"
#include "Exceptions.h"

RegisterFile::RegisterFile(std::size_t numGPR, std::size_t numFPR)
  : registers()
{
  Data data = { 0 };
  for (std::size_t i = 0; i < numGPR; ++i)
  {
    RegisterID gpr = { RegisterType::GPR, i };
    registers[gpr] = data;
  }
  for (std::size_t i = 0; i < numFPR; ++i)
  {
    RegisterID fpr = { RegisterType::FPR, i };
    registers[fpr] = data;
  }
}

Data RegisterFile::read(const RegisterID& reg) const
{
  if (reg == RegisterID::NONE)
  {
    Data data = { 0 };
    return data;
  }

  auto itr = registers.find(reg);
  if (itr == registers.end())
  {
    throw InvalidRegisterException(reg);
  }

  return itr->second;
}

void RegisterFile::write(const RegisterID& reg, Data data)
{
  if (reg == RegisterID::NONE || reg == R0)
  {
    return;
  }

  auto itr = registers.find(reg);
  if (itr == registers.end())
  {
    throw InvalidRegisterException(reg);
  }

  itr->second = data;
}
