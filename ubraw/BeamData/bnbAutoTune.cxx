#include "bnbAutoTune.h"

using namespace gov::fnal::uboone::datatypes;

namespace bnb
{
  bnbAutoTune::bnbAutoTune():
    fSeconds(0),
    fMilliSeconds(0),
    fMBFile(0),
    fHTG(kInvalidTG),
    fVTG(kInvalidTG)
  {
  }

  void bnbAutoTune::setEntry(uint32_t seconds, uint16_t milliseconds, uint16_t mbfile,
                             bnbOffsets data, bnbTG htg, bnbTG vtg) noexcept
  {
    fSeconds = seconds;
    fMilliSeconds = milliseconds;
    fMBFile = mbfile;
    fData = data;
    fHTG = htg;
    fVTG = vtg;
    fUTCTimeStamp = convertUTC(fSeconds, fMilliSeconds);
    fLocalTimeStamp = convertTimeStamp(fSeconds, fMilliSeconds);
  }

  // comparison here is with local time coordinates, same as autotune history so its straightforward
  bool bnbAutoTune::operator<(ub_BeamHeader const& h) const noexcept
  {
    uint64_t beamltc = convertTimeStamp(h.getSeconds(), h.getMilliSeconds());
    return (fLocalTimeStamp < beamltc);
  }

  bool bnbAutoTune::operator<=(ub_BeamHeader const& h) const noexcept
  {
    uint64_t beamltc = convertTimeStamp(h.getSeconds(), h.getMilliSeconds());
    return (fLocalTimeStamp <= beamltc);
  }

  bool bnbAutoTune::operator>(ub_BeamHeader const& h) const noexcept
  {
    uint64_t beamltc = convertTimeStamp(h.getSeconds(), h.getMilliSeconds());
    return (fLocalTimeStamp > beamltc);
  }

  bool bnbAutoTune::operator>=(ub_BeamHeader const& h) const noexcept
  {
    uint64_t beamltc = convertTimeStamp(h.getSeconds(), h.getMilliSeconds());
    return (fLocalTimeStamp >= beamltc);
  }

  // BeamInfo is in UTC
  bool bnbAutoTune::operator<(raw::BeamInfo const& h) const noexcept
  {
    uint64_t beamutc = convertTimeStamp(h.GetSeconds(), h.GetMilliSeconds());
    return (fUTCTimeStamp < beamutc);
  }

  bool bnbAutoTune::operator<=(raw::BeamInfo const& h) const noexcept
  {
    uint64_t beamutc = convertTimeStamp(h.GetSeconds(), h.GetMilliSeconds());
    return (fUTCTimeStamp <= beamutc);
  }

  bool bnbAutoTune::operator>(raw::BeamInfo const& h) const noexcept
  {
    uint64_t beamutc = convertTimeStamp(h.GetSeconds(), h.GetMilliSeconds());
    return (fUTCTimeStamp > beamutc);
  }

  bool bnbAutoTune::operator>=(raw::BeamInfo const& h) const noexcept
  {
    uint64_t beamutc = convertTimeStamp(h.GetSeconds(), h.GetMilliSeconds());
    return (fUTCTimeStamp >= beamutc);
  }

  // compare directly to utc timestamps
  bool bnbAutoTune::operator<(uint64_t const& utc) const noexcept
  {
    return (fUTCTimeStamp < utc);
  }

  bool bnbAutoTune::operator<=(uint64_t const& utc) const noexcept
  {
    return (fUTCTimeStamp <= utc);
  }

  bool bnbAutoTune::operator>(uint64_t const& utc) const noexcept
  {
    return (fUTCTimeStamp > utc);
  }

  bool bnbAutoTune::operator>=(uint64_t const& utc) const noexcept
  {
    return (fUTCTimeStamp >= utc);
  }
}
