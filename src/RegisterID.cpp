#include "RegisterID.h"

const RegisterID RegisterID::NONE = { RegisterType::None, 0xff };

std::ostream& operator<<(std::ostream& os, const RegisterID& reg)
{
  switch (reg.type)
  {
  case RegisterType::None:
    os << "None";
    break;
  case RegisterType::GPR:
    os << "R" << reg.index;
    break;
  case RegisterType::FPR:
    os << "F" << reg.index;
  }
  return os;
}

bool operator==(const RegisterID& lhs, const RegisterID& rhs)
{
  if (lhs.type == RegisterType::None)
  {
    return rhs.type == RegisterType::None;
  }
  else
  {
    return lhs.type == rhs.type && lhs.index == rhs.index;
  }
}

bool operator!=(const RegisterID& lhs, const RegisterID& rhs)
{
  return !(lhs == rhs);
}