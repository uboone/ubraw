////////////////////////////////////////////////////////////////////////
// $Id: DAQHeaderTimeUBooNE.cxx,v 1.0 2017/08/10 19:34:20 brebel Exp $
//
// DAQHeaderTimeUBooNE class
//
// kirby@fnal.gov
//
////////////////////////////////////////////////////////////////////////

#include "lardata/RawData/DAQHeader.h"
#include "uboone/RawData/utils/DAQHeaderTimeUBooNE.h"

namespace raw{

  //----------------------------------------------------------------------
  // Default constructor.

  DAQHeaderTimeUBooNE::DAQHeaderTimeUBooNE() :
    fGPSTime(0),
    fNTPTime(0)
  {}

  //----------------------------------------------------------------------
  // Initializing constructor.
  DAQHeaderTimeUBooNE::DAQHeaderTimeUBooNE(time_t gps_time, time_t ntp_time) :
    fGPSTime(gps_time),
    fNTPTime(ntp_time)
  {}
}
