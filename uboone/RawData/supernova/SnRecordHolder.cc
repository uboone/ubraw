///
/// Nathaniel Tagg, Otterbein University
/// ntagg@otterbein.edu
/// July 2017
///
/// 



#include "SnRecordHolder.h"
#include "lardata/ArtDataHelper/WireCreator.h"
#include "lardata/Utilities/DatabaseUtil.h" // lardata
#include "lardata/DetectorInfoServices/DetectorClocksService.h"
#include "lardata/DetectorInfo/ElecClock.h"

#include "art/Framework/IO/Sources/put_product_in_principal.h"

#include "messagefacility/MessageLogger/MessageLogger.h"



// uboonecode
#include "uboone/Geometry/UBOpChannelTypes.h"
#include "uboone/Geometry/UBOpReadoutMap.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"
#include "uboone/RawData/utils/LArRawInputDriverUBooNE.h"



#include <limits>       // std::numeric_limits
#include <fstream>

namespace snassembler {
  bool SnRecordHolder::evaluateSupernovaTpcData()
  {
    ///
    /// Quickly scan through this data file and determine basic statistics. 
    /// In particular, how many frames long is this event?
    ///
    if(fEvaluated) return true;
    
    fNumWires = 0;
    fMinTdc = std::numeric_limits<size_t>::max();
    fMaxTdc = 0;
    
    std::vector<uint16_t> packet_data_buffer; // native data type for speed
    packet_data_buffer.reserve(100);
    
    const ub_EventRecord::tpc_sn_map_t& sn_map = fEvent->getTpcSnSEBMap();
    // For us, the frame number is in fact the event number.
    fTpcFrame = fEvent->getGlobalHeader().getEventNumber();

    for( auto seb_it: sn_map ) {
      // int crate = seb_it.first;
      for(auto const& card_data: (seb_it.second).getCards() )
      {
        // int card = card_data.getModule();
        for(auto const& channel_data : card_data.getChannels() ) {
          // Channel number is
          // int channel = channel_data.getChannelNumber();
          fNumWires ++;
          if(channel_data.packets_.size()<1) continue;

          const auto& first_packet = channel_data.packets_.front();
          size_t tdc_start = first_packet.header().getSampleNumber();
          fMinTdc = std::min(fMinTdc,tdc_start);

          const auto& last_packet = channel_data.packets_.back();
          try { // Try block catches possible decompression errors.
            
            packet_data_buffer.resize(0);
            last_packet.decompress_into(packet_data_buffer,false);
            size_t tdc_last = last_packet.header().getSampleNumber() + packet_data_buffer.size();
            fMaxTdc = std::max(fMaxTdc,tdc_last);
            
          } catch (const datatypes_exception& e) {  continue;  } // We will deal with these later; this is just a peek.
          
          
          
        }
      }  
    }
    
    // This is all we want: estimate if this a 1-frame or 4-frame event.
    fNumFrames = (fMaxTdc/3200) + 1;
    std::cout << "evaluateSupernovaTpcData: Channels: " << fNumWires << " Min TDC: " << fMinTdc << " Max TDC: " << fMaxTdc 
              << "   NUMBER OF FRAMES: " << fNumFrames << std::endl;
    std::cout << "Event number is " << fTpcFrame << std::endl;
    fEvaluated = true;
    return true;
  }


  bool SnRecordHolder::getSupernovaTpcData(
      art::EventPrincipal &outArtEvent,
      const std::string& inName,
      bool remove_pedestal)
        
  {
    // Quick scan.
    if(!fEvaluated)  evaluateSupernovaTpcData(); // Quick scan
    
    std::unique_ptr< std::vector<recob::Wire> > wires;
    wires->clear();
  
    util::UBChannelMap_t  fChannelMap = art::ServiceHandle<util::DatabaseUtil>()->GetUBChannelMap(fEvent->LocalHostTime().seb_time_sec); // Fixme: database rollback
  
    size_t nrois = 0;
  
    // SN hits.
    const ub_EventRecord::tpc_sn_map_t& sn_map = fEvent->getTpcSnSEBMap();
    for( auto seb_it: sn_map ) {
      int crate = seb_it.first;
      const tpc_sn_crate_data_t& crate_data = (seb_it.second);
      // Double check frame number
      std::vector<tpc_sn_crate_data_t::card_t> const& cards = crate_data.getCards();
      for(auto const& card_data: cards )
      {
        int card = card_data.getModule();
        uint32_t frame = card_data.header().getFrame();
        if(frame!=fTpcFrame) {
          std::cout << "Event number is " << fTpcFrame << std::endl;
          std::cout << "Crate " << crate << " card " << card << " frame " << card_data.header().getFrame() << std::endl;
          // throw std::runtime_error("Supernova frame number and event number don't match on crate "+std::to_string(crate)+" card "+std::to_string(card));
        }


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

          recob::Wire::RegionsOfInterest_t rois(fMaxTdc);
        
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
                 mf::LogError("SnRecordHolder") << "Channel data dump:";
                 mf::LogError("SnRecordHolder") << channel_data.debugInfo();

                 std::string dumpfilename = "dump_frame" + std::to_string(frame) 
                       + "_crate" + std::to_string(crate)
                       + "_card" + std::to_string(card)
                       + "_channel" + std::to_string(channel)
                       + ".dump";
                 std::ofstream dumpfile(dumpfilename);
                 mf::LogError("SnRecordHolder") << "Zero sized packet at TDC " << tdc;
                 mf::LogError("SnRecordHolder") << "Dumping hex debug to file " <<dumpfilename;
                 dumpfile << channel_data.debugInfo() << std::endl;
                 dumpfile.close();

                // throw std::runtime_error("Unexpected zero-sized packet in raw data.");
                 continue; 
              }
            } else {
              packet.resize(0);
              try {
                p.decompress_into(packet,false); // False flag indicates unpacker shouldn't offset to tdc address in array when unpacking.
              } catch (const datatypes_exception& e) { 
                mf::LogError("SnRecordHolder") << "Decompression failure channel " << ch << " " << e.what();
                std::cout << channel_data.debugInfo() << std::endl;
              
                // mf::LogError("SnRecordHolder") << "Continuing anyway .... ";
                throw e;
              }
              // std::cout << Form("wire %4d  tdc %5lu  compressed %4lu uncompressed %4lu\n",ch,tdc,p.data().size(),packet.size());
              if(remove_pedestal) {
                float ped = *packet.begin();
                for(auto it = packet.begin(); it!= packet.end(); it++) *it -= ped;
              }
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
                                  outArtEvent,
                                  inName); // Module label
    
    return true;
    
  }


  bool SnRecordHolder::addSupernovaTpcData( SnRecordHolder::roimap_t& roi_map, int offset_tdc, size_t from_tdc, size_t to_tdc, size_t roi_size, bool remove_pedestal )
  {
    if(!fEvaluated)  evaluateSupernovaTpcData(); // Quick scan

    util::UBChannelMap_t  fChannelMap = art::ServiceHandle<util::DatabaseUtil>()->GetUBChannelMap(fEvent->LocalHostTime().seb_time_sec); // Fixme: database rollback
  
    size_t nrois = 0;
  
    // SN hits.
    const ub_EventRecord::tpc_sn_map_t& sn_map = fEvent->getTpcSnSEBMap();
    for( auto seb_it: sn_map ) {
      int crate = seb_it.first;
      const tpc_sn_crate_data_t& crate_data = (seb_it.second);
      // Double check frame number
      std::vector<tpc_sn_crate_data_t::card_t> const& cards = crate_data.getCards();
      for(auto const& card_data: cards )
      {
        int card = card_data.getModule();
        uint32_t frame = card_data.header().getFrame();
        if(frame!=fTpcFrame) {
          std::cout << "Event number is " << fTpcFrame << std::endl;
          std::cout << "Crate " << crate << " card " << card << " frame " << card_data.header().getFrame() << std::endl;
          // throw std::runtime_error("Supernova frame number and event number don't match on crate "+std::to_string(crate)+" card "+std::to_string(card));
          throw art::Exception(art::errors::DataCorruption,"CardHeaderFrameMismatch") << " Card header doesn't match frame on card " << card << " frame " << frame;
          
        }



        std::vector<tpc_sn_crate_data_t::card_t::card_channel_type> const& channels = card_data.getChannels();
        for(auto const& channel_data : channels ) {

          // Channel number is
          int channel = channel_data.getChannelNumber();
          
          // Important error check:
          // Does the channel frame number match the card frame? Channel header.
          uint16_t frame6 = channel_data.header().getFrameNumber_6bit();
          uint32_t channelFrame = lris::resolveFrame(frame, frame6, 0x3F);
          if(channelFrame != frame) {
            mf::LogError("SnRecordHolder" )<< "Channel header doesn't match frame on card " << card << " channel " << channel << " frame " << frame;
            mf::LogError("SnRecordHolder" )<< "This indicates corrupt data.";
            throw art::Exception(art::errors::DataCorruption,"ChannelHeaderFrameMismatch") << " Channel header doesn't match frame on card " << card << " channel " << channel << " frame " << frame;
          }
          

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
      	      mf::LogWarning("SnRecordHolder") << warn << std::endl;
              // throw std::runtime_error( warn );
              continue;
      	    }
      	  }        

          recob::Wire::RegionsOfInterest_t& rois = roi_map[ch]; // FIXME: set max value?
          rois.resize(roi_size);
        
          // size_t packets = channel_data.packets_.size();
          std::vector<float> packet;
          packet.reserve(200); // Temporary storage; reusable and exapandable

          for(auto const& p: channel_data.packets_) {
            size_t tdc = p.header().getSampleNumber();
            if(tdc > to_tdc) continue; // This packet is not wanted.
            if(p.data().size()==0) {
              // zero-sized packet. 
              if( ((tdc+1)%3200) ==0) {
                continue;  // This is a known 'feature' of the hardware: sometimes ROI packets start on the second-to-last tick and no data gets saved.
              } else {
                 mf::LogError("SnRecordHolder") << "Zero sized packet at TDC " << tdc;
                 mf::LogError("SnRecordHolder") << "Channel data dump:";
                 mf::LogError("SnRecordHolder") << channel_data.debugInfo();
                // throw std::runtime_error("Unexpected zero-sized packet in raw data.");
                 continue; 
              }
            } else {
              packet.resize(0);
              try {
                p.decompress_into(packet,false); // False flag indicates unpacker shouldn't offset to tdc address in array when unpacking.
              } catch (const datatypes_exception& e) { 
                // create a log file.
                std::string dumpfilename = "dump_frame" + std::to_string(frame) 
                      + "_crate" + std::to_string(crate)
                      + "_card" + std::to_string(card)
                      + "_channel" + std::to_string(channel)
                      + ".dump";
                std::ofstream dumpfile(dumpfilename);
                mf::LogError("SnRecordHolder") << "Decompression failure channel " << ch << " " << e.what();
                mf::LogError("SnRecordHolder") << "Dumping hex debug to file " <<dumpfilename;
                dumpfile << channel_data.debugInfo() << std::endl;
                dumpfile.close();
              
                // mf::LogError("SnRecordHolder") << "Continuing anyway .... ";
                throw e;
              }
              if(tdc+packet.size()<from_tdc) continue; // Don't need this packet.

              // std::cout << Form("wire %4d  tdc %5lu  compressed %4lu uncompressed %4lu\n",ch,tdc,p.data().size(),packet.size());
              if(remove_pedestal) {
                float ped = *packet.begin();
                for(auto it = packet.begin(); it!= packet.end(); it++) *it -= ped;
              }
              
              size_t tdc_out = tdc;
              auto begin_ = packet.begin();
              auto end_ = packet.end();
              
              if(tdc > to_tdc) continue; // Whole window is too late for use
              if(tdc+packet.size() < from_tdc) continue; // Whole window is too early for use
              // Trim end:
              if(tdc+packet.size() > to_tdc) end_-= (tdc+packet.size() - to_tdc); 
              // Trim start:
              if(tdc < from_tdc) {
                begin_ += (from_tdc-tdc);              
                tdc_out = tdc + (from_tdc-tdc); // i.e. from_tdc
              }
              if(begin_>=end_) continue; // No data to move.
              tdc_out += offset_tdc; // offset it.
              rois.add_range(tdc_out,begin_,end_);            
              nrois ++;
            }
           }
         }
      } // loop cards
    } // loop seb/crate
  
    return true;
  }
  
  

  bool SnRecordHolder::addSupernovaPmtData( pmtmap_t& pmt_map )
  {
    // PMT data is just like as in regular swizzling:
    lris::PMT_tracking_data_t dummy_tracking; // Maybe do some stats on this later.
  
    // Create a digit list to store, if it hasn't happend already.
    for ( unsigned int opdetcat=0; opdetcat<(unsigned int)opdet::NumUBOpticalChannelCategories; opdetcat++ ) {
      if(pmt_map.find((opdet::UBOpticalChannelCategory_t)opdetcat) == pmt_map.end())
        pmt_map.insert( std::make_pair( (opdet::UBOpticalChannelCategory_t)opdetcat, std::unique_ptr< std::vector<raw::OpDetWaveform> >(  new std::vector<raw::OpDetWaveform> ) ) );
    }
    //fill PMT data
    //crate -> card -> channel -> window

    auto const* timeService = lar::providerFrom<detinfo::DetectorClocksService>();
    ::art::ServiceHandle<geo::UBOpReadoutMap> ub_pmt_channel_map;
    
    // pmt channel map is assumed to be time dependent. therefore we need event time to set correct map.
    
    fEvent->getGlobalHeader().useLocalHostTime();
    uint32_t seconds=fEvent->getGlobalHeader().getSeconds();
    time_t mytime = (time_t)seconds;
    if ( mytime==0 ) {
      // some events seem o be missing time stamp. use run number in this case.
      std::cout << "[SnRecordHolder::fillPMTData] event epoch time 0 (!?). using run to set channel map" << std::endl;
      ub_pmt_channel_map->SetOpMapRun( fEvent->getGlobalHeader().getRunNumber() );
    }
    else
      ub_pmt_channel_map->SetOpMapTime( mytime );
    
    using namespace gov::fnal::uboone::datatypes;
    
    auto const seb_pmt_map = fEvent->getPMTSEBMap();
    if (seb_pmt_map.empty()) {
        std::cerr << "Warning swizzler didn't find any PMT data in the event." << std::endl;
        std::cerr << "If this is a calibration or laser run, that's ok." << std::endl;
        return false;
    }

    for(auto const& it:  seb_pmt_map) {
      pmt_crate_data_t const& crate_data = it.second;      
      std::vector<pmt_crate_data_t::card_t> const& cards = crate_data.getCards();
      for( pmt_crate_data_t::card_t const& card_data : cards ) {
                    
        for(auto const& channel_data : card_data.getChannels() ) { // auto here is pmt_crate_data_t::card_t::card_channel-type

          int channel_number = channel_data.getChannelNumber();
          auto const& windows = channel_data.getWindows();  // auto here is std::vector<ub_PMT_WindowData_v6>
          for(const auto& window: windows ) {               // auto here is ub_PMT_WindowData_v6
            const auto& window_header = window.header();    // auto here is ub_PMT_WindowHeader_v6
            const ub_RawData& window_data = window.data();
            size_t win_data_size=window_data.size();

            uint32_t sample=window_header.getSample();
            uint32_t frame = lris::resolveFrame(card_data.getFrame(),window_header.getFrame(),0x7);

            unsigned int data_product_ch_num = ub_pmt_channel_map->GetChannelNumberFromCrateSlotFEMCh( crate_data.crateHeader()->crate_number, card_data.getModule(), channel_number );
            
            // here we translate crate/card/daq channel to data product channel number
            // also need to go from clock time to time stamp
            opdet::UBOpticalChannelCategory_t ch_category = ub_pmt_channel_map->GetChannelCategory( data_product_ch_num );
            double window_timestamp = timeService->OpticalClock().Time( sample, frame );

            raw::OpDetWaveform rd( window_timestamp, data_product_ch_num, win_data_size);
            rd.reserve(win_data_size); // More efficient. push_back is terrible without it.

            for(ub_RawData::const_iterator it = window_data.begin(); it!= window_data.end(); it++){ 
              rd.push_back(*it & 0xfff);                
            }
            // std::cout << " opwaveform " << opdet::UBOpChannelEnumName( (opdet::UBOpticalChannelCategory_t)ch_category )
            //           << " frame,sample (" << frame << "," << sample << ") time " << window_timestamp
            //           << "Card frame: " << card_data.getFrame() << "  Window frame: " << window_header.getFrame()
            //             << std::hex << "  0x" << frame <<  "  0x"<< card_data.getFrame() <<   "  0x"<< window_header.getFrame() << std::dec
            //           << std::endl;
            pmt_map[ch_category]->emplace_back(rd);
          }
        }//<--End channel_pmt_it for loop
      }//<---End card_pmt_it for loop
    }//<---End seb_pmt_it for loop
    return true;
  }

} // namespace
