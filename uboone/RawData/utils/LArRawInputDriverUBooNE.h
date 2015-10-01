////////////////////////////////////////////////////////////////////////
/// \file  LArRawInputDriverUBooNE.h
/// \brief Source to convert raw binary files to root files for MicroBooNE
///
/// \Original Version from:
/// \version $Id: T962ConvertBinaryToROOT.h,v 1.7 2010/01/14 19:20:33 brebel Exp $
/// \author  brebel@fnal.gov, soderber@fnal.gov
/// \MicroBooNE author: jasaadi@fnal.gov, zarko@fnal.gov (with much help from Eric and Wes)
////////////////////////////////////////////////////////////////////////

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Core/ProductRegistryHelper.h"
#include "art/Framework/IO/Sources/SourceHelper.h"
#include "art/Framework/Core/FileBlock.h"
#include "art/Framework/Principal/RunPrincipal.h"
#include "art/Framework/Principal/SubRunPrincipal.h"
#include "art/Framework/Principal/EventPrincipal.h"
#include "art/Persistency/Provenance/SubRunID.h"

#include "datatypes/uboone_data_utils.h"
#include "datatypes/raw_data_access.h"
#include <boost/archive/binary_iarchive.hpp>
#include "datatypes/ub_EventRecord.h"

#include "uboone/Geometry/UBOpChannelTypes.h"
#include "Utilities/DatabaseUtil.h" // lardata

#include <fstream>
#include <vector>
#include <map>

namespace gov {
  namespace fnal {
    namespace uboone {
      namespace datatypes {
	class eventRecord;
      }
    }
  }
}

namespace raw {
  class RawDigit; 
  class BeamInfo;
  class DAQHeader;
  class Trigger;
  class OpDetWaveform;
}

class TH1D;

///Conversion of binary data to root files
namespace lris {

  class LArRawInputDriverUBooNE {
    /// Class to fill the constraints on a template argument to the class,
    /// FileReaderSource
  public:
    // Required constructor
    LArRawInputDriverUBooNE(fhicl::ParameterSet const &pset,
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

  unsigned int RollOver(unsigned int ref,
			unsigned int subject,
			unsigned int nbits);
  private:
    //Other functions

    bool processNextEvent(std::vector<raw::RawDigit>& digitList,
			  std::map< opdet::UBOpticalChannelCategory_t, std::unique_ptr< std::vector<raw::OpDetWaveform> > > & pmtDigitList,
			  raw::DAQHeader& daqHeader,
			  raw::BeamInfo& beamInfo,
			  std::vector<raw::Trigger>& trigInfo);
    void fillDAQHeaderData(gov::fnal::uboone::datatypes::ub_EventRecord& event_record,
			   raw::DAQHeader& daqHeader);
    void fillTPCData(gov::fnal::uboone::datatypes::ub_EventRecord &event_record, 
		     std::vector<raw::RawDigit>& digitList);
    void fillPMTData(gov::fnal::uboone::datatypes::ub_EventRecord &event_record, 
		     std::map< opdet::UBOpticalChannelCategory_t, std::unique_ptr< std::vector<raw::OpDetWaveform> > > & pmtDigitList );
    void fillBeamData(gov::fnal::uboone::datatypes::ub_EventRecord &event_record, 
		      raw::BeamInfo& beamInfo);
    void fillTriggerData(gov::fnal::uboone::datatypes::ub_EventRecord &event_record,
			 std::vector<raw::Trigger>& trigInfo);

    art::SourceHelper            fSourceHelper;
    art::SubRunID                  fCurrentSubRunID;
    std::ifstream                  fInputStream;
    std::vector<std::streampos>    fEventLocation;
    uint32_t                       fEventCounter; 
    uint32_t                       fNumberEventsInFile;
    bool                           fHuffmanDecode;
    util::UBChannelMap_t           fChannelMap;   
    int                            fDataTakingTime; //fhicl parameter. Optional to override raw data's internal time stamp.
    int                            fSwizzlingTime; //fhicl parameter.  Defaults as time of Hoot database query execution.
    bool                           fSwizzleTPC; //fhicl parameter.  Tells us whether to swizzle the TPC data
    bool                           fSwizzlePMT; //fhicl parameter.  Tells us whether to swizzle the PMT data
    bool                           fSaveOutput; //fhicl parameter.  Tells us whether to save the LArSoft output file or just run analysis

    //histograms
    std::map<std::string, TH1D*>   fHistMapBeam; //histograms for scalar beam devices

    // PMT Helper Methods
    std::map< opdet::UBOpticalChannelCategory_t, std::string > fPMTdataProductNames;
    void registerOpticalData( art::ProductRegistryHelper &helper );
    void putPMTDigitsIntoEvent( std::map< opdet::UBOpticalChannelCategory_t, std::unique_ptr< std::vector<raw::OpDetWaveform> > >& pmtdigitlist, art::EventPrincipal* &outE );

    // TPC Helper Methods
    std::vector<short> decodeChannelTrailer(unsigned short last_adc, unsigned short data);
    
   //Stuf that Andy added to make fun trigger plots! :)
   int N_discriminators [40];
    int discriminatorFrame [40][100];
    int discriminatorSample [40][100];
    int discriminatorType [40][100];
    uint32_t triggerFrame;
    uint32_t triggerSample;
    double triggerTime;
    uint32_t triggerActive;
    uint32_t triggerBitBNB;
    uint32_t triggerBitNuMI;
    uint32_t triggerBitEXT;
    uint32_t triggerBitPMTBeam;
    uint32_t triggerBitPMTCosmic;
    
    uint32_t FEM5triggerFrame ;
    uint32_t FEM5triggerSample;
    uint32_t FEM6triggerFrame ;
    uint32_t FEM6triggerSample;
    double FEM5triggerTime;
    double FEM6triggerTime;

    uint32_t RO_BNBtriggerFrame;
    uint32_t RO_NuMItriggerFrame;
    uint32_t RO_EXTtriggerFrame;
    uint32_t RO_RWMtriggerFrame;
    uint32_t RO_BNBtriggerSample;
    uint32_t RO_NuMItriggerSample;
    uint32_t RO_EXTtriggerSample;
    uint32_t RO_RWMtriggerSample;
    double RO_BNBtriggerTime;
    double RO_NuMItriggerTime;
    double RO_EXTtriggerTime;
    double RO_RWMtriggerTime;
    
//    uint32_t RO_Gate1Frame;
//    uint32_t RO_Gate1Sample;
//    uint32_t RO_Gate2Frame;
//    uint32_t RO_Gate2Sample;

    uint32_t TPCtriggerFrame;
    uint32_t TPCtriggerSample;
    
    uint32_t ADCwords_crate0;
    uint32_t ADCwords_crate1;
    uint32_t ADCwords_crate2;
    uint32_t ADCwords_crate3;
    uint32_t ADCwords_crate4;
    uint32_t ADCwords_crate5;
    uint32_t ADCwords_crate6;
    uint32_t ADCwords_crate7;
    uint32_t ADCwords_crate8;
    uint32_t ADCwords_crate9;
    uint32_t NumWords_crate0;
    uint32_t NumWords_crate1;
    uint32_t NumWords_crate2;
    uint32_t NumWords_crate3;
    uint32_t NumWords_crate4;
    uint32_t NumWords_crate5;
    uint32_t NumWords_crate6;
    uint32_t NumWords_crate7;
    uint32_t NumWords_crate8;
    uint32_t NumWords_crate9;

    int event;
    TTree *tMyTree;
    
  };  // LArRawInputDriverUBooNE;

}
