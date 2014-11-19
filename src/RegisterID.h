#ifndef __REGISTER_H__
#define __REGISTER_H__

#include "types.h"
#include <ostream>

enum class RegisterType
{
  None,
  GPR,
  FPR
};

/**
 * Identify a register by type and index.
 */
struct RegisterID
{
  static const RegisterID NONE;

  RegisterType type;
  std::size_t index;
};

std::ostream& operator<<(std::ostream& os, const RegisterID& reg);
bool operator==(const RegisterID& lhs, const RegisterID& rhs);
bool operator!=(const RegisterID& lhs, const RegisterID& rhs);

namespace std
{
template<>
struct hash<RegisterID>
{
  std::size_t operator()(const RegisterID& reg)
  {
    auto typeHash = std::hash<RegisterType>()(reg.type);
    auto indexHash = std::hash<std::size_t>()(reg.index);
    return typeHash ^ indexHash;
  }
};
}

#endif
