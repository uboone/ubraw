#ifndef _BNBAUTOTUNE_H
#define _BNBAUTOTUNE_H

#include "datatypes/ub_BeamHeader.h"
#include "lardataobj/RawData/BeamInfo.h"

namespace bnb
{
  struct bnbOffsets {
    double hp875 = -1.;
    double vp875 = -1.;
    double hptg1 = -1.;
    double vptg1 = -1.;
    double hptg2 = -1.;
    double vptg2 = -1.;
  };

  enum bnbTG {
    kInvalidTG,
    kTG1,
    kTG2
  };

  class bnbAutoTune{
    public:
      bnbAutoTune();
      
      uint32_t getSeconds() const noexcept { return fSeconds; }
      uint16_t getMilliSeconds() const noexcept { return fMilliSeconds; } 
      uint16_t getMBFile() const noexcept { return fMBFile; }
      bnbOffsets const& getData() const noexcept { return fData; } 
      bnbTG const& getHTG() const noexcept { return fHTG; }
      bnbTG const& getVTG() const noexcept { return fVTG; }
      uint64_t getUTCTimeStamp() const noexcept { return fUTCTimeStamp; }

      void setSeconds(uint32_t const& val) noexcept { fSeconds = val; }
      void setMilliSeconds(uint16_t const& val) noexcept { fMilliSeconds = val; }
      void setMBFile(uint16_t const& val) noexcept { fMBFile = val; }
      void setData(bnbOffsets const& val) noexcept { fData = val; }
      void setHTG(bnbTG const& val) noexcept { fHTG = val; }
      void setVTG(bnbTG const& val) noexcept { fVTG = val; }
      void setEntry(uint32_t seconds, uint16_t milliseconds, 
                    uint16_t mbfile, bnbOffsets data, bnbTG htg, bnbTG vtg) noexcept;
    

      bool operator < (gov::fnal::uboone::datatypes::ub_BeamHeader const& h) const noexcept;
      bool operator <= (gov::fnal::uboone::datatypes::ub_BeamHeader const& h ) const noexcept;
      bool operator > (gov::fnal::uboone::datatypes::ub_BeamHeader const& h) const noexcept;
      bool operator >= (gov::fnal::uboone::datatypes::ub_BeamHeader const& h ) const noexcept;
    
      bool operator < (raw::BeamInfo const& h) const noexcept;
      bool operator <= (raw::BeamInfo const& h ) const noexcept;
      bool operator > (raw::BeamInfo const& h) const noexcept;
      bool operator >= (raw::BeamInfo const& h ) const noexcept;
    
      bool operator < (uint64_t const& utc) const noexcept;
      bool operator <= (uint64_t const& utc ) const noexcept;
      bool operator > (uint64_t const& utc ) const noexcept;
      bool operator >= (uint64_t const& utc ) const noexcept;
    
    private:
      uint32_t fSeconds;
      uint16_t fMilliSeconds;
      uint64_t fUTCTimeStamp;
      uint16_t fMBFile;
      bnbOffsets fData;
      bnbTG fHTG;
      bnbTG fVTG;
  };
}     

#endif
