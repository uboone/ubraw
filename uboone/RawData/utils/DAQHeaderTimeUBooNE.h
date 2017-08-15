////////////////////////////////////////////////////////////////////////
// Name: DAQHeaderTimeUBooNE.h
//
// Purpose: Class to hold extended DAQ header information, in particular
//          GPS and NTP (host) event time stamp.
//
// Created: 10-Aug-2017
//
////////////////////////////////////////////////////////////////////////

#ifndef DAQHEADEREXTENDED_H
#define DAQHEADEREXTENDED_H

#include <time.h>

namespace raw {

  class DAQHeaderTimeUBooNE {
  public:

    // Constructors.

    DAQHeaderTimeUBooNE();
    DAQHeaderTimeUBooNE(time_t gps_time, time_t ntp_time);

    //Set Methods
    void SetGPSTime(time_t t);
    void SetNTPTime(time_t t);

    // Accessors.

    time_t gps_time() const {return fGPSTime;}
    time_t ntp_time() const {return fNTPTime;}

  private:

    // Data members.

    time_t         fGPSTime;
    time_t         fNTPTime;
  };
}

inline void           raw::DAQHeaderTimeUBooNE::SetGPSTime(time_t t)   { fGPSTime = t; }
inline void           raw::DAQHeaderTimeUBooNE::SetNTPTime(time_t t)   { fNTPTime = t; }
#endif // DAQHEADEREXTENDED_H
