#include "SnDataLoader.h"
#include "lardata/ArtDataHelper/WireCreator.h"
#include "lardata/Utilities/DatabaseUtil.h" // lardata
#include "art/Framework/IO/Sources/put_product_in_principal.h"

namespace snassembler {

  bool SnDataLoader::getSupernovaTpcData(
      std::shared_ptr<gov::fnal::uboone::datatypes::ub_EventRecord> event,
      art::EventPrincipal &outArtEvent,
      const std::string& inName)
        
  {
    util::UBChannelMap_t  channelMap = art::ServiceHandle<util::DatabaseUtil>()->GetUBChannelMap(event->LocalHostTime().seb_time_sec); // Fixme: database rollback
  
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
      	  auto it_chsearch = channelMap.find(daqId);
      	  if ( it_chsearch!=channelMap.end() ){
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
             packet.resize(0);
             p.decompress_into(packet,false);
             size_t tdc = p.header().getSampleNumber();
             rois.add_range(tdc,packet.begin(),packet.end());
             // Here's where we make the actual hit.
             rois.append(packet);
             nrois ++;
           }
           std::cout << "Channel " << channel << " rois:" << rois.size() << std::endl;
         
           recob::WireCreator created_wire(rois,ch,geo::View_t::kUnknown);
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
