///
/// Nathaniel Tagg, Otterbein University
/// ntagg@otterbein.edu
/// July 2017
///
/// 
#ifndef DAQFILE_H
#define DAQFILE_H

#include "datatypes/ub_EventRecord.h"
#include <fstream>
#include <vector>
#include <memory>

///
/// Code to open and read events from a DAQ binary file.
///

namespace snassembler {

class DaqFile
{
  public:
    DaqFile( const std::string& pathname );
    virtual ~DaqFile();
    bool Good()          { return good; };
    bool ClosedCleanly() { return closedCleanly; };
    int  NumEvents()     { return nevents; }; /// only valid if ClosedCleanly() 
    // bool  GetEventData(size_t entry, char* &outEventData, size_t &outEventSize); // returns true if good.
    std::shared_ptr<gov::fnal::uboone::datatypes::ub_EventRecord> GetEvent(size_t entry, bool unpack=true);
    std::shared_ptr<gov::fnal::uboone::datatypes::ub_EventRecord> GetNextEvent(bool unpack=true);

  private:
    bool      good;
    bool      closedCleanly;
    uint32_t  m_entry;
    uint32_t  nevents;
    std::vector<uint32_t>  index_buffer; 
    std::ifstream  ifs;
};

}

#endif /* end of include guard: DAQFILE_H */
