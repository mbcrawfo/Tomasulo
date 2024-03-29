#include "Exceptions.h"
#include "utility/stream_manip.h"
#include <sstream>

Exception::Exception(const std::string& msg)
  : msg(msg)
{
}

std::string Exception::toString() const
{
  return msg;
}

InvalidAddressException::InvalidAddressException(Address addr, std::size_t size,
  std::size_t memSize)
  : Exception()
{
  std::ostringstream os;
  os << "Attempt to access " << size << " bytes at 0x" << util::hex<Address>
    << addr << " but memory size is 0x" << util::hex < std::size_t >
    << memSize << " bytes";
  msg = os.str();
}

InvalidRegisterException::InvalidRegisterException(const RegisterID& reg)
  : Exception()
{
  std::ostringstream os;
  os << "Unknown register access: " << reg;
  msg = os.str();
}
