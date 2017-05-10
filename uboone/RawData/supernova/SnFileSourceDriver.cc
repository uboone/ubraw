#include "SnFileSourceDriver.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <vector>
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

#include "canvas/Utilities/Exception.h"


namespace snassembler {

SnFileSourceDriver::SnFileSourceDriver(fhicl::ParameterSet const &pset,
                      art::ProductRegistryHelper &helper,
                      art::SourceHelper const &pm)
                        : fSourceHelper(pm)
{
 
  mf::LogError("SnFileSourceDriver") << "SnFileSourceDriver constructor"; 
  mf::LogInfo("SnFileSourceDriver") << pset.to_string();
  
  helper.reconstitutes< raw::DAQHeader, art::InEvent>("sndaq");
  helper.reconstitutes< std::vector<raw::DAQHeader>, art::InEvent>("sndaq");
  helper.reconstitutes< std::vector<recob::Wire> , art::InEvent>("sndaq");  
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
  
  util::UBChannelMap_t  fChannelMap = art::ServiceHandle<util::DatabaseUtil>()->GetUBChannelMap(event->LocalHostTime().seb_time_sec); // Fixme: database rollback
  
  std::unique_ptr< std::vector<recob::Wire> > wires(new std::vector<recob::Wire>);
  
  size_t nrois = 0;
  
  // SN hits.
  const ub_EventRecord::tpc_sn_map_t& sn_map = event->getTpcSnSEBMap();
  for( auto seb_it: sn_map ) {
    int crate = seb_it.first;
    const tpc_sn_crate_data_t& crate_data = (seb_it.second);
    std::vector<tpc_sn_crate_data_t::card_t> const& cards = crate_data.getCards();
    for(auto const& card_data: cards )
    {
      int card = card_data.getModule();
      std::vector<tpc_sn_crate_data_t::card_t::card_channel_type> const& channels = card_data.getChannels();
      for(auto const& channel_data : channels ) {

        // Channel number is
        int channel = channel_data.getChannelNumber();

     	  util::UBDaqID daqId( crate, card, channel);
    	  int ch=0;
    	  auto it_chsearch = fChannelMap.find(daqId);
    	  if ( it_chsearch!=fChannelMap.end() ){
    	    ch=(*it_chsearch).second;
    	  }
    	  else {
    	    if ( ( crate==1 && card==8 && (channel>=32 && channel<64) ) ||
           		 ( crate==9 && card==5 && (channel>=32 && channel<64) ) ) {
    	      // As of 6/22/2016: We expect these FEM channels to have no database entry.
    	      continue; // do not write to data product
    	    }
    	    else {
    	      // unexpected channels are missing. throw.
    	      char warn[256];
    	      sprintf( warn, "Warning DAQ ID not found ( %d, %d, %d )!", crate, card, channel );
    	      std::cout << warn << std::endl;
            // throw std::runtime_error( warn );
            continue;
    	    }
    	  }        

        recob::Wire::RegionsOfInterest_t rois;
        
        // size_t packets = channel_data.packets_.size();
        std::vector<float> packet;
        packet.reserve(3200);
        for(auto const& p: channel_data.packets_) {
          size_t tdc = p.header().getSampleNumber();          
          if(p.data().size()==0) {
            // zero-sized packet. 
            if( ((tdc+1)%3200) ==0) {
              continue;  // This is a known 'feature' of the hardware: sometimes ROI packets start on the second-to-last tick and no data gets saved.
            } else {
               mf::LogError("SnFileSourceDriver") << "Zero sized packet at TDC " << tdc;
               mf::LogError("SnFileSourceDriver") << "Channel data dump:";
               mf::LogError("SnFileSourceDriver") << channel_data.debugInfo();
              // throw std::runtime_error("Unexpected zero-sized packet in raw data.");
               continue;
            }
          } else {
            packet.resize(0);
            try {
              p.decompress_into(packet,false); // False flag indicates unpacker shouldn't offset to tdc address in array when unpacking.
            } catch (const datatypes_exception& e) {
              mf::LogError("SnFileSourceDriver") << "Decompression failure channel " << ch << " " << e.what();
              std::cout << channel_data.debugInfo() << std::endl;
              
              mf::LogError("SnFileSourceDriver") << "Continuing anyway .... ";
              throw e;
            }
            // std::cout << Form("wire %4d  tdc %5lu  compressed %4lu uncompressed %4lu\n",ch,tdc,p.data().size(),packet.size());
            rois.add_range(tdc,packet.begin(),packet.end());            
            nrois ++;
          }
         }
         // std::cout << "Channel " << channel << " rois:" << rois.size() << std::endl;
         
         
         recob::WireCreator created_wire(rois, 
                                         ch, 
                                         art::ServiceHandle<geo::Geometry>()->View(ch)
                                         );
         wires->push_back(created_wire.move());
      }
    } // loop cards
  } // loop seb/crate

  std::cout << "Built wires with total " << nrois << " ROIs" << std::endl;

  art::put_product_in_principal(std::move(wires),
                                *outE,
                                "sndaq"); // Module label


  return true;
}


} // namespace