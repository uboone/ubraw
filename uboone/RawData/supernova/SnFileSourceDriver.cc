///
/// Nathaniel Tagg, Otterbein University
/// ntagg@otterbein.edu
/// July 2017
///
/// 

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
#include <fstream>

namespace snassembler {


SnFileSourceDriver::SnFileSourceDriver(fhicl::ParameterSet const &pset,
                      art::ProductRegistryHelper &helper,
                      art::SourceHelper const &pm)
                        : fSourceHelper(pm)
{
 
  mf::LogInfo("SnFileSourceDriver") << "SnFileSourceDriver constructor"; 
  mf::LogInfo("SnFileSourceDriver") << pset.to_string();
  
  fRemovePedestal = pset.get< bool       >("RemovePedestal"      , false);
  
  fTriggerRecordsOnly  = pset.get< bool  >("TriggerRecordsOnly" ,false);
  fSplitTriggerRecords = pset.get< bool  >("SplitTriggerRecords",true);
  fSamplesOverlapPre   = pset.get< int   >("SamplesOverlapPre"  ,1600);
  fSamplesOverlapPost  = pset.get< int   >("SamplesOverlapPost" ,1600);
  fTotalSamplesPerRecord=pset.get< int   >("TotalSamplesPerRecord",3200+fSamplesOverlapPre+fSamplesOverlapPost);

  std::string logfilename = pset.get< std::string >("LogFile","");
  if(logfilename.size()>0)
    fLogfile = std::shared_ptr<std::ofstream>(new std::ofstream(logfilename));


  if(fTotalSamplesPerRecord < 3200+fSamplesOverlapPre+fSamplesOverlapPost) {
    mf::LogError("SnFileSourceDriver") << "Inconsistent TotalSamplesPerRecord. Suggested values: 3200+pre+post OR 12800 if getting whole trigger records.";
    throw art::Exception(art::errors::Configuration,"SnFileSourceDriver") << "Inconsistent TotalSamplesPerRecord";
  }
  
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
  
  advance();  
}

bool SnFileSourceDriver::advance()
{
  // We actually need to get another record from the disk, and push one element along our little three-element fifo (prev,curr,next)
  fPrevRecord.swap(fCurrRecord);
  fCurrRecord.swap(fNextRecord);
  fNextRecord.reset();

  // Get the next event.
  std::shared_ptr<gov::fnal::uboone::datatypes::ub_EventRecord> event;
  try{
    event = fDaqFile->GetNextEvent();
    std::shared_ptr<SnRecordHolder> s(new SnRecordHolder(event));
    fNextRecord.swap(s);
    return true;
  } catch(std::runtime_error& e) {
    mf::LogWarning("SnFileSourceDriver") << e.what();
    return false;
  }
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

  // First question: do we need to advance into another record?
  int do_subframe = 0;
  if(  fCurrRecord                                                               // We have one we're processed once already
    && fCurrRecord->fNumFrames>1                                                 // It's a trigger frame
    && fSplitTriggerRecords                                                      // We're splitting trigger frames
    && ((fCurrentFrame+1) < (fCurrRecord->fTpcFrame + fCurrRecord->fNumFrames) )  // we've not yet got all frames processed
      ) 
  {
    fCurrentFrame++; // Advance event counter
    do_subframe = fCurrentFrame-(fCurrRecord->fTpcFrame);
  } else {
    
    // We actually need to get another record from the disk, and push one element along our little three-element fifo (prev,curr,next)
    advance();

    if(fTriggerRecordsOnly) {
      while(fCurrRecord && fCurrRecord->fNumFrames==1) {
        advance();      
      }
    }
    if(fCurrRecord) fCurrentFrame = fCurrRecord->fTpcFrame;
  }


  if(! fCurrRecord) {
    // Can happen if first event didn't load in readFile()
    mf::LogWarning("SnFileSourceDriver") << " No more data?";
    return false;
  }


  // Header and principal
  std::unique_ptr<raw::DAQHeader> daq_header(new raw::DAQHeader);
  lris::fillDAQHeaderData(*(fCurrRecord->fEvent),*daq_header,
                          false, // use gps
                          true ); // use ntp
    
                          // FIXME: Timestamp in DAQHeaderData is wrong if do_subframe is not zero.

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
  
  uint32_t event_number = fCurrentFrame+1;  
        // Add do_suframe in case we're looking at a subset of the current record
        // Add +1 because ART is a miserable misbegotten SoB who cant' deal with with a zero in this field. 
  
  outE = fSourceHelper.makeEventPrincipal(fCurrentSubRunID.run(),
			      fCurrentSubRunID.subRun(),
			      event_number,
			      tstamp);
  // mf::LogInfo("SnFileSourceDriver") << "Event " << fCurrentSubRunID.run() << "|" <<fCurrentSubRunID.subRun() << "|" << event_number;
  std::cout << "----Event " << fCurrentSubRunID.run() << "|" <<fCurrentSubRunID.subRun() << "|" << event_number << std::endl;
  
  
  art::put_product_in_principal(std::move(daq_header),
                                *outE,
                                "sndaq"); // Module label


  // Fake up a trigger.
  std::unique_ptr<std::vector<raw::Trigger>> trig_info( new std::vector<raw::Trigger> );

  auto const* timeService = lar::providerFrom<detinfo::DetectorClocksService>();
  double trigger_time = timeService->OpticalClock().Time(0,fCurrRecord->fTpcFrame+do_subframe);
  raw::Trigger trigger( 0,  // trigger number
                          trigger_time, // Trigger time
                          trigger_time, // Beam time. Not sure if this should be blank or not...
                          0 // Trigger bits
                         );

  trig_info->emplace_back( trigger );
  art::put_product_in_principal(std::move(trig_info),
                                *outE,
                                "sndaq"); // Module label
   
  fCurrRecord->evaluateSupernovaTpcData(); // Make sure
  if(fLogfile) *fLogfile <<"ART event: " << "----Event " << fCurrentSubRunID.run() << "|" <<fCurrentSubRunID.subRun() << "|" << event_number << "   ";
  if(fLogfile) *fLogfile << "Frame " << fCurrRecord->fTpcFrame << "   ";
  if(fLogfile) *fLogfile << "CurrFrame " << fCurrentFrame << "   ";
  if(fLogfile) *fLogfile << "subframe " << do_subframe << "   ";
  if(fLogfile) *fLogfile << "Frames: " << fCurrRecord->fNumFrames << std::endl;
  if(fLogfile) fLogfile->flush();

  /// PMT          
  SnRecordHolder::pmtmap_t pmt_map;
  fCurrRecord->addSupernovaPmtData( pmt_map );

  if(fSamplesOverlapPre>0  && fPrevRecord && do_subframe==0) {
    fPrevRecord->addSupernovaPmtData( pmt_map );    // Add PMT hits from previous frame.
  }
  if(fSamplesOverlapPost>0 && fNextRecord && ((fCurrRecord->fNumFrames - do_subframe)==1)) {
    fNextRecord->addSupernovaPmtData( pmt_map );    // Add PMT hits from next frame.
  }
  
  // Store.
  if(pmt_map.size()>0)
    lris::putPMTDigitsIntoEvent(pmt_map,outE,fPMTdataProductNames);

  
  // fCurrRecord->getSupernovaTpcData(*outE,"sndaq",fRemovePedestal);
  SnRecordHolder::roimap_t roi_map;
  int offset_tdc = fSamplesOverlapPre; // Offset this far
  size_t from_tdc = 0;
  size_t to_tdc = fTotalSamplesPerRecord;
  if(do_subframe>0) {
    offset_tdc = (-3200)*do_subframe; // Shift back in time N frames
    from_tdc   = 3200*do_subframe;    // Look at ticks from 3200*N on
    to_tdc     = from_tdc + fTotalSamplesPerRecord;
  }
  fCurrRecord->addSupernovaTpcData( roi_map, offset_tdc, from_tdc, to_tdc, fTotalSamplesPerRecord, fRemovePedestal );

  if(fSamplesOverlapPre>0 && fPrevRecord && do_subframe==0) {
    // Hits from prev frame
    from_tdc = fPrevRecord->fNumFrames*(3200) - fSamplesOverlapPre;   // TDC start time in prev record
    to_tdc   = fPrevRecord->fNumFrames*(3200);                         // TDC end time in prev record
    offset_tdc = -from_tdc ; // Shift back before start of frame
    fPrevRecord->addSupernovaTpcData(roi_map, offset_tdc, from_tdc, to_tdc, fTotalSamplesPerRecord, fRemovePedestal );
  }
  
  if(fSamplesOverlapPost && fNextRecord && ((fCurrRecord->fNumFrames - do_subframe)==1)) {
    // Hits from prev frame
    from_tdc = 0;
    to_tdc   = fSamplesOverlapPost;
    offset_tdc = fTotalSamplesPerRecord-fSamplesOverlapPost ; // Shift to end of frame
    fNextRecord->addSupernovaTpcData(roi_map, offset_tdc, from_tdc, to_tdc, fTotalSamplesPerRecord, fRemovePedestal );
  }


  std::unique_ptr< std::vector<recob::Wire> > wires(new std::vector<recob::Wire>);
  for(auto item: roi_map) {
    int ch = item.first;
    auto rois = item.second;
    recob::WireCreator created_wire(rois, 
                                         ch, 
                                         art::ServiceHandle<geo::Geometry>()->View(ch)
                                         );
    
    wires->push_back(created_wire.move());
  }

  art::put_product_in_principal(std::move(wires),
                                *outE,
                                "sndaq"); // Module label
  
  
  
  

 
  return true;
}


} // namespace