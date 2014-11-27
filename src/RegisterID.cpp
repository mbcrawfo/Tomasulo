#include "RegisterID.h"

const RegisterID RegisterID::NONE = {
  static_cast<RegisterType>(-1),
  static_cast<std::size_t>(-1)
};

std::ostream& operator<<(std::ostream& os, const RegisterID& reg)
{
  if (reg == RegisterID::NONE)
  {
    os << "None";
  }
  else if (reg.type == RegisterType::GPR)
  {
    os << "R" << reg.index;
  }
  else if (reg.type == RegisterType::FPR)
  {
    os << "F" << reg.index;
  }
  return os;
}

bool operator==(const RegisterID& lhs, const RegisterID& rhs)
{
  return lhs.type == rhs.type && lhs.index == rhs.index;
}

bool operator!=(const RegisterID& lhs, const RegisterID& rhs)
{
  return !(lhs == rhs);
}