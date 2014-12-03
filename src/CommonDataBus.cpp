#include "CommonDataBus.h"
#include "ReservationStation.h"
#include "log.h"
#include "utility/stream_manip.h"
#include <string>
#include <cassert>
#include <algorithm>
#include <iostream>

static const std::string TAG = "CommonDataBus";

CommonDataBus::CommonDataBus(RegisterFilePtr registers,
  RenameRegisterFilePtr renameRegisters)
  : used(false),
    idleThisCycle(true),
    source(nullptr),
    sourceID(ReservationStationID::NONE),
    destID(RegisterID::NONE),
    value(),
    registers(registers),
    renameRegisters(renameRegisters),
    listeners(),
    rejected()
{
  assert(registers != nullptr);
  assert(renameRegisters != nullptr);
}

void CommonDataBus::write(ReservationStation* src)
{
  assert(src != nullptr);

  if (source == nullptr)
  {
    source = src;
  }
  else
  {
    if (src->getStartClock() < source->getStartClock())
    {
      logger->verbose(TAG) << src->getID() << " evicted " << source->getID();
      rejected.push_back(source);
      source = src;
    }
    else
    {
      rejected.push_back(src);
    }
  }

  used = true;
  idleThisCycle = false;
}

void CommonDataBus::commit()
{
  if (used)
  {
    sourceID = source->getID();
    destID = source->getDest();
    value = source->getResult();    
    source->notifyWriteAccepted();
    source = nullptr;

    logger->debug(TAG) << sourceID << " wrote " << destID << "="
      << util::hex<UWord> << value.uw;
    for (auto rs : rejected)
    {
      logger->debug(TAG) << rs->getID() << " could not write";
    }
    rejected.clear();

    notifyListeners();
    registers->write(destID, value);

    logger->debug(TAG) << "Committed " << destID << "="
      << util::hex<UWord> << value.uw << " to register file";
        
    used = false;
  }
  else
  {
    idleThisCycle = true;
  }
}

void CommonDataBus::dumpState() const
{
  std::cout << "CDB: ";
  if (idleThisCycle)
  {
    std::cout << "Empty" << std::endl;
  }
  else
  {
    std::cout << sourceID << "=" << util::hex<UWord> << value.uw
      << std::endl;
  }
}

void CommonDataBus::addListener(ReservationStation* rs)
{
  assert(rs != nullptr);
  listeners.push_back(rs);
}

void CommonDataBus::notifyListeners()
{
  auto pred = [&](ReservationStation* rs) {
    return rs->notifyDataBus(sourceID, value);
  };
  listeners.remove_if(pred);
}

