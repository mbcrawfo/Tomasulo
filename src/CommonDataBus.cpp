#include "CommonDataBus.h"
#include "ReservationStation.h"
#include "log.h"
#include "utility/stream_manip.h"
#include <string>
#include <cassert>
#include <algorithm>

static const std::string TAG = "CommonDataBus";

CommonDataBus::CommonDataBus(RegisterFilePtr registers,
  RenameRegisterFilePtr renameRegisters)
  : used(false),
    source(ReservationStationID::NONE),
    dest(RegisterID::NONE),
    value(),
    registers(registers),
    renameRegisters(renameRegisters),
    listeners()
{
  assert(registers != nullptr);
  assert(renameRegisters != nullptr);
}

bool CommonDataBus::set(const ReservationStationID& source,
  const RegisterID& dest, Data value)
{
  if (used)
  {
    logger->debug(TAG) << "Is in use, cannot write " << source;
    return false;
  }

  this->source = source;
  this->dest = dest;
  this->value = value;
  used = true;
  logger->debug(TAG) << source << " wrote "
    << dest << "=" << util::hex<UWord> << value.uw;

  return true;
}

void CommonDataBus::writeAndClear()
{
  if (used)
  {
    registers->write(dest, value);
    logger->debug(TAG) << "Committed " << dest << "=" 
      << util::hex<UWord> << value.uw << " to register file";

    renameRegisters->clearRename(source);
    used = false;
    source = ReservationStationID::NONE;
    dest = RegisterID::NONE;
  }
}

void CommonDataBus::notifyAll()
{
  if (used)
  {
    auto pred = [&](ReservationStation* rs) {
      if (rs->notify(source, value))
      {
        return true;
      }
      return false;
    };
    listeners.remove_if(pred);
  }
}

void CommonDataBus::addListener(ReservationStation* rs)
{
  assert(rs != nullptr);
  listeners.push_back(rs);
}

void CommonDataBus::removeListener(ReservationStation* rs)
{
  listeners.remove(rs);
}
