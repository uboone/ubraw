#include "SnFileSourceDriver.h"
#include "SnRecordHolder.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <vector>
#include "lardataobj/RawData/TriggerData.h"
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/DAQHeader.h"
#include "lardataobj/RawData/OpDetWaveform.h"

// uboonecode
#include "uboone/Geometry/UBOpChannelTypes.h"
#include "uboone/Geometry/UBOpReadoutMap.h"
#include "uboone/Geometry/UBOpChannelTypes.h"
#include "uboone/RawData/utils/LArRawInputDriverUBooNE.h"

// larsoft
#include "lardata/Utilities/DatabaseUtil.h" // lardata
#include "lardataobj/RecoBase/Wire.h"
#include "lardata/ArtDataHelper/WireCreator.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"
#include "lardata/DetectorInfoServices/DetectorClocksService.h"


#include "canvas/Utilities/Exception.h"


namespace snassembler {

SnFileSourceDriver::SnFileSourceDriver(fhicl::ParameterSet const &pset,
                      art::ProductRegistryHelper &helper,
                      art::SourceHelper const &pm)
                        : fSourceHelper(pm)
{
 
  mf::LogError("SnFileSourceDriver") << "SnFileSourceDriver constructor"; 
  mf::LogInfo("SnFileSourceDriver") << pset.to_string();
  
  fRemovePedestal = pset.get< bool        >("remove_pedestal"      , false);
  
  helper.reconstitutes< raw::DAQHeader, art::InEvent>("sndaq");
  helper.reconstitutes< std::vector<raw::DAQHeader>, art::InEvent>("sndaq");
  helper.reconstitutes< std::vector<recob::Wire> ,   art::InEvent>("sndaq");  
  helper.reconstitutes<std::vector<raw::Trigger>,    art::InEvent>("sndaq");
  
  lris::registerOpticalData( helper, fPMTdataProductNames ); 
}


void SnFileSourceDriver::closeCurrentFile()
{
  mf::LogError("SnFileSourceDriver") << "SnFileSourceDriver closeCurrentFile()";
  fDaqFile = nullptr;
  
}

void SnFileSourceDriver::readFile(std::string const &name,
                                  art::FileBlock* &fb)              
{
  mf::LogError("SnFileSourceDriver") << "SnFileSourceDriver readFile()"; 
  mf::LogInfo("SnFileSourceDriver") << "Requested name: " << name; 
  // Fill and return a new Fileblock.
  fb = new art::FileBlock(art::FileFormatVersion(1, "SnAssembler codename-toriccelli"),
                          name);
  mf::LogInfo("SnFileSourceDriver") << "FileBlock created.";
  fDaqFile = std::make_unique<DaqFile>( name );
  mf::LogInfo("SnFileSourceDriver") << "DaqFile created.";
  if(!fDaqFile->Good()) throw std::runtime_error("Could not open file "+name);
}


bool SnFileSourceDriver::readNext(
              art::RunPrincipal* const &inR,
              art::SubRunPrincipal* const &inSR,
              art::RunPrincipal* &outR,
              art::SubRunPrincipal* &outSR,
              art::EventPrincipal* &outE)
{
  using namespace gov::fnal::uboone::datatypes;
  
  mf::LogError("SnFileSourceDriver") << "SnFileSourceDriver readNext()"; 

  // Get the event.
   std::shared_ptr<gov::fnal::uboone::datatypes::ub_EventRecord> event;
  try{
    event = fDaqFile->GetNextEvent();
  } catch(std::runtime_error& e) {
    mf::LogWarning("SnFileSourceDriver") << e.what();
    return false;
  }

  // Header and principal
  std::unique_ptr<raw::DAQHeader> daq_header(new raw::DAQHeader);
  lris::fillDAQHeaderData(*event,*daq_header,
                          false, // use gps
                          true ); // use ntp
    

  art::RunNumber_t rn = daq_header->GetRun();//+1;
  art::Timestamp tstamp = daq_header->GetTimeStamp();
  art::SubRunID newID(rn, daq_header->GetSubRun());
  if (fCurrentSubRunID.runID() != newID.runID()) { // New Run
    outR = fSourceHelper.makeRunPrincipal(rn, tstamp);
  }
  if (fCurrentSubRunID != newID) { // New SubRun
    outSR = fSourceHelper.makeSubRunPrincipal(rn,
                                              daq_header->GetSubRun(),
                                              tstamp);
    fCurrentSubRunID = newID;        
  }
  
  uint32_t event_number = event->getGlobalHeader().getEventNumber()+1;  // Because art is a miserable misbegotten SoB who cant' deal with with a zero in this field. 
  outE = fSourceHelper.makeEventPrincipal(fCurrentSubRunID.run(),
			      fCurrentSubRunID.subRun(),
			      event_number,
			      tstamp);
  
  
  art::put_product_in_principal(std::move(daq_header),
                                *outE,
                                "sndaq"); // Module label

            
  /// PMT          
  // PMT data is just like as in regular swizzling:
  lris::PMT_tracking_data_t dummy_tracking; // Maybe do some stats on this later.
  
  // Create a digit list to store.
  std::map< opdet::UBOpticalChannelCategory_t, std::unique_ptr< std::vector<raw::OpDetWaveform> > > pmtDigitList;
  for ( unsigned int opdetcat=0; opdetcat<(unsigned int)opdet::NumUBOpticalChannelCategories; opdetcat++ ) {
    pmtDigitList.insert( std::make_pair( (opdet::UBOpticalChannelCategory_t)opdetcat, std::unique_ptr< std::vector<raw::OpDetWaveform> >(  new std::vector<raw::OpDetWaveform> ) ) );
  }
  // Regular Swizzle code does the work
  lris::fillPMTData(*event,pmtDigitList,
                    false, // use gps
                    true,  // use ntp
                    dummy_tracking);

  // Store.
  if(pmtDigitList.size()>0)
    lris::putPMTDigitsIntoEvent(pmtDigitList,outE,fPMTdataProductNames);

                                
                                
  mf::LogInfo("SnFileSourceDriver") << "Getting Hoot Gibson lookup... please wait..."; 
   
 
  SnRecordHolder holder(event);
  holder.getSupernovaTpcData(*outE,"sndaq",fRemovePedestal);
  
  // Fake up a trigger.
  std::unique_ptr<std::vector<raw::Trigger>> trig_info( new std::vector<raw::Trigger> );

  auto const* timeService = lar::providerFrom<detinfo::DetectorClocksService>();
  double trigger_time = timeService->OpticalClock(0,holder.fTpcFrame).Time();
  raw::Trigger trigger( 0,  // trigger number
                          trigger_time, // Trigger time
                          trigger_time, // Beam time. Not sure if this should be blank or not...
                        0 // Trigger bits
                         );

  trig_info->emplace_back( trigger );
  art::put_product_in_principal(std::move(trig_info),
                                *outE,
                                "sndaq"); // Module label
 
  return true;
}


} // namespace