
#include "art/Framework/Principal/EventPrincipal.h"
#include "datatypes/uboone_data_utils.h"
#include "datatypes/raw_data_access.h"
#include "datatypes/ub_EventRecord.h"
#include "lardataobj/RecoBase/Wire.h"


namespace snassembler {

struct SnDataLoader {


  std::unique_ptr< std::vector<recob::Wire> > fWires;
  
  bool getSupernovaTpcData(
      std::shared_ptr<gov::fnal::uboone::datatypes::ub_EventRecord> inDaqEvent,
      art::EventPrincipal &outArtEvent,
      const std::string& inName = "sndaq" );
};
  
} // namespace
