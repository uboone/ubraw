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
  }

  bool bnbAutoTune::operator<(ub_BeamHeader const& h) const noexcept 
  {
    return ((fSeconds < h.getSeconds()) || (fSeconds == h.getSeconds() && fMilliSeconds<h.getMilliSeconds()));
  }

  bool bnbAutoTune::operator<=(ub_BeamHeader const& h) const noexcept 
  {
    return ((fSeconds < h.getSeconds()) || (fSeconds == h.getSeconds() && fMilliSeconds<=h.getMilliSeconds()));
  }
  
  bool bnbAutoTune::operator>(ub_BeamHeader const& h) const noexcept 
  {
    return ((fSeconds > h.getSeconds()) || (fSeconds == h.getSeconds() && fMilliSeconds>h.getMilliSeconds()));
  }

  bool bnbAutoTune::operator>=(ub_BeamHeader const& h) const noexcept 
  {
    return ((fSeconds > h.getSeconds()) || (fSeconds == h.getSeconds() && fMilliSeconds>=h.getMilliSeconds()));
  }
  
  bool bnbAutoTune::operator<(raw::BeamInfo const& h) const noexcept 
  {
    return ((fSeconds < h.GetSeconds()) || (fSeconds == h.GetSeconds() && fMilliSeconds<h.GetMilliSeconds()));
  }

  bool bnbAutoTune::operator<=(raw::BeamInfo const& h) const noexcept 
  {
    return ((fSeconds < h.GetSeconds()) || (fSeconds == h.GetSeconds() && fMilliSeconds<=h.GetMilliSeconds()));
  }
  
  bool bnbAutoTune::operator>(raw::BeamInfo const& h) const noexcept 
  {
    return ((fSeconds > h.GetSeconds()) || (fSeconds == h.GetSeconds() && fMilliSeconds>h.GetMilliSeconds()));
  }

  bool bnbAutoTune::operator>=(raw::BeamInfo const& h) const noexcept 
  {
    return ((fSeconds > h.GetSeconds()) || (fSeconds == h.GetSeconds() && fMilliSeconds>=h.GetMilliSeconds()));
  }
}
