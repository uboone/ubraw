///
/// Nathaniel Tagg, Otterbein University
/// ntagg@otterbein.edu
/// July 2017
///
/// 

#include "art/Framework/Principal/EventPrincipal.h"
#include "datatypes/uboone_data_utils.h"
#include "datatypes/raw_data_access.h"
#include "datatypes/ub_EventRecord.h"
#include "lardataobj/RecoBase/Wire.h"
#include "lardataobj/RawData/OpDetWaveform.h"
#include "uboone/Geometry/UBOpChannelTypes.h"


namespace snassembler {

struct SnRecordHolder {

  SnRecordHolder(std::shared_ptr<gov::fnal::uboone::datatypes::ub_EventRecord> ev)
     : fEvent(ev), fNumWires(0), fMinTdc(0), fMaxTdc(0), fNumFrames(0), fTpcFrame(0), fEvaluated(false) 
       { evaluateSupernovaTpcData(); };

  std::shared_ptr<gov::fnal::uboone::datatypes::ub_EventRecord> fEvent;
  size_t                                      fNumWires;
  size_t                                      fMinTdc;
  size_t                                      fMaxTdc;
  size_t                                      fNumFrames;
  uint32_t                                    fTpcFrame;
  bool                                        fEvaluated;

  bool evaluateSupernovaTpcData();
  
  // This version is defunct; use addSupernovaTpcData instead.
  bool getSupernovaTpcData( art::EventPrincipal &outArtEvent,
                            const std::string& inName = "sndaq", bool remove_pedestal=false );

  typedef std::map<int,recob::Wire::RegionsOfInterest_t> roimap_t;

  bool addSupernovaTpcData( roimap_t& roi_map, // Array of data to add to. Will be turned into Wires.
                            int offset_tdc,                 // Offset data I retrieve by this much into the output 
                            size_t from_tdc, size_t to_tdc, // insert this range of values from the eventRecord
                            size_t roi_size,                // size of the roi object on output
                            bool remove_pedestal );
                            
  typedef std::map< opdet::UBOpticalChannelCategory_t, std::unique_ptr< std::vector<raw::OpDetWaveform> > >  pmtmap_t;
                            
  bool addSupernovaPmtData( pmtmap_t& pmt_map );

};
  
} // namespace
