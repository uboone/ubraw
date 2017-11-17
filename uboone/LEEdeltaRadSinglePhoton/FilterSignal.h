

#ifndef FILTERSIGNAL_H
#define FILTERSIGNAL_H


#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"


#include "nusimdata/SimulationBase/MCTruth.h"


bool FilterSignal(art::Event const & e, size_t & delta_mct_index) {

  art::ValidHandle<std::vector<simb::MCTruth>> const & ev_mct =
    e.getValidHandle<std::vector<simb::MCTruth>>("generator");

  size_t signal_counter = 0;

  for(size_t mct_index = 0; mct_index < ev_mct->size(); ++mct_index) {
 
    simb::MCTruth const & mct = ev_mct->at(mct_index);

    if(mct.GetNeutrino().CCNC() == 0) continue;
    
    size_t external_photon_parent_index = SIZE_MAX;
    bool continue_bool = false;

    for(int i = 0; i < mct.NParticles(); ++i) {
      simb::MCParticle const & mcp = mct.GetParticle(i);
      if(mcp.TrackId() != i) {
	std::cout << "ERROR: " << __LINE__ << " " << __PRETTY_FUNCTION__ << "\nTrackId does not match index\n";
	exit(1);
      }
      if(mcp.StatusCode() != 1) continue;
      switch(abs(mcp.PdgCode())) {
      case 22: {
	if(external_photon_parent_index == SIZE_MAX) {
	  external_photon_parent_index = mcp.Mother();
	}
	else {
	  continue_bool = true;
	}
	break;
      }
      case 12:
      case 14:
      case 2112:
      case 2212:
	break;
      default:
	continue_bool = true;
      }
      if(continue_bool) break;
    }
    
    if(external_photon_parent_index == SIZE_MAX || continue_bool) 
      continue;   

    if(mct.GetParticle(external_photon_parent_index).PdgCode() == 22)
      external_photon_parent_index = mct.GetParticle(external_photon_parent_index).Mother();

    if(!(abs(mct.GetParticle(external_photon_parent_index).PdgCode()) == 2214 || abs(mct.GetParticle(external_photon_parent_index).PdgCode()) == 2114))
      continue;
    
    delta_mct_index = mct_index;
    ++signal_counter;
    
  }

  /*
  for(size_t mct_index = 0; mct_index < ev_mct->size(); ++mct_index) {
    simb::MCTruth const & mct = ev_mct->at(mct_index);
    for(int i = 0; i < mct.NParticles(); ++i) {
      simb::MCParticle const & mcp = mct.GetParticle(i);
      std::cout << mcp.TrackId() << " " << mcp.PdgCode() << " " << mcp.Mother() << " " << mcp.StatusCode() << "\n";
    }
    if(signal_counter == 0) std::cout << "Fail\n";
    else std::cout << "Pass\n";
    std::cout << "\n";
  }
  */

  return signal_counter > 0;
  
}



#endif
