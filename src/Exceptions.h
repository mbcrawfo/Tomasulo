#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include "types.h"
#include "RegisterID.h"
#include "utility/IToString.h"
#include <exception>
#include <string>

/**
 * The base for for the exception hierarchy.
 */
class Exception
  : public std::exception, public util::IToString
{
protected:
  std::string msg;

public:
  Exception() = default;
  Exception(const std::string& msg);
  Exception& operator=(const Exception&) = default;
  virtual ~Exception() throw();

  virtual const char* what() const override;
  virtual std::string toString() const override;
};

/**
 * Signals an attempt to access an invalid memory location.
 */
class InvalidAddressException
  : public Exception
{
public:
  InvalidAddressException(Address addr, std::size_t size, std::size_t memSize);
};

/**
 * Non-existing register access.
 */
class InvalidRegisterException
  : public Exception
{
public:
  InvalidRegisterException(const RegisterID& reg);
};

#endif
