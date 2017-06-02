
#include "art/Framework/Principal/EventPrincipal.h"
#include "datatypes/uboone_data_utils.h"
#include "datatypes/raw_data_access.h"
#include "datatypes/ub_EventRecord.h"
#include "lardataobj/RecoBase/Wire.h"


namespace snassembler {

struct SnRecordHolder {

  SnRecordHolder(std::shared_ptr<gov::fnal::uboone::datatypes::ub_EventRecord> ev)
     : fEvent(ev), fNumWires(0), fMinTdc(0), fMaxTdc(0), fNumFrames(0), fTpcFrame(0), fWires(new std::vector<recob::Wire>) {};

  std::shared_ptr<gov::fnal::uboone::datatypes::ub_EventRecord> fEvent;
  size_t                                      fNumWires;
  size_t                                      fMinTdc;
  size_t                                      fMaxTdc;
  size_t                                      fNumFrames;
  uint32_t                                    fTpcFrame;
  std::unique_ptr< std::vector<recob::Wire> > fWires;
    

  bool evaluateSupernovaTpcData();
  
  bool getSupernovaTpcData( art::EventPrincipal &outArtEvent,
                            const std::string& inName = "sndaq" );
};
  
} // namespace
