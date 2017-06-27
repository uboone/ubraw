#include "SnRecordHolder.h"
#include "lardata/ArtDataHelper/WireCreator.h"
#include "lardata/Utilities/DatabaseUtil.h" // lardata
#include "art/Framework/IO/Sources/put_product_in_principal.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

// uboonecode

#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"



#include <limits>       // std::numeric_limits

namespace snassembler {
  bool SnRecordHolder::evaluateSupernovaTpcData()
  {
    ///
    /// Quickly scan through this data file and determine basic statistics. 
    /// In particular, how many frames long is this event?
    ///
    
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
    return true;
  }


  bool SnRecordHolder::getSupernovaTpcData(
      art::EventPrincipal &outArtEvent,
      const std::string& inName,
      bool remove_pedestal)
        
  {
    // Quick scan.
    evaluateSupernovaTpcData();
    
    fWires->clear();
  
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
           fWires->push_back(created_wire.move());
        }
      } // loop cards
    } // loop seb/crate

    std::cout << "Built wires with total " << nrois << " ROIs" << std::endl;
    
    art::put_product_in_principal(std::move(fWires),
                                  outArtEvent,
                                  inName); // Module label
    
    return true;
    
  }


}
