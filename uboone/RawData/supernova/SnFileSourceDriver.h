#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Core/ProductRegistryHelper.h"
#include "art/Framework/IO/Sources/SourceHelper.h"
#include "art/Framework/Core/FileBlock.h"
#include "art/Framework/Principal/RunPrincipal.h"
#include "art/Framework/Principal/SubRunPrincipal.h"
#include "art/Framework/Principal/EventPrincipal.h"
#include "canvas/Persistency/Provenance/SubRunID.h"
#include "art/Framework/IO/Sources/put_product_in_principal.h"

#include "datatypes/raw_data_access.h"
#include "datatypes/ub_EventRecord.h"

#include "uboone/Geometry/UBOpChannelTypes.h"


#include <fstream>
#include <vector>
#include <map>

#include "DaqFile.h"

namespace snassembler {

  class SnFileSourceDriver {
    /// Class to fill the constraints on a template argument to the class,
    /// FileReaderSource
  public:
    // Required constructor
    SnFileSourceDriver(fhicl::ParameterSet const &pset,
                      art::ProductRegistryHelper &helper,
                      art::SourceHelper const &pm);

    // Required by FileReaderSource:
    void closeCurrentFile();
    void readFile(std::string const &name,
                  art::FileBlock* &fb);
    bool readNext(art::RunPrincipal* const &inR,
                  art::SubRunPrincipal* const &inSR,
                  art::RunPrincipal* &outR,
                  art::SubRunPrincipal* &outSR,
                  art::EventPrincipal* &outE);
                  

  private:

    art::SourceHelper              fSourceHelper;
    art::SubRunID                  fCurrentSubRunID;
    std::unique_ptr<DaqFile>       fDaqFile;
    std::map< opdet::UBOpticalChannelCategory_t, std::string > fPMTdataProductNames;
  
  };  // LArRawInputDriverUBooNE

}