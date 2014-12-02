#ifndef __REGISTER_H__
#define __REGISTER_H__

#include "types.h"
#include <ostream>
#include <functional>

enum class RegisterType
{
  GPR,
  FPR
};

/**
 * Identifies a register by type and index.
 */
struct RegisterID
{
  static const RegisterID NONE;
  static const RegisterID R0;

  RegisterType type;
  std::size_t index;
};

std::ostream& operator<<(std::ostream& os, const RegisterID& reg);
bool operator==(const RegisterID& lhs, const RegisterID& rhs);
bool operator!=(const RegisterID& lhs, const RegisterID& rhs);

struct RegisterIDHash
{
  std::size_t operator()(const RegisterID& reg)
  {
    auto typeHash = std::hash<RegisterType>()(reg.type);
    auto indexHash = std::hash<std::size_t>()(reg.index);
    return typeHash ^ indexHash;
  }
};

#endif
