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
#include "canvas/Persistency/Provenance/SubRunID.h"

#include "datatypes/uboone_data_utils.h"
#include "datatypes/raw_data_access.h"
#include <boost/archive/binary_iarchive.hpp>
#include "datatypes/ub_EventRecord.h"
#include "uboone/RawData/utils/ubdaqSoftwareTriggerData.h"

#include "uboone/Geometry/UBOpChannelTypes.h"
#include "lardata/Utilities/DatabaseUtil.h" // lardata

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
  class DAQHeaderTimeUBooNE;
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
			  std::map< opdet::UBOpticalChannelCategory_t,
			  std::unique_ptr< std::vector<raw::OpDetWaveform> > > & pmtDigitList,
			  raw::DAQHeader& daqHeader,
			  raw::DAQHeaderTimeUBooNE& daqHeaderTimeUBooNE,
			  raw::BeamInfo& beamInfo,
			  std::vector<raw::Trigger>& trigInfo,
			  raw::ubdaqSoftwareTriggerData& sw_trigInfo,
			  uint32_t& event_number,
			  bool skip);
    void fillDAQHeaderData(gov::fnal::uboone::datatypes::ub_EventRecord& event_record,
			   raw::DAQHeader& daqHeader, raw::DAQHeaderTimeUBooNE& daqHeaderTimeUBooNE);
    void fillTPCData(gov::fnal::uboone::datatypes::ub_EventRecord &event_record, 
		     std::vector<raw::RawDigit>& digitList);
    void fillPMTData(gov::fnal::uboone::datatypes::ub_EventRecord &event_record, 
		     std::map< opdet::UBOpticalChannelCategory_t, std::unique_ptr< std::vector<raw::OpDetWaveform> > > & pmtDigitList );
    void fillBeamData(gov::fnal::uboone::datatypes::ub_EventRecord &event_record, 
		      raw::BeamInfo& beamInfo);
    void fillTriggerData(gov::fnal::uboone::datatypes::ub_EventRecord &event_record,
			 std::vector<raw::Trigger>& trigInfo);
    void fillSWTriggerData(gov::fnal::uboone::datatypes::ub_EventRecord &event_record,
                        raw::ubdaqSoftwareTriggerData& trigInfo);

    void checkTimeStampConsistency(void);
    
    double _trigger_beam_window_time;

    art::SourceHelper            fSourceHelper;
    art::SubRunID                  fCurrentSubRunID;
    std::ifstream                  fInputStream;
    std::vector<std::streampos>    fEventLocation;
    uint32_t                       fEventCounter; 
    uint32_t                       fNumberEventsInFile;
    uint32_t                       fFinalEventCutOff;
    std::ios::streampos            fPreviousPosition;
    bool                           fCompleteFile;
    bool                           fHuffmanDecode;
    bool                           fUseGPS;    // fhicl parameter force use GPS time.
    bool                           fUseNTP;    // fhicl parameter force use NTP time.
    util::UBChannelMap_t           fChannelMap;
    int                            fMaxEvents; //fhicl parameter.  Maximum number of events.
    int                            fSkipEvents; // fhicl parameter.  Number of events to skip.
    int                            fDataTakingTime; //fhicl parameter. Optional to override raw data's internal time stamp.
    int                            fSwizzlingTime; //fhicl parameter.  Defaults as time of Hoot database query execution.
    bool                           fSwizzleTPC; //fhicl parameter.  Tells us whether to swizzle the TPC data
    bool                           fSwizzlePMT; //fhicl parameter.  Tells us whether to swizzle the PMT data
    bool                           fSwizzlePMT_init; //stored initial fhicl parameter telling whether to swizzle PMT data
                                                    // using this to reset fSwizzlePMT for events without PMT data but
                                                    // with the fSwizzlePMT fhicl parameter set to true
                                                    // (fix crash in checkTimeStampConsistency)
    bool                           fSwizzleTrigger; //fhicl parameter.  Tells us whether to swizzle the trigger data. (desired if we don't care about frame slippage)
    bool                           fEnforceFrameMatching; //fhicl parameter. Should be set to true unless using for debugging.  False allows the swizzler to complete swizzling without the event or trigger frames matching across different headers in an event.
    std::string                    fSwizzleTriggerType; //fhicl parameter.  Tells us whether to swizzle a specific trigger type only. Options are ALL, BNB, NuMI, CALIB
    bool skipEvent; // tag to skip event if trigger is not the type we want.

    bool kazuTestSwizzleTrigger;

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
    double PMT_waveform_times[400];
    int N_PMT_waveforms; 

    int triggerFrame;
    int triggerSample;
    double triggerTime;
    uint32_t triggerActive;
    uint32_t triggerBitBNB;
    uint32_t triggerBitNuMI;
    uint32_t triggerBitEXT;
    uint32_t triggerBitPMTBeam;
    uint32_t triggerBitPMTCosmic;
    uint32_t triggerBitPaddles;

    int RO_BNBtriggerFrame;
    int RO_NuMItriggerFrame;
    int RO_EXTtriggerFrame;
    int RO_RWMtriggerFrame;
    int RO_BNBtriggerSample;
    int RO_NuMItriggerSample;
    int RO_EXTtriggerSample;
    int RO_RWMtriggerSample;
    double RO_BNBtriggerTime;
    double RO_NuMItriggerTime;
    double RO_EXTtriggerTime;
    double RO_RWMtriggerTime;
   
    int RO_LEDFlashTriggerFrame;
    int RO_LEDtriggerFrame;
    int RO_PaddleTriggerFrame;
    int RO_HVtriggerFrame;
    int RO_LEDFlashTriggerSample;
    int RO_LEDtriggerSample;
    int RO_PaddleTriggerSample;
    int RO_HVtriggerSample;
    double RO_LEDFlashTriggerTime;
    double RO_LEDtriggerTime;
    double RO_PaddleTriggerTime;
    double RO_HVtriggerTime;

    int RO_NuMIRWMtriggerFrame; // NuMI RWM signal
    int RO_NuMIRWMtriggerSample;// NuMI RWM signal
    double RO_NuMIRWMtriggerTime;// NuMI RWM signal

    // internal checking variable for tpc
    // We check each card matches - the first card in each is saved as the "crate" value
    int TPCtriggerFrame;  // tpc trigger frame number - each crate is also saved individually for cross-checking
    int TPCeventFrame;	// tpc "event packet" frame number - each crate is also saved individually for cross-checking
    int TPCtriggerSample; // tpc trigger sample number - each crate is also saved individually for cross checking
    
    int TPC1triggerFrame;	// internal checking variable for tpc crate 1
    int TPC1eventFrame;
    int TPC1triggerSample;
    int TPC2triggerFrame;	// internal checking variable for tpc crate 2
    int TPC2eventFrame;
    int TPC2triggerSample;
    int TPC3triggerFrame;	// internal checking variable for tpc crate 3
    int TPC3eventFrame;
    int TPC3triggerSample;
    int TPC4triggerFrame;	// internal checking variable for tpc crate 4
    int TPC4eventFrame;
    int TPC4triggerSample;
    int TPC5triggerFrame;	// internal checking variable for tpc crate 5
    int TPC5eventFrame;
    int TPC5triggerSample;
    int TPC6triggerFrame;	// internal checking variable for tpc crate 6
    int TPC6eventFrame;
    int TPC6triggerSample;
    int TPC7triggerFrame;	// internal checking variable for tpc crate 7
    int TPC7eventFrame;
    int TPC7triggerSample;
    int TPC8triggerFrame;	// internal checking variable for tpc crate 8
    int TPC8eventFrame;
    int TPC8triggerSample;
    int TPC9triggerFrame;	// internal checking variable for tpc crate 9
    int TPC9eventFrame;
    int TPC9triggerSample;
    
    // same for the pmt FEMs.
    // This time, we keep the information for each PMT FEM separately (FEMs are in slots 4,5,6)
    int PMTtriggerFrame;	
    int PMTeventFrame;	
    int PMTtriggerSample;

    int PMTFEM4triggerFrame;	
    int PMTFEM4eventFrame;	
    int PMTFEM4triggerSample;
    int PMTFEM5triggerFrame;	
    int PMTFEM5eventFrame;	
    int PMTFEM5triggerSample;
    int PMTFEM6triggerFrame;	
    int PMTFEM6eventFrame;	
    int PMTFEM6triggerSample;


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
    uint32_t NumWords_crate1;
    uint32_t NumWords_crate2;
    uint32_t NumWords_crate3;
    uint32_t NumWords_crate4;
    uint32_t NumWords_crate5;
    uint32_t NumWords_crate6;
    uint32_t NumWords_crate7;
    uint32_t NumWords_crate8;
    uint32_t NumWords_crate9;

    int N_trig_algos;
    std::vector<std::string> algo_instance_name;
    bool pass_algo[20];
    bool pass_prescale[20];

    int event;
    TTree *ValidationTree;
    
  };  // LArRawInputDriverUBooNE;

}
