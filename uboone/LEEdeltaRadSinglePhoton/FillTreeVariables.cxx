
#ifndef FILLTREEVARIABLES_CXX
#define FILLTREEVARIABLES_CXX

#include "FillTreeVariables.h"

#include "uboone/EventWeight/MCEventWeight.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "larsim/MCCheater/BackTracker.h"

#include "FilterSignal.h"

FillTreeVariables::FillTreeVariables() :
  fmcrecomatching(false),
  ftpc_volume(0,
	      -lar::providerFrom<geo::Geometry>()->DetHalfHeight(),
	      0,
	      2*lar::providerFrom<geo::Geometry>()->DetHalfWidth(),
	      lar::providerFrom<geo::Geometry>()->DetHalfHeight(),
	      lar::providerFrom<geo::Geometry>()->DetLength()),
  foffset(10),
  ffiducial_volume(foffset,
		   foffset-lar::providerFrom<geo::Geometry>()->DetHalfHeight(),
		   foffset,
		   -foffset+2*lar::providerFrom<geo::Geometry>()->DetHalfWidth(),
		   -foffset+lar::providerFrom<geo::Geometry>()->DetHalfHeight(),
		   -foffset+lar::providerFrom<geo::Geometry>()->DetLength()),
  fwire_plane(2),
  fverbose(false),
  fevent_tree(nullptr),
  fvertex_tree(nullptr) {
  

}


void FillTreeVariables::SetProducers(std::string const & mcordata,
				     bool const mcrecomatching,
				     std::string const & track_producer,
				     std::string const & shower_producer,
				     std::string const & opflash_producer) {

  fmcordata = mcordata;
  fmcrecomatching = mcrecomatching;
  ftrack_producer = track_producer;
  fshower_producer = shower_producer;
  fopflash_producer = opflash_producer;  

}


void FillTreeVariables::SetUpTreeBranches() {

  art::ServiceHandle< art::TFileService > tfs;
  fevent_tree = tfs->make<TTree>("event_tree", "");
  fvertex_tree = tfs->make<TTree>("vertex_tree", "");

  fevent_tree->Branch("run_number", &run_number, "run_number/I");
  fevent_tree->Branch("subrun_number", &subrun_number, "subrun_number/I"); 
  fevent_tree->Branch("event_number", &event_number, "event_number/I");

  fevent_tree->Branch("tracknumber", &tracknumber, "tracknumber/I");
  fevent_tree->Branch("showernumber", &showernumber, "showernumber/I");
    
  fevent_tree->Branch("passed_swtrigger", &passed_swtrigger, "passed_swtrigger/I");
  fevent_tree->Branch("totalpe_sum", &totalpe_sum, "totalpe_sum/D");
  fevent_tree->Branch("totalpe_ibg_sum", &totalpe_ibg_sum, "totalpe_ibg_sum/D");
  fevent_tree->Branch("totalpe_bbg_sum", &totalpe_bbg_sum, "totalpe_bbg_sum/D");

  fevent_tree->Branch("most_energetic_reco_shower", &most_energetic_reco_shower, "most_energetic_reco_shower/D");
  fevent_tree->Branch("second_most_energetic_reco_shower", &second_most_energetic_reco_shower, "second_most_energetic_reco_shower/D");

  if(fmcordata == "mc") {

    fevent_tree->Branch("external_singlephoton_protons_neutrons_only", &external_singlephoton_protons_neutrons_only, "external_singlephoton_protons_neutrons_only/I");

    fevent_tree->Branch("nu_pdg", &nu_pdg, "nu_pdg/I");
    fevent_tree->Branch("nu_energy", &nu_energy, "nu_energy/D");
    fevent_tree->Branch("lep_pdg", &lep_pdg, "lep_pdg/I");
    fevent_tree->Branch("lep_energy", &lep_energy, "lep_energy/D");
    fevent_tree->Branch("ccnc", &ccnc, "ccnc/I");
    fevent_tree->Branch("mode", &mode, "mode/I");
    fevent_tree->Branch("interaction_type", &interaction_type, "interaction_type/I");

    fevent_tree->Branch("exiting_photon_number", &exiting_photon_number, "exiting_photon_number/I");
    fevent_tree->Branch("exiting_proton_number", &exiting_proton_number, "exiting_proton_number/I");
    fevent_tree->Branch("exiting_neutron_number", &exiting_neutron_number, "exiting_neutron_number/I");
    fevent_tree->Branch("exiting_electron_number", &exiting_electron_number, "exiting_electron_number/I");
    fevent_tree->Branch("exiting_antielectron_number", &exiting_antielectron_number, "exiting_antielectron_number/I");
    fevent_tree->Branch("exiting_muon_number", &exiting_muon_number, "exiting_muon_number/I");
    fevent_tree->Branch("exiting_antimuon_number", &exiting_antimuon_number, "exiting_antimuon_number/I");
    fevent_tree->Branch("exiting_piplus_number", &exiting_piplus_number, "exiting_piplus_number/I");
    fevent_tree->Branch("exiting_piminus_number", &exiting_piminus_number, "exiting_piminus_number/I");
    fevent_tree->Branch("exiting_pi0_number", &exiting_pi0_number, "exiting_pi0_number/I");
    fevent_tree->Branch("total_exiting_particles", &total_exiting_particles, "total_exiting_particles/I"); 
    fevent_tree->Branch("exiting_particle_vector", &exiting_particle_vector); 

    fevent_tree->Branch("is_single_photon", &is_single_photon, "is_single_photon/I");
    fevent_tree->Branch("is_delta_rad", &is_delta_rad, "is_delta_rad/I");
    fevent_tree->Branch("delta_true_pdg", &delta_true_pdg, "delta_true_pdg/I");
    fevent_tree->Branch("delta_true_energy", &delta_true_energy, "delta_true_energy/D");
    fevent_tree->Branch("delta_photon_energy", &delta_photon_energy, "delta_photon_energy/D");
    fevent_tree->Branch("delta_proton_energy", &delta_proton_energy, "delta_proton_energy/D");
    fevent_tree->Branch("delta_mcshower_true_pdg", &delta_mcshower_true_pdg, "delta_mcshower_true_pdg/I");
    fevent_tree->Branch("delta_mcshower_true_energy", &delta_mcshower_true_energy, "delta_mcshower_true_energy/D");
    fevent_tree->Branch("delta_mcshower_detprofile_energy", &delta_mcshower_detprofile_energy, "delta_mcshower_detprofile_energy/D");
    fevent_tree->Branch("delta_mctrack_true_pdg", &delta_mctrack_true_pdg, "delta_mctrack_true_pdg/I");
    fevent_tree->Branch("delta_mctrack_true_energy", &delta_mctrack_true_energy, "delta_mctrack_true_energy/D");
    fevent_tree->Branch("delta_mctrack_true_length", &delta_mctrack_true_length, "delta_mctrack_true_length/D");
    
    fevent_tree->Branch("mctracknumber", &mctracknumber, "mctracknumber/I");
    fevent_tree->Branch("mcshowernumber", &mcshowernumber, "mcshowernumber/I");

    fevent_tree->Branch("true_nuvertx", &true_nuvertx, "true_nuvertx/D");
    fevent_tree->Branch("true_nuverty", &true_nuverty, "true_nuverty/D");
    fevent_tree->Branch("true_nuvertz", &true_nuvertz, "true_nuvertz/D");

    fevent_tree->Branch("true_nu_E", &true_nu_E, "true_nu_E/D");
    
    fevent_tree->Branch("true_nu_vtx_tpc_contained", &true_nu_vtx_tpc_contained, "true_nu_vtx_tpc_contained/I"); 
    fevent_tree->Branch("true_nu_vtx_fid_contained", &true_nu_vtx_fid_contained, "true_nu_vtx_fid_contained/I"); 
  }

  fvertex_tree->Branch("run_number", &run_number, "run_number/I");
  fvertex_tree->Branch("subrun_number", &subrun_number, "subrun_number/I"); 
  fvertex_tree->Branch("event_number", &event_number, "event_number/I");
  fvertex_tree->Branch("tracknumber", &tracknumber, "tracknumber/I");
  fvertex_tree->Branch("showernumber", &showernumber, "showernumber/I");
    
  fvertex_tree->Branch("passed_swtrigger", &passed_swtrigger, "passed_swtrigger/I");
  fvertex_tree->Branch("totalpe_sum", &totalpe_sum, "totalpe_sum/D");
  fvertex_tree->Branch("totalpe_ibg_sum", &totalpe_ibg_sum, "totalpe_ibg_sum/D");
  fvertex_tree->Branch("totalpe_bbg_sum", &totalpe_bbg_sum, "totalpe_bbg_sum/D");

  fvertex_tree->Branch("shower_dist_to_flashzcenter", &shower_dist_to_flashzcenter, "shower_dist_to_flashzcenter/D");
  fvertex_tree->Branch("shower_dist_to_flashzcenter_splitshowers", &shower_dist_to_flashzcenter_splitshowers, "shower_dist_to_flashzcenter_splitshowers/D");
    
  fvertex_tree->Branch("selected", &selected, "selected/I");
    
  fvertex_tree->Branch("reco_nuvertx", &reco_nuvertx, "reco_nuvertx/D");
  fvertex_tree->Branch("reco_nuverty", &reco_nuverty, "reco_nuverty/D");
  fvertex_tree->Branch("reco_nuvertz", &reco_nuvertz, "reco_nuvertz/D");

  fvertex_tree->Branch("reco_nu_vtx_dist_to_closest_tpc_wall", &reco_nu_vtx_dist_to_closest_tpc_wall, "reco_nu_vtx_dist_to_closest_tpc_wall/D");
  fvertex_tree->Branch("reco_true_nuvert_dist", &reco_true_nuvert_dist, "reco_true_nuvert_dist/D");    
    
  fvertex_tree->Branch("reco_nu_vtx_fid_contained", &reco_nu_vtx_fid_contained, "reco_nu_vtx_fid_contained/I");
    
  fvertex_tree->Branch("reco_asso_tracks", &reco_asso_tracks, "reco_asso_tracks/I");
  fvertex_tree->Branch("reco_asso_showers", &reco_asso_showers, "reco_asso_showers/I");

  fvertex_tree->Branch("longest_asso_track_displacement", &longest_asso_track_length, "longest_asso_track_displacement/D");
  fvertex_tree->Branch("longest_asso_track_reco_dirx", &longest_asso_track_reco_dirx, "longest_asso_track_reco_dirx/D");
  fvertex_tree->Branch("longest_asso_track_reco_diry", &longest_asso_track_reco_diry, "longest_asso_track_reco_diry/D");
  fvertex_tree->Branch("longest_asso_track_reco_dirz", &longest_asso_track_reco_dirz, "longest_asso_track_reco_dirz/D");
  fvertex_tree->Branch("longest_asso_track_thetayx", &longest_asso_track_thetayx, "longest_asso_track_thetayx/D");
  fvertex_tree->Branch("longest_asso_track_thetaxz", &longest_asso_track_thetaxz, "longest_asso_track_thetaxz/D");
  fvertex_tree->Branch("longest_asso_track_thetayz", &longest_asso_track_thetayz, "longest_asso_track_thetayz/D");

  fvertex_tree->Branch("closest_asso_shower_dist_to_flashzcenter", &closest_asso_shower_dist_to_flashzcenter, "closest_asso_shower_dist_to_flashzcenter/D");

  fvertex_tree->Branch("most_energetic_shower_reco_startx", &most_energetic_shower_reco_startx, "most_energetic_shower_reco_startx/D");
  fvertex_tree->Branch("most_energetic_shower_reco_starty", &most_energetic_shower_reco_starty, "most_energetic_shower_reco_starty/D");
  fvertex_tree->Branch("most_energetic_shower_reco_startz", &most_energetic_shower_reco_startz, "most_energetic_shower_reco_startz/D");   
  fvertex_tree->Branch("most_energetic_shower_reco_dist", &most_energetic_shower_reco_dist, "most_energetic_shower_reco_dist/D");
  fvertex_tree->Branch("most_energetic_shower_reco_distx", &most_energetic_shower_reco_distx, "most_energetic_shower_reco_distx/D");
  fvertex_tree->Branch("most_energetic_shower_reco_disty", &most_energetic_shower_reco_disty, "most_energetic_shower_reco_disty/D");
  fvertex_tree->Branch("most_energetic_shower_reco_distz", &most_energetic_shower_reco_distz, "most_energetic_shower_reco_distz/D");
  fvertex_tree->Branch("most_energetic_shower_reco_thetayx", &most_energetic_shower_reco_thetayx, "most_energetic_shower_reco_thetayx/D");
  fvertex_tree->Branch("most_energetic_shower_reco_thetaxz", &most_energetic_shower_reco_thetaxz, "most_energetic_shower_reco_thetaxz/D");
  fvertex_tree->Branch("most_energetic_shower_reco_thetayz", &most_energetic_shower_reco_thetayz, "most_energetic_shower_reco_thetayz/D"); 
  fvertex_tree->Branch("most_energetic_shower_reco_width0", &most_energetic_shower_reco_width0, "most_energetic_shower_reco_width0/D");
  fvertex_tree->Branch("most_energetic_shower_reco_width1", &most_energetic_shower_reco_width1, "most_energetic_shower_reco_width1/D");
  fvertex_tree->Branch("most_energetic_shower_reco_opening_angle", &most_energetic_shower_reco_opening_angle, "most_energetic_shower_reco_opening_angle/D");
  fvertex_tree->Branch("most_energetic_shower_reco_length", &most_energetic_shower_reco_length, "most_energetic_shower_reco_length/D");
  fvertex_tree->Branch("most_energetic_shower_reco_dirx", &most_energetic_shower_reco_dirx, "most_energetic_shower_reco_dirx/D");
  fvertex_tree->Branch("most_energetic_shower_reco_diry", &most_energetic_shower_reco_diry, "most_energetic_shower_reco_diry/D");
  fvertex_tree->Branch("most_energetic_shower_reco_dirz", &most_energetic_shower_reco_dirz, "most_energetic_shower_reco_dirz/D");
  fvertex_tree->Branch("most_energetic_shower_reco_energy", &most_energetic_shower_reco_energy, "most_energetic_shower_reco_energy/D");
  fvertex_tree->Branch("most_energetic_shower_helper_energy", &most_energetic_shower_helper_energy, "most_energetic_shower_helper_energy/D");
  fvertex_tree->Branch("reco_shower_energy_vector", &reco_shower_energy_vector);
  fvertex_tree->Branch("most_energetic_shower_bp_dist_to_tpc", &most_energetic_shower_bp_dist_to_tpc, "most_energetic_shower_bp_dist_to_tpc/D");
  fvertex_tree->Branch("reco_shower_dedx_vector",  &reco_shower_dedx_vector);
  fvertex_tree->Branch("reco_shower_dedx_plane0", &reco_shower_dedx_plane0, "reco_shower_dedx_plane0/D");
  fvertex_tree->Branch("reco_shower_dedx_plane1", &reco_shower_dedx_plane1, "reco_shower_dedx_plane1/D");
  fvertex_tree->Branch("reco_shower_dedx_plane2", &reco_shower_dedx_plane2, "reco_shower_dedx_plane2/D");
  fvertex_tree->Branch("reco_shower_dedx_best_plane", &reco_shower_dedx_best_plane, "reco_shower_dedx_best_plane/D");
  fvertex_tree->Branch("closest_shower_dedx_plane2", &closest_shower_dedx_plane2, "closest_shower_dedx_plane2/D");

  fvertex_tree->Branch("shortest_asso_shower_to_vert_dist", &shortest_asso_shower_to_vert_dist, "shortest_asso_shower_to_vert_dist/D");
  fvertex_tree->Branch("summed_associated_reco_shower_energy", &summed_associated_reco_shower_energy, "summed_associated_reco_shower_energy/D");
  fvertex_tree->Branch("summed_associated_helper_shower_energy", &summed_associated_helper_shower_energy, "summed_associated_helper_shower_energy/D");
  fvertex_tree->Branch("summed_associated_helper_track_energy", &summed_associated_helper_track_energy, "summed_associated_helper_track_energy/D");
    
  fvertex_tree->Branch("pass_all_cuts", &pass_all_cuts, "pass_all_cuts/I");

  if(fmcordata == "mc") {

    fvertex_tree->Branch("external_singlephoton_protons_neutrons_only", &external_singlephoton_protons_neutrons_only, "external_singlephoton_protons_neutrons_only/I");

    fvertex_tree->Branch("nu_pdg", &nu_pdg, "nu_pdg/I");
    fvertex_tree->Branch("nu_energy", &nu_energy, "nu_energy/D");
    fvertex_tree->Branch("lep_pdg", &lep_pdg, "lep_pdg/I");
    fvertex_tree->Branch("lep_energy", &lep_energy, "lep_energy/D");
    fvertex_tree->Branch("ccnc", &ccnc, "ccnc/I");
    fvertex_tree->Branch("mode", &mode, "mode/I");
    fvertex_tree->Branch("interaction_type", &interaction_type, "interaction_type/I");

    fvertex_tree->Branch("exiting_photon_number", &exiting_photon_number, "exiting_photon_number/I");
    fvertex_tree->Branch("exiting_proton_number", &exiting_proton_number, "exiting_proton_number/I");
    fvertex_tree->Branch("exiting_neutron_number", &exiting_neutron_number, "exiting_neutron_number/I");
    fvertex_tree->Branch("exiting_electron_number", &exiting_electron_number, "exiting_electron_number/I");
    fvertex_tree->Branch("exiting_antielectron_number", &exiting_antielectron_number, "exiting_antielectron_number/I");
    fvertex_tree->Branch("exiting_muon_number", &exiting_muon_number, "exiting_muon_number/I");
    fvertex_tree->Branch("exiting_antimuon_number", &exiting_antimuon_number, "exiting_antimuon_number/I");
    fvertex_tree->Branch("exiting_piplus_number", &exiting_piplus_number, "exiting_piplus_number/I");
    fvertex_tree->Branch("exiting_piminus_number", &exiting_piminus_number, "exiting_piminus_number/I");
    fvertex_tree->Branch("exiting_pi0_number", &exiting_pi0_number, "exiting_pi0_number/I");
    fvertex_tree->Branch("total_exiting_particles", &total_exiting_particles, "total_exiting_particles/I"); 
    fvertex_tree->Branch("exiting_particle_vector", &exiting_particle_vector);

    fvertex_tree->Branch("is_single_photon", &is_single_photon, "is_single_photon/I");
    fvertex_tree->Branch("is_delta_rad", &is_delta_rad, "is_delta_rad/I");
    fvertex_tree->Branch("delta_true_pdg", &delta_true_pdg, "delta_true_pdg/I");
    fvertex_tree->Branch("delta_true_energy", &delta_true_energy, "delta_true_energy/D");
    fvertex_tree->Branch("delta_photon_energy", &delta_photon_energy, "delta_photon_energy/D");
    fvertex_tree->Branch("delta_proton_energy", &delta_proton_energy, "delta_proton_energy/D");
    fvertex_tree->Branch("delta_mcshower_true_pdg", &delta_mcshower_true_pdg, "delta_mcshower_true_pdg/I");
    fvertex_tree->Branch("delta_mcshower_true_energy", &delta_mcshower_true_energy, "delta_mcshower_true_energy/D");
    fvertex_tree->Branch("delta_mcshower_detprofile_energy", &delta_mcshower_detprofile_energy, "delta_mcshower_detprofile_energy/D");
    fvertex_tree->Branch("delta_mctrack_true_pdg", &delta_mctrack_true_pdg, "delta_mctrack_true_pdg/I");
    fvertex_tree->Branch("delta_mctrack_true_energy", &delta_mctrack_true_energy, "delta_mctrack_true_energy/D");
    fvertex_tree->Branch("delta_mctrack_true_length", &delta_mctrack_true_length, "delta_mctrack_true_length/D");    
    
    fvertex_tree->Branch("mctracknumber", &mctracknumber, "mctracknumber/I");
    fvertex_tree->Branch("mcshowernumber", &mcshowernumber, "mcshowernumber/I");

    fvertex_tree->Branch("true_nuvertx", &true_nuvertx, "true_nuvertx/D");
    fvertex_tree->Branch("true_nuverty", &true_nuverty, "true_nuverty/D");
    fvertex_tree->Branch("true_nuvertz", &true_nuvertz, "true_nuvertz/D");    

    fvertex_tree->Branch("true_nu_E", &true_nu_E, "true_nu_E/D");
    
    fvertex_tree->Branch("true_nu_vtx_tpc_contained", &true_nu_vtx_tpc_contained, "true_nu_vtx_tpc_contained/I"); 
    fvertex_tree->Branch("true_nu_vtx_fid_contained", &true_nu_vtx_fid_contained, "true_nu_vtx_fid_contained/I"); 

    fvertex_tree->Branch("longest_asso_track_matched_to_mcshower", &longest_asso_track_matched_to_mcshower, "longest_asso_track_matched_to_mcshower/I");
    fvertex_tree->Branch("longest_asso_track_matched_to_mctrack", &longest_asso_track_matched_to_mctrack, "longest_asso_track_matched_to_mctrack/I");
    fvertex_tree->Branch("longest_asso_track_matched_to_mcparticle", &longest_asso_track_matched_to_mcparticle, "longest_asso_track_matched_to_mcparticle/I");
    fvertex_tree->Branch("longest_asso_track_from_ncdeltarad", &longest_asso_track_from_ncdeltarad, "longest_asso_track_from_ncdeltarad/I");
    fvertex_tree->Branch("longest_asso_track_is_ncdeltarad_track", &longest_asso_track_is_ncdeltarad_track, "longest_asso_track_is_ncdeltarad_track/I");
    fvertex_tree->Branch("longest_asso_track_true_pdg", &longest_asso_track_true_pdg, "longest_asso_track_true_pdg/I");
    fvertex_tree->Branch("longest_asso_track_true_parent_pdg", &longest_asso_track_true_parent_pdg, "longest_asso_track_true_parent_pdg/I");
    fvertex_tree->Branch("longest_asso_track_true_ancestor_pdg", &longest_asso_track_true_ancestor_pdg, "longest_asso_track_true_ancestor_pdg/I");
    fvertex_tree->Branch("longest_asso_track_true_origin", &longest_asso_track_true_origin, "longest_asso_track_true_origin/I");
    fvertex_tree->Branch("longest_asso_track_true_startx", &longest_asso_track_true_startx, "longest_asso_track_true_startx/D"); 
    fvertex_tree->Branch("longest_asso_track_true_starty", &longest_asso_track_true_starty, "longest_asso_track_true_starty/D"); 
    fvertex_tree->Branch("longest_asso_track_true_startz", &longest_asso_track_true_startz, "longest_asso_track_true_startz/D"); 
    fvertex_tree->Branch("longest_asso_track_true_endx", &longest_asso_track_true_endx, "longest_asso_track_true_endx/D"); 
    fvertex_tree->Branch("longest_asso_track_true_endy", &longest_asso_track_true_endy, "longest_asso_track_true_endy/D"); 
    fvertex_tree->Branch("longest_asso_track_true_endz", &longest_asso_track_true_endz, "longest_asso_track_true_endz/D"); 
    fvertex_tree->Branch("longest_asso_track_true_thetayx", &longest_asso_track_true_thetayx, "longest_asso_track_true_thetayx/D");
    fvertex_tree->Branch("longest_asso_track_true_thetaxz", &longest_asso_track_true_thetaxz, "longest_asso_track_true_thetaxz/D");
    fvertex_tree->Branch("longest_asso_track_true_thetayz", &longest_asso_track_true_thetayz, "longest_asso_track_true_thetayz/D");
    fvertex_tree->Branch("longest_asso_track_true_energy", &longest_asso_track_true_energy, "longest_asso_track_true_energy/D");
    
    fvertex_tree->Branch("longest_asso_track_matched_to_mcshower", &longest_asso_track_matched_to_mcshower, "longest_asso_track_matched_to_mcshower/I");
    fvertex_tree->Branch("longest_asso_track_matched_to_mctrack", &longest_asso_track_matched_to_mctrack, "longest_asso_track_matched_to_mctrack/I");
    fvertex_tree->Branch("longest_asso_track_matched_to_mcparticle", &longest_asso_track_matched_to_mcparticle, "longest_asso_track_matched_to_mcparticle/I");
    fvertex_tree->Branch("longest_asso_track_true_pdg", &longest_asso_track_true_pdg, "longest_asso_track_true_pdg/I");
    fvertex_tree->Branch("longest_asso_track_true_parent_pdg", &longest_asso_track_true_parent_pdg, "longest_asso_track_true_parent_pdg/I");
    fvertex_tree->Branch("longest_asso_track_true_ancestor_pdg", &longest_asso_track_true_ancestor_pdg, "longest_asso_track_true_ancestor_pdg/I");
    fvertex_tree->Branch("longest_asso_track_true_origin", &longest_asso_track_true_origin, "longest_asso_track_true_origin/I");

    fvertex_tree->Branch("shower_matching_ratio", &shower_matching_ratio, "shower_matching_ratio/D");
    fvertex_tree->Branch("shower_matched_to_mcshower", &shower_matched_to_mcshower, "shower_matched_to_mcshower/I");
    fvertex_tree->Branch("shower_matched_to_mctrack", &shower_matched_to_mctrack, "shower_matched_to_mctrack/I");
    fvertex_tree->Branch("shower_matched_to_mcparticle", &shower_matched_to_mcparticle, "shower_matched_to_mcparticle/I");
    fvertex_tree->Branch("shower_from_ncdeltarad", &shower_from_ncdeltarad, "shower_from_ncdeltarad/I");
    fvertex_tree->Branch("shower_is_ncdeltarad_shower", &shower_is_ncdeltarad_shower, "shower_is_ncdeltarad_shower/I");
    fvertex_tree->Branch("shower_true_pdg", &shower_true_pdg, "shower_true_pdg/I");
    fvertex_tree->Branch("shower_true_parent_pdg", &shower_true_parent_pdg, "shower_true_parent_pdg/I");
    fvertex_tree->Branch("shower_true_ancestor_pdg", &shower_true_ancestor_pdg, "shower_true_ancestor_pdg/I");
    fvertex_tree->Branch("shower_true_origin", &shower_true_origin, "shower_true_origin/I");
    fvertex_tree->Branch("shower_true_startx", &shower_true_startx, "shower_true_startx/D"); 
    fvertex_tree->Branch("shower_true_starty", &shower_true_starty, "shower_true_starty/D"); 
    fvertex_tree->Branch("shower_true_startz", &shower_true_startz, "shower_true_startz/D"); 
    fvertex_tree->Branch("shower_true_endx", &shower_true_endx, "shower_true_endx/D"); 
    fvertex_tree->Branch("shower_true_endy", &shower_true_endy, "shower_true_endy/D"); 
    fvertex_tree->Branch("shower_true_endz", &shower_true_endz, "shower_true_endz/D"); 
    fvertex_tree->Branch("shower_true_dist", &shower_true_dist, "shower_true_dist/D");
    fvertex_tree->Branch("shower_true_distx", &shower_true_distx, "shower_true_distx/D"); 
    fvertex_tree->Branch("shower_true_disty", &shower_true_disty, "shower_true_disty/D"); 
    fvertex_tree->Branch("shower_true_distz", &shower_true_distz, "shower_true_distz/D"); 
    fvertex_tree->Branch("shower_true_thetayx", &shower_true_thetayx, "shower_true_thetayx/D");
    fvertex_tree->Branch("shower_true_thetaxz", &shower_true_thetaxz, "shower_true_thetaxz/D");
    fvertex_tree->Branch("shower_true_thetayz", &shower_true_thetayz, "shower_true_thetayz/D");
    fvertex_tree->Branch("shower_true_energy", &shower_true_energy, "shower_true_energy/D");
    fvertex_tree->Branch("shower_detprofile_thetayx", &shower_detprofile_thetayx, "shower_detprofile_thetayx/D");
    fvertex_tree->Branch("shower_detprofile_thetaxz", &shower_detprofile_thetaxz, "shower_detprofile_thetaxz/D");
    fvertex_tree->Branch("shower_detprofile_thetayz", &shower_detprofile_thetayz, "shower_detprofile_thetayz/D");
    fvertex_tree->Branch("shower_detprofile_energy", &shower_detprofile_energy, "shower_detprofile_energy/D");
    
    fevent_tree->Branch("fweight_genie_ncelaxial_p1sigma", &fweight_genie_ncelaxial_p1sigma, "fweight_genie_ncelaxial_p1sigma/D");
    fevent_tree->Branch("fweight_genie_ncelaxial_m1sigma", &fweight_genie_ncelaxial_m1sigma, "fweight_genie_ncelaxial_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_ncelaxial_p1sigma", &fweight_genie_ncelaxial_p1sigma, "fweight_genie_ncelaxial_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_ncelaxial_m1sigma", &fweight_genie_ncelaxial_m1sigma, "fweight_genie_ncelaxial_m1sigma/D");
    weight_branch_map.emplace("genie_NCELaxial_Genie", std::vector<double *>{&fweight_genie_ncelaxial_p1sigma, &fweight_genie_ncelaxial_m1sigma});
    
    fevent_tree->Branch("fweight_genie_nceleta_p1sigma", &fweight_genie_nceleta_p1sigma, "fweight_genie_nceleta_p1sigma/D");
    fevent_tree->Branch("fweight_genie_nceleta_m1sigma", &fweight_genie_nceleta_m1sigma, "fweight_genie_nceleta_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_nceleta_p1sigma", &fweight_genie_nceleta_p1sigma, "fweight_genie_nceleta_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_nceleta_m1sigma", &fweight_genie_nceleta_m1sigma, "fweight_genie_nceleta_m1sigma/D");
    weight_branch_map.emplace("genie_NCELeta_Genie", std::vector<double *>{&fweight_genie_nceleta_p1sigma, &fweight_genie_nceleta_m1sigma});
    
    fevent_tree->Branch("fweight_genie_qema_p1sigma", &fweight_genie_qema_p1sigma, "fweight_genie_qema_p1sigma/D");
    fevent_tree->Branch("fweight_genie_qema_m1sigma", &fweight_genie_qema_m1sigma, "fweight_genie_qema_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_qema_p1sigma", &fweight_genie_qema_p1sigma, "fweight_genie_qema_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_qema_m1sigma", &fweight_genie_qema_m1sigma, "fweight_genie_qema_m1sigma/D");
    weight_branch_map.emplace("genie_QEMA_Genie", std::vector<double *>{&fweight_genie_qema_p1sigma, &fweight_genie_qema_m1sigma});
    
    fevent_tree->Branch("fweight_genie_qevec_p1sigma", &fweight_genie_qevec_p1sigma, "fweight_genie_qevec_p1sigma/D");
    fevent_tree->Branch("fweight_genie_qevec_m1sigma", &fweight_genie_qevec_m1sigma, "fweight_genie_qevec_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_qevec_p1sigma", &fweight_genie_qevec_p1sigma, "fweight_genie_qevec_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_qevec_m1sigma", &fweight_genie_qevec_m1sigma, "fweight_genie_qevec_m1sigma/D");
    weight_branch_map.emplace("genie_QEVec_Genie", std::vector<double *>{&fweight_genie_qevec_p1sigma, &fweight_genie_qevec_m1sigma});

    fevent_tree->Branch("fweight_genie_ccresaxial_p1sigma", &fweight_genie_ccresaxial_p1sigma, "fweight_genie_ccresaxial_p1sigma/D");
    fevent_tree->Branch("fweight_genie_ccresaxial_m1sigma", &fweight_genie_ccresaxial_m1sigma, "fweight_genie_ccresaxial_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_ccresaxial_p1sigma", &fweight_genie_ccresaxial_p1sigma, "fweight_genie_ccresaxial_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_ccresaxial_m1sigma", &fweight_genie_ccresaxial_m1sigma, "fweight_genie_ccresaxial_m1sigma/D");
    weight_branch_map.emplace("genie_CCResAxial_Genie", std::vector<double *>{&fweight_genie_ccresaxial_p1sigma, &fweight_genie_ccresaxial_m1sigma});
    
    fevent_tree->Branch("fweight_genie_ccresvector_p1sigma", &fweight_genie_ccresvector_p1sigma, "fweight_genie_ccresvector_p1sigma/D");
    fevent_tree->Branch("fweight_genie_ccresvector_m1sigma", &fweight_genie_ccresvector_m1sigma, "fweight_genie_ccresvector_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_ccresvector_p1sigma", &fweight_genie_ccresvector_p1sigma, "fweight_genie_ccresvector_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_ccresvector_m1sigma", &fweight_genie_ccresvector_m1sigma, "fweight_genie_ccresvector_m1sigma/D");
    weight_branch_map.emplace("genie_CCResVector_Genie", std::vector<double *>{&fweight_genie_ccresvector_p1sigma, &fweight_genie_ccresvector_m1sigma});
    
    fevent_tree->Branch("fweight_genie_resganged_p1sigma", &fweight_genie_resganged_p1sigma, "fweight_genie_resganged_p1sigma/D");
    fevent_tree->Branch("fweight_genie_resganged_m1sigma", &fweight_genie_resganged_m1sigma, "fweight_genie_resganged_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_resganged_p1sigma", &fweight_genie_resganged_p1sigma, "fweight_genie_resganged_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_resganged_m1sigma", &fweight_genie_resganged_m1sigma, "fweight_genie_resganged_m1sigma/D");
    weight_branch_map.emplace("genie_ResGanged_Genie", std::vector<double *>{&fweight_genie_resganged_p1sigma, &fweight_genie_resganged_m1sigma});
    
    fevent_tree->Branch("fweight_genie_ncresaxial_p1sigma", &fweight_genie_ncresaxial_p1sigma, "fweight_genie_ncresaxial_p1sigma/D");
    fevent_tree->Branch("fweight_genie_ncresaxial_m1sigma", &fweight_genie_ncresaxial_m1sigma, "fweight_genie_ncresaxial_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_ncresaxial_p1sigma", &fweight_genie_ncresaxial_p1sigma, "fweight_genie_ncresaxial_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_ncresaxial_m1sigma", &fweight_genie_ncresaxial_m1sigma, "fweight_genie_ncresaxial_m1sigma/D");
    weight_branch_map.emplace("genie_NCResAxial_Genie", std::vector<double *>{&fweight_genie_ncresaxial_p1sigma, &fweight_genie_ncresaxial_m1sigma});
    
    fevent_tree->Branch("fweight_genie_ncresvector_p1sigma", &fweight_genie_ncresvector_p1sigma, "fweight_genie_ncresvector_p1sigma/D");
    fevent_tree->Branch("fweight_genie_ncresvector_m1sigma", &fweight_genie_ncresvector_m1sigma, "fweight_genie_ncresvector_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_ncresvector_p1sigma", &fweight_genie_ncresvector_p1sigma, "fweight_genie_ncresvector_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_ncresvector_m1sigma", &fweight_genie_ncresvector_m1sigma, "fweight_genie_ncresvector_m1sigma/D");
    weight_branch_map.emplace("genie_NCResVector_Genie", std::vector<double *>{&fweight_genie_ncresvector_p1sigma, &fweight_genie_ncresvector_m1sigma});
    
    fevent_tree->Branch("fweight_genie_cohma_p1sigma", &fweight_genie_cohma_p1sigma, "fweight_genie_cohma_p1sigma/D");
    fevent_tree->Branch("fweight_genie_cohma_m1sigma", &fweight_genie_cohma_m1sigma, "fweight_genie_cohma_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_cohma_p1sigma", &fweight_genie_cohma_p1sigma, "fweight_genie_cohma_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_cohma_m1sigma", &fweight_genie_cohma_m1sigma, "fweight_genie_cohma_m1sigma/D");
    weight_branch_map.emplace("genie_CohMA_Genie", std::vector<double *>{&fweight_genie_cohma_p1sigma, &fweight_genie_cohma_m1sigma});
    
    fevent_tree->Branch("fweight_genie_cohr0_p1sigma", &fweight_genie_cohr0_p1sigma, "fweight_genie_cohr0_p1sigma/D");
    fevent_tree->Branch("fweight_genie_cohr0_m1sigma", &fweight_genie_cohr0_m1sigma, "fweight_genie_cohr0_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_cohr0_p1sigma", &fweight_genie_cohr0_p1sigma, "fweight_genie_cohr0_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_cohr0_m1sigma", &fweight_genie_cohr0_m1sigma, "fweight_genie_cohr0_m1sigma/D");
    weight_branch_map.emplace("genie_CohR0_Genie", std::vector<double *>{&fweight_genie_cohr0_p1sigma, &fweight_genie_cohr0_m1sigma});
    
    fevent_tree->Branch("fweight_genie_nonresrvp1pi_p1sigma", &fweight_genie_nonresrvp1pi_p1sigma, "fweight_genie_nonresrvp1pi_p1sigma/D");
    fevent_tree->Branch("fweight_genie_nonresrvp1pi_m1sigma", &fweight_genie_nonresrvp1pi_m1sigma, "fweight_genie_nonresrvp1pi_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_nonresrvp1pi_p1sigma", &fweight_genie_nonresrvp1pi_p1sigma, "fweight_genie_nonresrvp1pi_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_nonresrvp1pi_m1sigma", &fweight_genie_nonresrvp1pi_m1sigma, "fweight_genie_nonresrvp1pi_m1sigma/D");
    weight_branch_map.emplace("genie_NonResRvp1pi_Genie", std::vector<double *>{&fweight_genie_nonresrvp1pi_p1sigma, &fweight_genie_nonresrvp1pi_m1sigma});
    
    fevent_tree->Branch("fweight_genie_nonresrvbarp1pi_p1sigma", &fweight_genie_nonresrvbarp1pi_p1sigma, "fweight_genie_nonresrvbarp1pi_p1sigma/D");
    fevent_tree->Branch("fweight_genie_nonresrvbarp1pi_m1sigma", &fweight_genie_nonresrvbarp1pi_m1sigma, "fweight_genie_nonresrvbarp1pi_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_nonresrvbarp1pi_p1sigma", &fweight_genie_nonresrvbarp1pi_p1sigma, "fweight_genie_nonresrvbarp1pi_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_nonresrvbarp1pi_m1sigma", &fweight_genie_nonresrvbarp1pi_m1sigma, "fweight_genie_nonresrvbarp1pi_m1sigma/D");
    weight_branch_map.emplace("genie_NonResRvbarp1pi_Genie", std::vector<double *>{&fweight_genie_nonresrvbarp1pi_p1sigma, &fweight_genie_nonresrvbarp1pi_m1sigma});
    
    fevent_tree->Branch("fweight_genie_nonresrvp2pi_p1sigma", &fweight_genie_nonresrvp2pi_p1sigma, "fweight_genie_nonresrvp2pi_p1sigma/D");
    fevent_tree->Branch("fweight_genie_nonresrvp2pi_m1sigma", &fweight_genie_nonresrvp2pi_m1sigma, "fweight_genie_nonresrvp2pi_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_nonresrvp2pi_p1sigma", &fweight_genie_nonresrvp2pi_p1sigma, "fweight_genie_nonresrvp2pi_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_nonresrvp2pi_m1sigma", &fweight_genie_nonresrvp2pi_m1sigma, "fweight_genie_nonresrvp2pi_m1sigma/D");
    weight_branch_map.emplace("genie_NonResRvp2pi_Genie", std::vector<double *>{&fweight_genie_nonresrvp2pi_p1sigma, &fweight_genie_nonresrvp2pi_m1sigma});
    
    fevent_tree->Branch("fweight_genie_nonresrvbarp2pi_p1sigma", &fweight_genie_nonresrvbarp2pi_p1sigma, "fweight_genie_nonresrvbarp2pi_p1sigma/D");
    fevent_tree->Branch("fweight_genie_nonresrvbarp2pi_m1sigma", &fweight_genie_nonresrvbarp2pi_m1sigma, "fweight_genie_nonresrvbarp2pi_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_nonresrvbarp2pi_p1sigma", &fweight_genie_nonresrvbarp2pi_p1sigma, "fweight_genie_nonresrvbarp2pi_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_nonresrvbarp2pi_m1sigma", &fweight_genie_nonresrvbarp2pi_m1sigma, "fweight_genie_nonresrvbarp2pi_m1sigma/D");
    weight_branch_map.emplace("genie_NonResRvbarp2pi_Genie", std::vector<double *>{&fweight_genie_nonresrvbarp2pi_p1sigma, &fweight_genie_nonresrvbarp2pi_m1sigma});
    
    fevent_tree->Branch("fweight_genie_resdecaygamma_p1sigma", &fweight_genie_resdecaygamma_p1sigma, "fweight_genie_resdecaygamma_p1sigma/D");
    fevent_tree->Branch("fweight_genie_resdecaygamma_m1sigma", &fweight_genie_resdecaygamma_m1sigma, "fweight_genie_resdecaygamma_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_resdecaygamma_p1sigma", &fweight_genie_resdecaygamma_p1sigma, "fweight_genie_resdecaygamma_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_resdecaygamma_m1sigma", &fweight_genie_resdecaygamma_m1sigma, "fweight_genie_resdecaygamma_m1sigma/D");
    weight_branch_map.emplace("genie_ResDecayGamma_Genie", std::vector<double *>{&fweight_genie_resdecaygamma_p1sigma, &fweight_genie_resdecaygamma_m1sigma});
    
    fevent_tree->Branch("fweight_genie_resdecayeta_p1sigma", &fweight_genie_resdecayeta_p1sigma, "fweight_genie_resdecayeta_p1sigma/D");
    fevent_tree->Branch("fweight_genie_resdecayeta_m1sigma", &fweight_genie_resdecayeta_m1sigma, "fweight_genie_resdecayeta_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_resdecayeta_p1sigma", &fweight_genie_resdecayeta_p1sigma, "fweight_genie_resdecayeta_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_resdecayeta_m1sigma", &fweight_genie_resdecayeta_m1sigma, "fweight_genie_resdecayeta_m1sigma/D");
    weight_branch_map.emplace("genie_ResDecayEta_Genie", std::vector<double *>{&fweight_genie_resdecayeta_p1sigma, &fweight_genie_resdecayeta_m1sigma});
    
    fevent_tree->Branch("fweight_genie_resdecaytheta_p1sigma", &fweight_genie_resdecaytheta_p1sigma, "fweight_genie_resdecaytheta_p1sigma/D");
    fevent_tree->Branch("fweight_genie_resdecaytheta_m1sigma", &fweight_genie_resdecaytheta_m1sigma, "fweight_genie_resdecaytheta_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_resdecaytheta_p1sigma", &fweight_genie_resdecaytheta_p1sigma, "fweight_genie_resdecaytheta_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_resdecaytheta_m1sigma", &fweight_genie_resdecaytheta_m1sigma, "fweight_genie_resdecaytheta_m1sigma/D");
    weight_branch_map.emplace("genie_ResDecayTheta_Genie", std::vector<double *>{&fweight_genie_resdecaytheta_p1sigma, &fweight_genie_resdecaytheta_m1sigma});
    
    fevent_tree->Branch("fweight_genie_nc_p1sigma", &fweight_genie_nc_p1sigma, "fweight_genie_nc_p1sigma/D");
    fevent_tree->Branch("fweight_genie_nc_m1sigma", &fweight_genie_nc_m1sigma, "fweight_genie_nc_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_nc_p1sigma", &fweight_genie_nc_p1sigma, "fweight_genie_nc_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_nc_m1sigma", &fweight_genie_nc_m1sigma, "fweight_genie_nc_m1sigma/D");
    weight_branch_map.emplace("genie_NC_Genie", std::vector<double *>{&fweight_genie_nc_p1sigma, &fweight_genie_nc_m1sigma});
    
    fevent_tree->Branch("fweight_genie_disath_p1sigma", &fweight_genie_disath_p1sigma, "fweight_genie_disath_p1sigma/D");
    fevent_tree->Branch("fweight_genie_disath_m1sigma", &fweight_genie_disath_m1sigma, "fweight_genie_disath_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_disath_p1sigma", &fweight_genie_disath_p1sigma, "fweight_genie_disath_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_disath_m1sigma", &fweight_genie_disath_m1sigma, "fweight_genie_disath_m1sigma/D");
    weight_branch_map.emplace("genie_DISAth_Genie", std::vector<double *>{&fweight_genie_disath_p1sigma, &fweight_genie_disath_m1sigma});
    
    fevent_tree->Branch("fweight_genie_disbth_p1sigma", &fweight_genie_disbth_p1sigma, "fweight_genie_disbth_p1sigma/D");
    fevent_tree->Branch("fweight_genie_disbth_m1sigma", &fweight_genie_disbth_m1sigma, "fweight_genie_disbth_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_disbth_p1sigma", &fweight_genie_disbth_p1sigma, "fweight_genie_disbth_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_disbth_m1sigma", &fweight_genie_disbth_m1sigma, "fweight_genie_disbth_m1sigma/D");
    weight_branch_map.emplace("genie_DISBth_Genie", std::vector<double *>{&fweight_genie_disbth_p1sigma, &fweight_genie_disbth_m1sigma});
    
    fevent_tree->Branch("fweight_genie_discv1u_p1sigma", &fweight_genie_discv1u_p1sigma, "fweight_genie_discv1u_p1sigma/D");
    fevent_tree->Branch("fweight_genie_discv1u_m1sigma", &fweight_genie_discv1u_m1sigma, "fweight_genie_discv1u_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_discv1u_p1sigma", &fweight_genie_discv1u_p1sigma, "fweight_genie_discv1u_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_discv1u_m1sigma", &fweight_genie_discv1u_m1sigma, "fweight_genie_discv1u_m1sigma/D");
    weight_branch_map.emplace("genie_DISCv1u_Genie", std::vector<double *>{&fweight_genie_discv1u_p1sigma, &fweight_genie_discv1u_m1sigma});
    
    fevent_tree->Branch("fweight_genie_discv2u_p1sigma", &fweight_genie_discv2u_p1sigma, "fweight_genie_discv2u_p1sigma/D");
    fevent_tree->Branch("fweight_genie_discv2u_m1sigma", &fweight_genie_discv2u_m1sigma, "fweight_genie_discv2u_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_discv2u_p1sigma", &fweight_genie_discv2u_p1sigma, "fweight_genie_discv2u_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_discv2u_m1sigma", &fweight_genie_discv2u_m1sigma, "fweight_genie_discv2u_m1sigma/D");
    weight_branch_map.emplace("genie_DISCv2u_Genie", std::vector<double *>{&fweight_genie_discv2u_p1sigma, &fweight_genie_discv2u_m1sigma});
  
    fevent_tree->Branch("fweight_genie_disnucl_p1sigma", &fweight_genie_disnucl_p1sigma, "fweight_genie_disnucl_p1sigma/D");
    fevent_tree->Branch("fweight_genie_disnucl_m1sigma", &fweight_genie_disnucl_m1sigma, "fweight_genie_disnucl_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_disnucl_p1sigma", &fweight_genie_disnucl_p1sigma, "fweight_genie_disnucl_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_disnucl_m1sigma", &fweight_genie_disnucl_m1sigma, "fweight_genie_disnucl_m1sigma/D");
    weight_branch_map.emplace("genie_DISnucl_Genie", std::vector<double *>{&fweight_genie_disnucl_p1sigma, &fweight_genie_disnucl_m1sigma});
    
    fevent_tree->Branch("fweight_genie_agkyxf_p1sigma", &fweight_genie_agkyxf_p1sigma, "fweight_genie_agkyxf_p1sigma/D");
    fevent_tree->Branch("fweight_genie_agkyxf_m1sigma", &fweight_genie_agkyxf_m1sigma, "fweight_genie_agkyxf_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_agkyxf_p1sigma", &fweight_genie_agkyxf_p1sigma, "fweight_genie_agkyxf_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_agkyxf_m1sigma", &fweight_genie_agkyxf_m1sigma, "fweight_genie_agkyxf_m1sigma/D");
    weight_branch_map.emplace("genie_AGKYxF_Genie", std::vector<double *>{&fweight_genie_agkyxf_p1sigma, &fweight_genie_agkyxf_m1sigma});
    
    fevent_tree->Branch("fweight_genie_agkypt_p1sigma", &fweight_genie_agkypt_p1sigma, "fweight_genie_agkypt_p1sigma/D");
    fevent_tree->Branch("fweight_genie_agkypt_m1sigma", &fweight_genie_agkypt_m1sigma, "fweight_genie_agkypt_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_agkypt_p1sigma", &fweight_genie_agkypt_p1sigma, "fweight_genie_agkypt_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_agkypt_m1sigma", &fweight_genie_agkypt_m1sigma, "fweight_genie_agkypt_m1sigma/D");
    weight_branch_map.emplace("genie_AGKYpT_Genie", std::vector<double *>{&fweight_genie_agkypt_p1sigma, &fweight_genie_agkypt_m1sigma});
    
    fevent_tree->Branch("fweight_genie_formzone_p1sigma", &fweight_genie_formzone_p1sigma, "fweight_genie_formzone_p1sigma/D");
    fevent_tree->Branch("fweight_genie_formzone_m1sigma", &fweight_genie_formzone_m1sigma, "fweight_genie_formzone_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_formzone_p1sigma", &fweight_genie_formzone_p1sigma, "fweight_genie_formzone_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_formzone_m1sigma", &fweight_genie_formzone_m1sigma, "fweight_genie_formzone_m1sigma/D");
    weight_branch_map.emplace("genie_FormZone_Genie", std::vector<double *>{&fweight_genie_formzone_p1sigma, &fweight_genie_formzone_m1sigma});
    
    fevent_tree->Branch("fweight_genie_fermigasmodelkf_p1sigma", &fweight_genie_fermigasmodelkf_p1sigma, "fweight_genie_fermigasmodelkf_p1sigma/D");
    fevent_tree->Branch("fweight_genie_fermigasmodelkf_m1sigma", &fweight_genie_fermigasmodelkf_m1sigma, "fweight_genie_fermigasmodelkf_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_fermigasmodelkf_p1sigma", &fweight_genie_fermigasmodelkf_p1sigma, "fweight_genie_fermigasmodelkf_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_fermigasmodelkf_m1sigma", &fweight_genie_fermigasmodelkf_m1sigma, "fweight_genie_fermigasmodelkf_m1sigma/D");
    weight_branch_map.emplace("genie_FermiGasModelKf_Genie", std::vector<double *>{&fweight_genie_fermigasmodelkf_p1sigma, &fweight_genie_fermigasmodelkf_m1sigma});
    
    fevent_tree->Branch("fweight_genie_fermigasmodelsf_p1sigma", &fweight_genie_fermigasmodelsf_p1sigma, "fweight_genie_fermigasmodelsf_p1sigma/D");
    fevent_tree->Branch("fweight_genie_fermigasmodelsf_m1sigma", &fweight_genie_fermigasmodelsf_m1sigma, "fweight_genie_fermigasmodelsf_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_fermigasmodelsf_p1sigma", &fweight_genie_fermigasmodelsf_p1sigma, "fweight_genie_fermigasmodelsf_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_fermigasmodelsf_m1sigma", &fweight_genie_fermigasmodelsf_m1sigma, "fweight_genie_fermigasmodelsf_m1sigma/D");
    weight_branch_map.emplace("genie_FermiGasModelSf_Genie", std::vector<double *>{&fweight_genie_fermigasmodelsf_p1sigma, &fweight_genie_fermigasmodelsf_m1sigma});
    
    fevent_tree->Branch("fweight_genie_intranukenmfp_p1sigma", &fweight_genie_intranukenmfp_p1sigma, "fweight_genie_intranukenmfp_p1sigma/D");
    fevent_tree->Branch("fweight_genie_intranukenmfp_m1sigma", &fweight_genie_intranukenmfp_m1sigma, "fweight_genie_intranukenmfp_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukenmfp_p1sigma", &fweight_genie_intranukenmfp_p1sigma, "fweight_genie_intranukenmfp_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukenmfp_m1sigma", &fweight_genie_intranukenmfp_m1sigma, "fweight_genie_intranukenmfp_m1sigma/D");
    weight_branch_map.emplace("genie_IntraNukeNmfp_Genie", std::vector<double *>{&fweight_genie_intranukenmfp_p1sigma, &fweight_genie_intranukenmfp_m1sigma});
    
    fevent_tree->Branch("fweight_genie_intranukencex_p1sigma", &fweight_genie_intranukencex_p1sigma, "fweight_genie_intranukencex_p1sigma/D");
    fevent_tree->Branch("fweight_genie_intranukencex_m1sigma", &fweight_genie_intranukencex_m1sigma, "fweight_genie_intranukencex_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukencex_p1sigma", &fweight_genie_intranukencex_p1sigma, "fweight_genie_intranukencex_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukencex_m1sigma", &fweight_genie_intranukencex_m1sigma, "fweight_genie_intranukencex_m1sigma/D");
    weight_branch_map.emplace("genie_IntraNukeNcex_Genie", std::vector<double *>{&fweight_genie_intranukencex_p1sigma, &fweight_genie_intranukencex_m1sigma});
    
    fevent_tree->Branch("fweight_genie_intranukenel_p1sigma", &fweight_genie_intranukenel_p1sigma, "fweight_genie_intranukenel_p1sigma/D");
    fevent_tree->Branch("fweight_genie_intranukenel_m1sigma", &fweight_genie_intranukenel_m1sigma, "fweight_genie_intranukenel_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukenel_p1sigma", &fweight_genie_intranukenel_p1sigma, "fweight_genie_intranukenel_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukenel_m1sigma", &fweight_genie_intranukenel_m1sigma, "fweight_genie_intranukenel_m1sigma/D");
    weight_branch_map.emplace("genie_IntraNukeNel_Genie", std::vector<double *>{&fweight_genie_intranukenel_p1sigma, &fweight_genie_intranukenel_m1sigma});
    
    fevent_tree->Branch("fweight_genie_intranukeninel_p1sigma", &fweight_genie_intranukeninel_p1sigma, "fweight_genie_intranukeninel_p1sigma/D");
    fevent_tree->Branch("fweight_genie_intranukeninel_m1sigma", &fweight_genie_intranukeninel_m1sigma, "fweight_genie_intranukeninel_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukeninel_p1sigma", &fweight_genie_intranukeninel_p1sigma, "fweight_genie_intranukeninel_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukeninel_m1sigma", &fweight_genie_intranukeninel_m1sigma, "fweight_genie_intranukeninel_m1sigma/D");
    weight_branch_map.emplace("genie_IntraNukeNinel_Genie", std::vector<double *>{&fweight_genie_intranukeninel_p1sigma, &fweight_genie_intranukeninel_m1sigma});
    
    fevent_tree->Branch("fweight_genie_intranukenabs_p1sigma", &fweight_genie_intranukenabs_p1sigma, "fweight_genie_intranukenabs_p1sigma/D");
    fevent_tree->Branch("fweight_genie_intranukenabs_m1sigma", &fweight_genie_intranukenabs_m1sigma, "fweight_genie_intranukenabs_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukenabs_p1sigma", &fweight_genie_intranukenabs_p1sigma, "fweight_genie_intranukenabs_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukenabs_m1sigma", &fweight_genie_intranukenabs_m1sigma, "fweight_genie_intranukenabs_m1sigma/D");
    weight_branch_map.emplace("genie_IntraNukeNabs_Genie", std::vector<double *>{&fweight_genie_intranukenabs_p1sigma, &fweight_genie_intranukenabs_m1sigma});
    
    fevent_tree->Branch("fweight_genie_intranukenpi_p1sigma", &fweight_genie_intranukenpi_p1sigma, "fweight_genie_intranukenpi_p1sigma/D");
    fevent_tree->Branch("fweight_genie_intranukenpi_m1sigma", &fweight_genie_intranukenpi_m1sigma, "fweight_genie_intranukenpi_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukenpi_p1sigma", &fweight_genie_intranukenpi_p1sigma, "fweight_genie_intranukenpi_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukenpi_m1sigma", &fweight_genie_intranukenpi_m1sigma, "fweight_genie_intranukenpi_m1sigma/D");
    weight_branch_map.emplace("genie_IntraNukeNpi_Genie", std::vector<double *>{&fweight_genie_intranukenpi_p1sigma, &fweight_genie_intranukenpi_m1sigma});
    
    fevent_tree->Branch("fweight_genie_intranukepimfp_p1sigma", &fweight_genie_intranukepimfp_p1sigma, "fweight_genie_intranukepimfp_p1sigma/D");
    fevent_tree->Branch("fweight_genie_intranukepimfp_m1sigma", &fweight_genie_intranukepimfp_m1sigma, "fweight_genie_intranukepimfp_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukepimfp_p1sigma", &fweight_genie_intranukepimfp_p1sigma, "fweight_genie_intranukepimfp_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukepimfp_m1sigma", &fweight_genie_intranukepimfp_m1sigma, "fweight_genie_intranukepimfp_m1sigma/D");
    weight_branch_map.emplace("genie_IntraNukePImfp_Genie", std::vector<double *>{&fweight_genie_intranukepimfp_p1sigma, &fweight_genie_intranukepimfp_m1sigma});
    
    fevent_tree->Branch("fweight_genie_intranukepicex_p1sigma", &fweight_genie_intranukepicex_p1sigma, "fweight_genie_intranukepicex_p1sigma/D");
    fevent_tree->Branch("fweight_genie_intranukepicex_m1sigma", &fweight_genie_intranukepicex_m1sigma, "fweight_genie_intranukepicex_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukepicex_p1sigma", &fweight_genie_intranukepicex_p1sigma, "fweight_genie_intranukepicex_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukepicex_m1sigma", &fweight_genie_intranukepicex_m1sigma, "fweight_genie_intranukepicex_m1sigma/D");
    weight_branch_map.emplace("genie_IntraNukePIcex_Genie", std::vector<double *>{&fweight_genie_intranukepicex_p1sigma, &fweight_genie_intranukepicex_m1sigma});
    
    fevent_tree->Branch("fweight_genie_intranukepiel_p1sigma", &fweight_genie_intranukepiel_p1sigma, "fweight_genie_intranukepiel_p1sigma/D");
    fevent_tree->Branch("fweight_genie_intranukepiel_m1sigma", &fweight_genie_intranukepiel_m1sigma, "fweight_genie_intranukepiel_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukepiel_p1sigma", &fweight_genie_intranukepiel_p1sigma, "fweight_genie_intranukepiel_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukepiel_m1sigma", &fweight_genie_intranukepiel_m1sigma, "fweight_genie_intranukepiel_m1sigma/D");
    weight_branch_map.emplace("genie_IntraNukePIel_Genie", std::vector<double *>{&fweight_genie_intranukepiel_p1sigma, &fweight_genie_intranukepiel_m1sigma});
    
    fevent_tree->Branch("fweight_genie_intranukepiinel_p1sigma", &fweight_genie_intranukepiinel_p1sigma, "fweight_genie_intranukepiinel_p1sigma/D");
    fevent_tree->Branch("fweight_genie_intranukepiinel_m1sigma", &fweight_genie_intranukepiinel_m1sigma, "fweight_genie_intranukepiinel_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukepiinel_p1sigma", &fweight_genie_intranukepiinel_p1sigma, "fweight_genie_intranukepiinel_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukepiinel_m1sigma", &fweight_genie_intranukepiinel_m1sigma, "fweight_genie_intranukepiinel_m1sigma/D");
    weight_branch_map.emplace("genie_IntraNukePIinel_Genie", std::vector<double *>{&fweight_genie_intranukepiinel_p1sigma, &fweight_genie_intranukepiinel_m1sigma});
    
    fevent_tree->Branch("fweight_genie_intranukepiabs_p1sigma", &fweight_genie_intranukepiabs_p1sigma, "fweight_genie_intranukepiabs_p1sigma/D");
    fevent_tree->Branch("fweight_genie_intranukepiabs_m1sigma", &fweight_genie_intranukepiabs_m1sigma, "fweight_genie_intranukepiabs_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukepiabs_p1sigma", &fweight_genie_intranukepiabs_p1sigma, "fweight_genie_intranukepiabs_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukepiabs_m1sigma", &fweight_genie_intranukepiabs_m1sigma, "fweight_genie_intranukepiabs_m1sigma/D");
    weight_branch_map.emplace("genie_IntraNukePIabs_Genie", std::vector<double *>{&fweight_genie_intranukepiabs_p1sigma, &fweight_genie_intranukepiabs_m1sigma});
    
    fevent_tree->Branch("fweight_genie_intranukepipi_p1sigma", &fweight_genie_intranukepipi_p1sigma, "fweight_genie_intranukepipi_p1sigma/D");
    fevent_tree->Branch("fweight_genie_intranukepipi_m1sigma", &fweight_genie_intranukepipi_m1sigma, "fweight_genie_intranukepipi_m1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukepipi_p1sigma", &fweight_genie_intranukepipi_p1sigma, "fweight_genie_intranukepipi_p1sigma/D");
    fvertex_tree->Branch("fweight_genie_intranukepipi_m1sigma", &fweight_genie_intranukepipi_m1sigma, "fweight_genie_intranukepipi_m1sigma/D");
    weight_branch_map.emplace("genie_IntraNukePIpi_Genie", std::vector<double *>{&fweight_genie_intranukepipi_p1sigma, &fweight_genie_intranukepipi_m1sigma});
  }    

}


void FillTreeVariables::ResetEvent() {

  run_number = -1;
  subrun_number = -1;
  event_number = -1;
  nu_pdg = 0;
  nu_energy = -1;
  lep_pdg = -1;
  lep_energy = 0;
  ccnc = -1;
  mode = -1;
  interaction_type = -1;

  exiting_photon_number = 0;
  exiting_proton_number = 0;
  exiting_neutron_number = 0;
  exiting_electron_number = 0;
  exiting_antielectron_number = 0;
  exiting_muon_number = 0;
  exiting_antimuon_number = 0;
  exiting_piplus_number = 0;
  exiting_piminus_number = 0;
  exiting_pi0_number = 0;
  total_exiting_particles = 0;
  exiting_particle_vector.clear();
  
  is_single_photon = -1;
  is_delta_rad = -1;
  delta_true_pdg = 0;
  delta_true_energy = -1;
  delta_photon_energy = -1;
  delta_proton_energy = -1;
  delta_mcshower_true_pdg = 0;
  delta_mcshower_true_energy = -1;
  delta_mcshower_detprofile_energy = -1;
  delta_mctrack_true_pdg = 0;
  delta_mctrack_true_energy = -1;
  delta_mctrack_true_length = -1;  

  mctracknumber = -1;
  mcshowernumber = -1;
  tracknumber = -1;
  showernumber = -1;

  external_singlephoton_protons_neutrons_only = -1;

  passed_swtrigger = -1;
  totalpe_sum = -1;
  totalpe_ibg_sum = -1;
  totalpe_bbg_sum = -1;

  most_energetic_reco_shower = -1;
  second_most_energetic_reco_shower = -1;

  true_nuvertx = -1;
  true_nuverty = -120;
  true_nuvertz = -1;

  true_nu_E = -1;

  true_nu_vtx_tpc_contained = -1;
  true_nu_vtx_fid_contained = -1;

  selected = -1;
  number_of_selected_vertices = -1;
  number_of_selected_vertices_cut = -1;

  fweight_genie_ncelaxial_p1sigma = -1;
  fweight_genie_ncelaxial_m1sigma = -1;
  fweight_genie_nceleta_p1sigma = -1;
  fweight_genie_nceleta_m1sigma = -1;
  fweight_genie_qema_p1sigma = -1;
  fweight_genie_qema_m1sigma = -1;
  fweight_genie_qevec_p1sigma = -1;
  fweight_genie_qevec_m1sigma = -1;
  fweight_genie_ccresaxial_p1sigma = -1;
  fweight_genie_ccresaxial_m1sigma = -1;
  fweight_genie_ccresvector_p1sigma = -1;
  fweight_genie_ccresvector_m1sigma = -1;
  fweight_genie_resganged_p1sigma = -1;
  fweight_genie_resganged_m1sigma = -1;
  fweight_genie_ncresaxial_p1sigma = -1;
  fweight_genie_ncresaxial_m1sigma = -1;
  fweight_genie_ncresvector_p1sigma = -1;
  fweight_genie_ncresvector_m1sigma = -1;
  fweight_genie_cohma_p1sigma = -1;
  fweight_genie_cohma_m1sigma = -1;
  fweight_genie_cohr0_p1sigma = -1;
  fweight_genie_cohr0_m1sigma = -1;
  fweight_genie_nonresrvp1pi_p1sigma = -1;
  fweight_genie_nonresrvp1pi_m1sigma = -1;
  fweight_genie_nonresrvbarp1pi_p1sigma = -1;
  fweight_genie_nonresrvbarp1pi_m1sigma = -1;
  fweight_genie_nonresrvp2pi_p1sigma = -1;
  fweight_genie_nonresrvp2pi_m1sigma = -1;
  fweight_genie_nonresrvbarp2pi_p1sigma = -1;
  fweight_genie_nonresrvbarp2pi_m1sigma = -1;
  fweight_genie_resdecaygamma_p1sigma = -1;
  fweight_genie_resdecaygamma_m1sigma = -1;
  fweight_genie_resdecayeta_p1sigma = -1;
  fweight_genie_resdecayeta_m1sigma = -1;
  fweight_genie_resdecaytheta_p1sigma = -1;
  fweight_genie_resdecaytheta_m1sigma = -1;
  fweight_genie_nc_p1sigma = -1;
  fweight_genie_nc_m1sigma = -1;
  fweight_genie_disath_p1sigma = -1;
  fweight_genie_disath_m1sigma = -1;
  fweight_genie_disbth_p1sigma = -1;
  fweight_genie_disbth_m1sigma = -1;
  fweight_genie_discv1u_p1sigma = -1;
  fweight_genie_discv1u_m1sigma = -1;
  fweight_genie_discv2u_p1sigma = -1;
  fweight_genie_discv2u_m1sigma = -1;
  fweight_genie_disnucl_p1sigma = -1;
  fweight_genie_disnucl_m1sigma = -1;
  fweight_genie_agkyxf_p1sigma = -1;
  fweight_genie_agkyxf_m1sigma = -1;
  fweight_genie_agkypt_p1sigma = -1;
  fweight_genie_agkypt_m1sigma = -1;
  fweight_genie_formzone_p1sigma = -1;
  fweight_genie_formzone_m1sigma = -1;
  fweight_genie_fermigasmodelkf_p1sigma = -1;
  fweight_genie_fermigasmodelkf_m1sigma = -1;
  fweight_genie_fermigasmodelsf_p1sigma = -1;
  fweight_genie_fermigasmodelsf_m1sigma = -1;
  fweight_genie_intranukenmfp_p1sigma = -1;
  fweight_genie_intranukenmfp_m1sigma = -1;
  fweight_genie_intranukencex_p1sigma = -1;
  fweight_genie_intranukencex_m1sigma = -1;
  fweight_genie_intranukenel_p1sigma = -1;
  fweight_genie_intranukenel_m1sigma = -1;
  fweight_genie_intranukeninel_p1sigma = -1;
  fweight_genie_intranukeninel_m1sigma = -1;
  fweight_genie_intranukenabs_p1sigma = -1;
  fweight_genie_intranukenabs_m1sigma = -1;
  fweight_genie_intranukenpi_p1sigma = -1;
  fweight_genie_intranukenpi_m1sigma = -1;
  fweight_genie_intranukepimfp_p1sigma = -1;
  fweight_genie_intranukepimfp_m1sigma = -1;
  fweight_genie_intranukepicex_p1sigma = -1;
  fweight_genie_intranukepicex_m1sigma = -1;
  fweight_genie_intranukepiel_p1sigma = -1;
  fweight_genie_intranukepiel_m1sigma = -1;
  fweight_genie_intranukepiinel_p1sigma = -1;
  fweight_genie_intranukepiinel_m1sigma = -1;
  fweight_genie_intranukepiabs_p1sigma = -1;
  fweight_genie_intranukepiabs_m1sigma = -1;
  fweight_genie_intranukepipi_p1sigma = -1;
  fweight_genie_intranukepipi_m1sigma = -1;

}


void FillTreeVariables::ResetVertex() {

  reco_nuvertx = -1;
  reco_nuverty = -120;
  reco_nuvertz = -1;
  
  reco_nu_vtx_dist_to_closest_tpc_wall = -1;
  reco_nu_vtx_fid_contained = -1;
  
  reco_asso_tracks = -1;
  reco_asso_showers = -1;

  reco_true_nuvert_dist = -1;

  longest_asso_track_matched_to_mcshower = -1;
  longest_asso_track_matched_to_mctrack = -1;
  longest_asso_track_matched_to_mcparticle = -1;
  longest_asso_track_true_pdg = 0;
  longest_asso_track_true_parent_pdg = 0;
  longest_asso_track_true_ancestor_pdg = 0;
  longest_asso_track_true_origin = -1;
  longest_asso_track_true_startx = -1; 
  longest_asso_track_true_starty = -120; 
  longest_asso_track_true_startz = -1;    
  longest_asso_track_true_endx = -1; 
  longest_asso_track_true_endy = -120; 
  longest_asso_track_true_endz = -1;    
  longest_asso_track_from_ncdeltarad = -1;
  longest_asso_track_is_ncdeltarad_track = -1;
  longest_asso_track_true_thetayx = -2;
  longest_asso_track_true_thetaxz = -2;
  longest_asso_track_true_thetayz = -2;
  longest_asso_track_true_energy = -1;
  
  shower_matching_ratio = -3;
  shower_matched_to_mcshower = -1;
  shower_matched_to_mctrack = -1;
  shower_matched_to_mcparticle = -1;
  shower_true_pdg = 0;
  shower_true_parent_pdg = 0;
  shower_true_ancestor_pdg = 0;
  shower_true_origin = -1;
  shower_true_startx = -1; 
  shower_true_starty = -120; 
  shower_true_startz = -1;    
  shower_true_endx = -1; 
  shower_true_endy = -120;
  shower_true_endz = -1;    
  shower_from_ncdeltarad = -1;
  shower_is_ncdeltarad_shower = -1;
  shower_true_dist = -1; 
  shower_true_distx = -1;
  shower_true_disty = -120;
  shower_true_distz = -1;
  shower_true_thetayx = -2;
  shower_true_thetaxz = -2;
  shower_true_thetayz = -2;
  shower_true_energy = -1;
  shower_detprofile_thetayx = -2;
  shower_detprofile_thetaxz = -2;
  shower_detprofile_thetayz = -2;
  shower_detprofile_energy = -1;
 
  closest_asso_shower_dist_to_flashzcenter = -1;

  most_energetic_shower_reco_startx = -1;
  most_energetic_shower_reco_starty = -120;
  most_energetic_shower_reco_startz = -1;
  most_energetic_shower_reco_dist = -1;
  most_energetic_shower_reco_distx = -1;
  most_energetic_shower_reco_disty = -120;
  most_energetic_shower_reco_distz = -1;
  most_energetic_shower_reco_thetayx = -2;
  most_energetic_shower_reco_thetaxz = -2;
  most_energetic_shower_reco_thetayz = -2;
  most_energetic_shower_reco_width0 = -1;
  most_energetic_shower_reco_width1 = -1;
  most_energetic_shower_reco_opening_angle = -4;
  most_energetic_shower_reco_length = -1;
  most_energetic_shower_reco_dirx = -2;
  most_energetic_shower_reco_diry = -2;
  most_energetic_shower_reco_dirz = -2;
  most_energetic_shower_reco_energy = -1;
  most_energetic_shower_helper_energy = -1;
  reco_shower_energy_vector.clear();
  most_energetic_shower_bp_dist_to_tpc = -1;
  reco_shower_dedx_vector.clear();
  reco_shower_dedx_plane0 = -1;
  reco_shower_dedx_plane1 = -1;
  reco_shower_dedx_plane2 = -1;
  reco_shower_dedx_best_plane = -1;
  closest_shower_dedx_plane2 = -1;
  closest_shower_dedx_best_plane = -1;

  shortest_asso_shower_to_vert_dist = -1;
  summed_associated_reco_shower_energy = -1;
  summed_associated_helper_shower_energy = -1;
  summed_associated_helper_track_energy = -1;

  pass_all_cuts = -1;

  shower_dist_to_flashzcenter = -1;
  shower_dist_to_flashzcenter_splitshowers = -1;

}


void FillTreeVariables::FillWeights(art::Event const & e) {
  
  art::ValidHandle<std::vector<evwgh::MCEventWeight>> const & ev_evw =
    e.getValidHandle<std::vector<evwgh::MCEventWeight>>("eventweight");

  std::map<std::string, std::vector<double>> const & weight_map = ev_evw->front().fWeight;

  if(ev_evw->size() > 1) {
    std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
	      << "WARNING: eventweight has more than one entry\n";
  }

  for(auto const & p : weight_map) {
    auto const wbm_it = weight_branch_map.find(p.first);
    if(wbm_it == weight_branch_map.end()) {
      std::cout << "Could not find weight: " << p.first << "\n";
      continue;
    }
    *wbm_it->second.at(0) = p.second.at(0);
    *wbm_it->second.at(1) = p.second.at(1);
  }

}


bool FillTreeVariables::SinglePhotonFilter(art::Event const & e,
					   size_t & mctruth_index) {

  if(fverbose) {
    std::cout << "SinglePhotonFilter: START\n";
  }

  art::ValidHandle<std::vector<simb::MCTruth>> const & ev_mct =
    e.getValidHandle<std::vector<simb::MCTruth>>("generator");  
  
  std::vector<int> pdg_v{13, -13, 11, -11, 22};
  std::vector<int> option_v{0, 0, 0, 0, 1};
  
  bool return_bool = false;

  for(size_t j = 0; j < ev_mct->size(); ++j) {

    simb::MCTruth const & mct = ev_mct->at(j);

    std::vector<int> counter_v(pdg_v.size(), 0);
  
    for(int i = 0; i < mct.NParticles(); ++i) {

      simb::MCParticle const & mcp = mct.GetParticle(i);

      if(mcp.StatusCode() == 1) {
	for(size_t i = 0; i < pdg_v.size(); ++i) {
	  if(mcp.PdgCode() == pdg_v.at(i)) {
	    ++counter_v.at(i);
	  }
	}
      }

    }

    bool temp = true;
    for(size_t i = 0; i < option_v.size(); ++i) {
      if(option_v.at(i) != counter_v.at(i)) {
	temp = false;
      }
    }
    
    if(temp) {
      return_bool = true;
      mctruth_index = j;
    }
    
  }    

  if(fverbose) {
    std::cout << "SinglePhotonFilter: FINISH\n";
  }
  
  return return_bool;
  
}


bool FillTreeVariables::DeltaRadFilter(art::Event const & e,
					  size_t & mctruth_index) {

  art::ValidHandle<std::vector<simb::MCTruth>> const & ev_mct =
    e.getValidHandle<std::vector<simb::MCTruth>>("generator");  

  for(size_t i = 0; i < ev_mct->size(); ++i) {
    
    simb::MCTruth const & mct = ev_mct->at(i);
    if(mct.GetNeutrino().CCNC() != 1) continue;
   
    std::vector<size_t> exiting_photon_parents;
    for(int i = 0; i < mct.NParticles(); ++i) {
      simb::MCParticle const & mcp = mct.GetParticle(i);
      if(mcp.TrackId() != i) {
	std::cout << "ERROR: " << __LINE__ << " " << __PRETTY_FUNCTION__ << "\nTrackId does not match index\n";
	exit(1);
      }
      if(!(mcp.StatusCode() == 1 && mcp.PdgCode() == 22)) continue;
      exiting_photon_parents.push_back(mcp.Mother());
    }

    std::vector<size_t> in_nucleus_photons;
    for(size_t const s : exiting_photon_parents) {
      simb::MCParticle const & mcp = mct.GetParticle(s);
      if(abs(mcp.PdgCode()) == 2114 || abs(mcp.PdgCode()) == 2214) {
	mctruth_index = i;
	return true;
      }
      else if(mcp.PdgCode() == 22) {
	in_nucleus_photons.push_back(mcp.Mother());
      }
    }
    
    for(size_t const s : in_nucleus_photons) {
      simb::MCParticle const & mcp = mct.GetParticle(s);
      if(abs(mcp.PdgCode()) == 2114 || abs(mcp.PdgCode()) == 2214) {
	mctruth_index = i;
	return true;
      }
    }
    
  }

  return false;

}


bool FillTreeVariables::OldDeltaRadFilter(art::Event const & e,
					  size_t & mctruth_index) {

  art::ValidHandle<std::vector<simb::MCTruth>> const & ev_mct =
    e.getValidHandle<std::vector<simb::MCTruth>>("generator");  

   size_t delta_counter = 0;

   for(size_t i = 0; i < ev_mct->size(); ++i) {

     simb::MCTruth const & mct = ev_mct->at(i);

     if(!mct.GetNeutrino().CCNC()) continue;

     size_t exiting_photon_counter = 0;
     int parent_id = -1;

     for(int j = mct.NParticles() - 1; j >= 0; --j) {
       
       simb::MCParticle const & mcp = mct.GetParticle(j);
       
       if(mcp.PdgCode() == 22) {
	 
	 if(mcp.StatusCode() == 1) {
	   parent_id = mcp.Mother();
	   if(++exiting_photon_counter > 1) break;
	 }

	 else if(mcp.TrackId() == parent_id) {
	   parent_id = mcp.Mother();
	 }

       }

       else if((abs(mcp.PdgCode()) == 2214 || abs(mcp.PdgCode()) == 2114) &&
	       mcp.TrackId() == parent_id) {
	 mctruth_index = i;
	 ++delta_counter;
	 break;
       }
       
     }

   }      

   if(delta_counter > 0) return true;
   
   return false;

}


bool FillTreeVariables::PassedSWTrigger(art::Event const & e) const {
  
  art::ValidHandle<::raw::ubdaqSoftwareTriggerData> const & swt =
    e.getValidHandle<::raw::ubdaqSoftwareTriggerData>("swtrigger");

  std::vector<std::string> const & algo_v = swt->getListOfAlgorithms();

  std::string const int_str = "BNB_FEMBeamTriggerAlgo";
  std::string const ext_str = "EXT_BNBwin_FEMBeamTriggerAlgo";

  auto const int_it = std::find(algo_v.begin(), algo_v.end(), int_str);
  auto const ext_it = std::find(algo_v.begin(), algo_v.end(), ext_str);

  if(int_it != algo_v.end() && ext_it != algo_v.end()) {
    std::cout << "function: " << __PRETTY_FUNCTION__ << " line: " << __LINE__ << std::endl
	      << "Found both swtriggers\n";
    return false;
  }
  else if(int_it == algo_v.end() && ext_it == algo_v.end()) {
    std::cout << "function: " << __PRETTY_FUNCTION__ << " line: " << __LINE__ << std::endl
	      << "Found neither swtrigger\n";
    return false;
  }
  else if(int_it != algo_v.end()) {
    return swt->passedAlgo(int_str);
  }      
  else if(ext_it != algo_v.end()) {
    return swt->passedAlgo(ext_str);
  }

  std::cout << __PRETTY_FUNCTION__ << std::endl;
  return false;

}


void FillTreeVariables::FillTruth(art::Event const & e,
				  size_t & delta_rad_mct_index) {
    
  art::ValidHandle<std::vector<simb::MCTruth>> const & ev_mct =
    e.getValidHandle<std::vector<simb::MCTruth>>("generator");  
  
  if(SinglePhotonFilter(e, delta_rad_mct_index)) is_single_photon = 1;
  else is_single_photon = 0;
  if(FilterSignal(e, delta_rad_mct_index)) is_delta_rad = 1;
  else is_delta_rad = 0;
  if(is_delta_rad == 1) {
    if(delta_rad_mct_index == SIZE_MAX) {
      std::cout << "delta_rad_mct_index == SIZE_MAX\n";
      return;
    }
    if(delta_rad_mct_index >= ev_mct->size()) {
      std::cout << "delta_rad_mct_index: " << delta_rad_mct_index
		<< " >= ev_mct->size(): " << ev_mct->size() << std::endl;
      return;
    }
  }
    
  if(is_delta_rad == 0) delta_rad_mct_index = 0;
  simb::MCTruth const & mct = ev_mct->at(delta_rad_mct_index);
  simb::MCNeutrino const & mcn = mct.GetNeutrino();

  nu_pdg = mcn.Nu().PdgCode();
  nu_energy = mcn.Nu().Trajectory().E(0);
  lep_pdg = mcn.Lepton().PdgCode();
  lep_energy = mcn.Lepton().Trajectory().E(0);
  ccnc = mcn.CCNC();
  mode = mcn.Mode();
  interaction_type = mcn.InteractionType();

  exiting_photon_number = 0;
  exiting_proton_number = 0;
  exiting_neutron_number = 0;
  exiting_electron_number = 0;
  exiting_antielectron_number = 0;
  exiting_muon_number = 0;
  exiting_antimuon_number = 0;
  exiting_piplus_number = 0;
  exiting_piminus_number = 0;
  exiting_pi0_number = 0;
  total_exiting_particles = 0;
    
  for(int i = 0; i < mct.NParticles(); ++i) {
    simb::MCParticle const & mcp = mct.GetParticle(i);
    if(mcp.StatusCode() != 1) continue; 
    switch(mcp.PdgCode()) {
    case 22:
      ++exiting_photon_number;
      break;
    case 2212:
      ++exiting_proton_number;
      break;
    case 2112:
      ++exiting_neutron_number;
      break;
    case 11:
      ++exiting_electron_number;
      break;
    case -11:
      ++exiting_antielectron_number;
      break;
    case 13:
      ++exiting_muon_number;
      break;
    case -13:
      ++exiting_antimuon_number;
      break;
    case 211:
      ++exiting_piplus_number;
      break;
    case -211:
      ++exiting_piminus_number;
      break;
    case 111:
      ++exiting_pi0_number;
      break;
    }
    ++total_exiting_particles;
    exiting_particle_vector.push_back(mcp.PdgCode());
  }

  TLorentzVector const & true_nu_pos = mcn.Nu().Trajectory().Position(0);
  if(ftpc_volume.Contain(geoalgo::Point_t(true_nu_pos))) true_nu_vtx_tpc_contained = 1;
  else true_nu_vtx_tpc_contained = 0;
  if(ffiducial_volume.Contain(geoalgo::Point_t(true_nu_pos))) true_nu_vtx_fid_contained = 1;
  else true_nu_vtx_fid_contained = 0;

  true_nuvertx = true_nu_pos.X();
  true_nuverty = true_nu_pos.Y();
  true_nuvertz = true_nu_pos.Z();

  true_nu_E = mcn.Nu().Trajectory().E(0);
    
}


void FillTreeVariables::GetDeltaMCShowerMCTrackIndices(art::Event const & e,
						       size_t const delta_rad_mct_index,
						       int & delta_photon_index,
						       int & delta_mcshower_index,
						       int & delta_proton_index,
						       int & delta_mctrack_index) {

  art::ValidHandle<std::vector<simb::MCTruth>> const & ev_mctruth =
    e.getValidHandle<std::vector<simb::MCTruth>>("generator");  
  
  art::ValidHandle<std::vector<sim::MCTrack>> const & ev_mctrack =
    e.getValidHandle<std::vector<sim::MCTrack>>("mcreco");
  
  art::ValidHandle<std::vector<sim::MCShower>> const & ev_mcshower =
    e.getValidHandle<std::vector<sim::MCShower>>("mcreco");

  simb::MCTruth const & mct = ev_mctruth->at(delta_rad_mct_index);

  int delta_index = -1;
  for(int i = 0; i < mct.NParticles(); ++i) {
    simb::MCParticle const & mcp = mct.GetParticle(i);
    if(abs(mcp.PdgCode()) == 2214 || abs(mcp.PdgCode()) == 2114) {
      delta_index = i;
      delta_true_pdg = mcp.PdgCode();
      delta_true_energy = mcp.Trajectory().E(0);
      break;
    }
  }

  if(delta_index == -1) {
    std::cout << "No delta\n";
    return;
  }

  std::vector<int> children_pos;
  std::vector<int> children_ext_pos;
  for(int i = 0; i < mct.NParticles(); ++i) {
    simb::MCParticle const & mcp = mct.GetParticle(i);
    if(mcp.Mother() == mct.GetParticle(delta_index).TrackId()) {
      if(mcp.StatusCode() == 1) {
	children_ext_pos.push_back(i);  
      }
      else {
	children_pos.push_back(i);
      }
    }
  }

  for(int i = 0; i < mct.NParticles(); ++i) {
    simb::MCParticle const & mcp = mct.GetParticle(i);
    if(mcp.StatusCode() == 1) {
      for(int const j : children_pos) {
	simb::MCParticle const & mcp_c = mct.GetParticle(j);
	if(mcp.Mother() == mcp_c.TrackId()) {
	  children_ext_pos.push_back(i);
	}
      }
    }
  }

  int photon_counter = 0;
  int proton_counter = 0;
  int iphoton_index = -1;
  int iproton_index = -1;
  for(int const i : children_ext_pos) {
    simb::MCParticle const & mcp = mct.GetParticle(i);
    if(mcp.PdgCode() == 22) {
      ++photon_counter;
      iphoton_index = i;
    }
    if(mcp.PdgCode() == 2212) {
      ++proton_counter;
      iproton_index = i;
    }
  }

  if(iphoton_index == -1) {
    std::cout << "could not find photon\n";
    return;
  }
    
  delta_photon_index = iphoton_index;
  delta_proton_index = iproton_index;

  delta_photon_energy = mct.GetParticle(delta_photon_index).Trajectory().E(0);
  if(delta_proton_index != -1) 
    delta_proton_energy = mct.GetParticle(delta_proton_index).Trajectory().E(0);
    
  double const diffd = 1e-10;
  double const diffp = 1e-10;
  double const diffE = 1e-2;

  std::vector<int> exiting_visible_particles;
  for(int i = 0; i < mct.NParticles(); ++i) {
    simb::MCParticle const & mcp = mct.GetParticle(i);
    if(mcp.StatusCode() == 1 && 
       abs(mcp.PdgCode()) != 12 &&
       abs(mcp.PdgCode()) != 14 &&
       abs(mcp.PdgCode()) != 2112 &&
       abs(mcp.PdgCode()) != 111)
      exiting_visible_particles.push_back(i);
  }

  std::map<int, int> mcp_mcs;    
  for(size_t i = 0; i < ev_mcshower->size(); ++i) {
    sim::MCShower const & mcs = ev_mcshower->at(i);
    if(mcs.TrackID() != mcs.AncestorTrackID() || mcs.Origin() != 1) continue;
    double dist = 2000;
    double E = 1e12;
    int mcp_index = -1;
    geoalgo::Point_t mcsp(mcs.Start().Position());     
    for(int const i : exiting_visible_particles) {
      simb::MCParticle const & mcp = mct.GetParticle(i);
      if(mcp.PdgCode() != mcs.PdgCode()) continue;
      double tdist = mcsp.Dist(mcp.Trajectory().Position(0));
      double tE = fabs(mcs.Start().E() - mcp.Trajectory().E(0) * 1e3);
      if(tdist <= dist && tE < E && tE < diffE) {
	dist = tdist;
	E = tE;
	mcp_index = i;
      }
    }
    if(dist < diffd) {
      if(mcp_mcs.find(mcp_index) != mcp_mcs.end()) {
	auto mm_it = mcp_mcs.find(mcp_index);
	std::cout << "mcp_index already added, current mcsack trackid: "
		  << mcs.TrackID() << " previous mcsack trackid: "
		  << ev_mcshower->at(mm_it->second).TrackID() << "\nmcp_index: "
		  << mcp_index << " mcp trackid: " << mct.GetParticle(mcp_index).TrackId() << std::endl;
	exit(1);
      }
      mcp_mcs.emplace(mcp_index, i);
    }
  }

  auto const mcs_it = mcp_mcs.find(delta_photon_index);
  if(mcs_it != mcp_mcs.end()) {
    delta_mcshower_index = mcs_it->second;
  }
    
  if(delta_mcshower_index != -1) {
    sim::MCShower const & mcs = ev_mcshower->at(delta_mcshower_index);
    delta_mcshower_true_pdg = mcs.PdgCode();
    delta_mcshower_true_energy = mcs.Start().E();
    delta_mcshower_detprofile_energy = mcs.DetProfile().E();
  }

  std::map<int, int> mcp_mctr;
  for(size_t i = 0; i < ev_mctrack->size(); ++i) {
    sim::MCTrack const & mctr = ev_mctrack->at(i);
    if(mctr.TrackID() != mctr.AncestorTrackID() || mctr.Origin() != 1) continue;
    double mom = 2000;
    int mcp_index = -1;
    geoalgo::Point_t mctrp(mctr.Start().Position());
    for(int const i : exiting_visible_particles) {
      simb::MCParticle const & mcp = mct.GetParticle(i);
      if(mcp.PdgCode() != mctr.PdgCode()) continue;
      double tdist = mctrp.Dist(mcp.Trajectory().Position(0));
      double tmom = geoalgo::Vector_t(mctr.Start().Momentum()*1e-3).Dist(mcp.Trajectory().Momentum(0));
      if(tmom < mom && tdist < diffd) {
	mom = tmom;
	mcp_index = i;
      }
    }

    if(mom < diffp) {
      if(mcp_mctr.find(mcp_index) != mcp_mctr.end()) {
	auto mm_it = mcp_mctr.find(mcp_index);
	std::cout << "mcp_index already added, current mctrack trackid: "
		  << mctr.TrackID() << " previous mctrack trackid: "
		  << ev_mctrack->at(mm_it->second).TrackID() << "\nmcp_index: "
		  << mcp_index << " mcp trackid: " << mct.GetParticle(mcp_index).TrackId() << std::endl;
	std::cout << "Ancestor MCTrack list:\n";
	for(size_t i = 0; i < ev_mctrack->size(); ++i) {
	  sim::MCTrack const & mctr = ev_mctrack->at(i);
	  if(mctr.TrackID() != mctr.AncestorTrackID() || mctr.Origin() != 1) continue;
	  std::cout << i << " tid: " << mctr.TrackID() << " pdg: " << mctr.PdgCode() << " origin: " << mctr.Origin() << " Pos: " << geoalgo::Point_t(mctr.Start().Position()) << " Mom: " << geoalgo::Point_t(mctr.Start().Momentum()*1e-3) << "\n";
	}
	std::cout << "Gen Origin: " << mct.Origin() << "\nMCParticles:\n";
	for(int const i : exiting_visible_particles) {
	  simb::MCParticle const & mcp = mct.GetParticle(i);
	  std::cout << i << " " << mcp.TrackId() << " " << mcp.PdgCode() << " Pos: " << geoalgo::Point_t(mcp.Trajectory().Position(0)) << " Mom: " << geoalgo::Point_t(mcp.Trajectory().Momentum(0)) << "\n";
	}
	exit(1);
      }
      mcp_mctr.emplace(mcp_index, i);
    }
  }

  if(fverbose && exiting_visible_particles.size() != mcp_mctr.size() + mcp_mcs.size()) {
    std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
	      << "WARNING: not all particles matched\n";
    std::cout << "Ancestor MCShowers\n";
    for(size_t i = 0; i < ev_mcshower->size(); ++i) {
      sim::MCShower const & mcs = ev_mcshower->at(i);
      if(mcs.TrackID() != mcs.AncestorTrackID() || mcs.Origin() != 1) continue;
      std::cout << i << " tid: " << mcs.TrackID() << " pdg: " << mcs.PdgCode() << " origin: " << mcs.Origin() << " Pos: " << geoalgo::Point_t(mcs.Start().Position()) << " Mom: " << geoalgo::Point_t(mcs.Start().Momentum()*1e-3) << "\n";
    }
    std::cout << "Ancestor MCTracks\n";
    for(size_t i = 0; i < ev_mctrack->size(); ++i) {
      sim::MCTrack const & mctr = ev_mctrack->at(i);
      if(mctr.TrackID() != mctr.AncestorTrackID() || mctr.Origin() != 1) continue;
      std::cout << i << " tid: " << mctr.TrackID() << " pdg: " << mctr.PdgCode() << " origin: " << mctr.Origin() << " Pos: " << geoalgo::Point_t(mctr.Start().Position()) << " Mom: " << geoalgo::Point_t(mctr.Start().Momentum()*1e-3) << "\n";
    }
    std::cout << "Gen Origin: " << mct.Origin() << "\nExiting MCParticles\n";
    for(int const i : exiting_visible_particles) {
      simb::MCParticle const & mcp = mct.GetParticle(i);
      std::cout << i << " " << mcp.TrackId() << " " << mcp.PdgCode() << " Pos: " << geoalgo::Point_t(mcp.Trajectory().Position(0)) << " Mom: " << geoalgo::Point_t(mcp.Trajectory().Momentum(0)) << " Energy: " << mcp.Trajectory().E(0) << "\n";
    }
    std::cout << "Shower matches\n";
    for(auto const & p : mcp_mcs) {
      std::cout << p.first << " " << p.second << "\n";
    }
    std::cout << "Track matches\n";
    for(auto const & p : mcp_mctr) {
      std::cout << p.first << " " << p.second << "\n";
    }
    
  }

  auto const mctr_it = mcp_mctr.find(delta_proton_index);
  if(mctr_it != mcp_mctr.end()) {
    delta_mctrack_index = mctr_it->second;    
  }

  if(delta_mctrack_index != -1) {
    sim::MCTrack const & mctr = ev_mctrack->at(delta_mctrack_index);
    delta_mctrack_true_pdg = mctr.PdgCode();
    delta_mctrack_true_energy = mctr.Start().E();
    delta_mctrack_true_length = geoalgo::Point_t(mctr.Start().Position()).Dist(mctr.End().Position());
  }
    
}


double FillTreeVariables::DistToClosestTPCWall(geoalgo::Point_t const & pos) {

  double dist = fabs(pos.at(0));
  if(fabs(pos.at(0) - 2*lar::providerFrom<geo::Geometry>()->DetHalfWidth()) < dist) dist = fabs(pos.at(0) - 2*lar::providerFrom<geo::Geometry>()->DetHalfWidth());
  if(fabs(pos.at(1) + lar::providerFrom<geo::Geometry>()->DetHalfHeight()) < dist) dist = fabs(pos.at(1) + lar::providerFrom<geo::Geometry>()->DetHalfHeight());
  if(fabs(pos.at(1) - lar::providerFrom<geo::Geometry>()->DetHalfHeight()) < dist) dist = fabs(pos.at(1) - lar::providerFrom<geo::Geometry>()->DetHalfHeight()); 
  if(fabs(pos.at(2)) < dist) dist = fabs(pos.at(2));
  if(fabs(pos.at(2) - lar::providerFrom<geo::Geometry>()->DetLength()) < dist) dist = fabs(pos.at(2) - lar::providerFrom<geo::Geometry>()->DetLength());

  return dist;

}



double FillTreeVariables::GetShowerHelperEnergy(art::Event const & e,
						size_t const shower_index) {
  
  art::ValidHandle<std::vector<recob::PFParticle>> const & ev_pfp =
    e.getValidHandle<std::vector<recob::PFParticle>>(fshower_producer);    
  art::FindManyP<recob::Shower> PFPToShower(ev_pfp, e, fshower_producer);  

  art::Ptr<recob::Shower> const * shower_ptr = nullptr;

  for(size_t i = 0; i < ev_pfp->size(); ++i) {
    std::vector<art::Ptr<recob::Shower>> const & showers = PFPToShower.at(i);
    for(art::Ptr<recob::Shower> const & shower : showers) {
      if(shower.key() == shower_index) {
	shower_ptr = &shower;
	break;
      }
      if(shower_ptr) break;
    }
  }

  if(!shower_ptr) {
    std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
	      << "WARNING: no shower pointer found\n";
    return -1;
  }

  return energyHelper.showerEnergy(*shower_ptr, e, fshower_producer);

}


double FillTreeVariables::GetTrackHelperEnergy(art::Event const & e,
						size_t const track_index) {
  
  art::ValidHandle<std::vector<recob::PFParticle>> const & ev_pfp =
    e.getValidHandle<std::vector<recob::PFParticle>>(ftrack_producer);    
  art::FindManyP<recob::Track> PFPToTrack(ev_pfp, e, ftrack_producer);  

  art::Ptr<recob::Track> const * track_ptr = nullptr;

  for(size_t i = 0; i < ev_pfp->size(); ++i) {
    std::vector<art::Ptr<recob::Track>> const & tracks = PFPToTrack.at(i);
    for(art::Ptr<recob::Track> const & track : tracks) {
      if(track.key() == track_index) {
	track_ptr = &track;
	break;
      }
      if(track_ptr) break;
    }
  }

  if(!track_ptr) {
    std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
	      << "WARNING: no track pointer found\n";
    return -1;
  }

  return energyHelper.trackEnergy(*track_ptr, e, ftrack_producer);

}


int FillTreeVariables::GetBestShowerPlane(art::Event const & e,
					  size_t const shower_index) {
	
  art::ValidHandle<std::vector<recob::Shower>> const & ev_s =
    e.getValidHandle<std::vector<recob::Shower>>(fshower_producer);  
  art::FindManyP<recob::Hit> ShowerToHits(ev_s, e, fshower_producer);
  
  std::vector<art::Ptr<recob::Hit>> const & hits = ShowerToHits.at(shower_index);

  size_t plane0 = 0;
  size_t plane1 = 0;

  for(art::Ptr<recob::Hit> const & hit : hits) {
    if(hit->WireID().Plane == 0) ++plane0;
    else if(hit->WireID().Plane == 1) ++plane1;
  }

  return plane0 > plane1 ? 0 : 1;

}



void FillTreeVariables::FilldEdx(art::Event const & e,
				 size_t const most_energetic_associated_shower_index,
				 size_t const closest_associated_shower_index) {

  if(fverbose) std::cout << "FilldEdx\n";

  art::ValidHandle<std::vector<recob::Shower>> const & ev_s =
    e.getValidHandle<std::vector<recob::Shower>>(fshower_producer);    
  art::FindManyP<recob::PFParticle> ShowerToPFP(ev_s, e, fshower_producer);

  std::vector<art::Ptr<recob::PFParticle>> most_energetic_pfp = ShowerToPFP.at(most_energetic_associated_shower_index);
  if(most_energetic_pfp.size() != 1)
    std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
	      << "WARNING: most_energetic_pfp.size() != 1: " << most_energetic_pfp.size() << "\n";
  std::vector<double> dqdx_v(3, -1);
  std::vector<double> dqdx_hits_v(3, -1);
  std::vector<double> reco_shower_dedx_vector(3, -1);
  energyHelper.dQdx(most_energetic_pfp.front().key(), e, dqdx_v, dqdx_hits_v, 4, 2);
  energyHelper.dEdxFromdQdx(reco_shower_dedx_vector, dqdx_v);
  reco_shower_dedx_plane0 = reco_shower_dedx_vector.at(0);
  reco_shower_dedx_plane1 = reco_shower_dedx_vector.at(1);
  reco_shower_dedx_plane2 = reco_shower_dedx_vector.at(2);
  reco_shower_dedx_best_plane = reco_shower_dedx_plane2;
  if(reco_shower_dedx_best_plane == -1) {
    if(reco_shower_dedx_vector.at(0) == -1)
      reco_shower_dedx_best_plane = reco_shower_dedx_vector.at(1);
    else if(reco_shower_dedx_vector.at(1) == -1)
      reco_shower_dedx_best_plane = reco_shower_dedx_vector.at(0);
    else
      reco_shower_dedx_best_plane = reco_shower_dedx_vector.at(GetBestShowerPlane(e, most_energetic_associated_shower_index));
  }
  
  std::vector<art::Ptr<recob::PFParticle>> closest_pfp = ShowerToPFP.at(closest_associated_shower_index);
  if(closest_pfp.size() != 1)
    std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
	      << "WARNING: closest_pfp.size() != 1: " << closest_pfp.size() << "\n";
  std::vector<double> closest_shower_dedx_vector(3, -1);
  energyHelper.dQdx(closest_pfp.front().key(), e, dqdx_v, dqdx_hits_v, 4, 2);
  energyHelper.dEdxFromdQdx(closest_shower_dedx_vector, dqdx_v);
  closest_shower_dedx_plane2 = closest_shower_dedx_vector.at(2);
  closest_shower_dedx_best_plane = closest_shower_dedx_plane2;
  if(closest_shower_dedx_best_plane == -1) {
    if(closest_shower_dedx_vector.at(0) == -1)
      closest_shower_dedx_best_plane = closest_shower_dedx_vector.at(1);
    else if(closest_shower_dedx_vector.at(1) == -1)
      closest_shower_dedx_best_plane = closest_shower_dedx_vector.at(0);
    else
      closest_shower_dedx_best_plane = closest_shower_dedx_vector.at(GetBestShowerPlane(e, closest_associated_shower_index));
  }

  
}



double FillTreeVariables::ShowerZDistToClosestFlash(art::Event const & e,
						    int const shower_index) {

  art::ValidHandle<std::vector<recob::Shower>> const & ev_s =
    e.getValidHandle<std::vector<recob::Shower>>(fshower_producer);  
  art::ValidHandle<std::vector<recob::OpFlash>> const & ev_opf =
    e.getValidHandle<std::vector<recob::OpFlash>>(fopflash_producer);

  if(fverbose) std::cout << "ShowerZDistToClosestFlash\n";

  int opf_ibg_counter = 0;
  double shortest_dist = DBL_MAX;
  for(recob::OpFlash const & opf : *ev_opf) {
    if(fverbose) std::cout << "\topf time: " << opf.Time() << " (opf.Time() > 3.2 && opf.Time() < 4.8)\n";
    if(!(opf.Time() > 3.2 && opf.Time() < 4.8)) continue;
    if(fverbose) std::cout << "\tWithin time range\n";
    ++opf_ibg_counter;
    recob::Shower const & s = ev_s->at(shower_index);
    double zmin = s.ShowerStart().Z();
    double zmax = s.ShowerStart().Z() + s.Direction().Z() * s.Length();
    if(fverbose) std::cout << "\tShower index: " << shower_index << " zmin: " << zmin << " zmax: " << zmax << "\n";
    if(zmin > zmax) {
      std::swap(zmin, zmax);
      if(fverbose) std::cout << "\t\tSwap - Shower zmin: " << zmin << " " << zmax << "\n";
    }
    double const zcenter = opf.ZCenter();
    if(fverbose) std::cout << "\tflashzcenter: " << zcenter << "\n";
    double dist = DBL_MAX;
    if(zcenter < zmin) {
      dist = zmin - zcenter;
      if(fverbose) std::cout << "\tfzc - zmin dist: " << dist << "\n";
    }
    else if(zcenter > zmax) {
      dist = zcenter - zmax;
      if(fverbose) std::cout << "\tfzc - zmax dist: " << dist << "\n";
    } 
    else {
      dist = 0;
      if(fverbose) std::cout << "\tflazcenter inside shower\n";
    }
    if(dist < shortest_dist) shortest_dist = dist;
  }

  if(fverbose) std::cout << "\tshortest_dist: " << shortest_dist << "\n";

  if(opf_ibg_counter == 0) shortest_dist = -2;
  return shortest_dist;

}



void FillTreeVariables::FillShowerRecoMCMatching(art::Event const & e,
						 size_t const most_energetic_associated_shower_index,
						 size_t const delta_rad_mct_index,
						 size_t const delta_mcshower_index,
						 lar_pandora::MCParticlesToPFParticles const & matchedMCToPFParticles) {
  
  size_t mcparticle_index = SIZE_MAX;
  for(auto const & p : matchedMCToPFParticles) {
    if(p.second.key() == most_energetic_associated_shower_index) {
      mcparticle_index = p.first.key();
      break;
    }
  }
  if(mcparticle_index != SIZE_MAX) {
    art::ValidHandle<std::vector<simb::MCParticle>> const & ev_mcp =
      e.getValidHandle<std::vector<simb::MCParticle>>("largeant");
    art::ValidHandle<std::vector<sim::MCShower>> const & ev_mcs =
      e.getValidHandle<std::vector<sim::MCShower>>("mcreco");
    art::ValidHandle<std::vector<sim::MCTrack>> const & ev_mctr =
      e.getValidHandle<std::vector<sim::MCTrack>>("mcreco");
    simb::MCParticle const & mcp = ev_mcp->at(mcparticle_index);
    unsigned int const mcp_tid = mcp.TrackId();
    int const mcp_pdg = mcp.PdgCode();

    art::ServiceHandle<cheat::BackTracker> bt;
    art::Ptr<simb::MCTruth> const mct = bt->TrackIDToMCTruth(mcp_tid);

    size_t mcs_index = SIZE_MAX;
    for(size_t i = 0; i < ev_mcs->size(); ++i) {
      sim::MCShower const & mcs = ev_mcs->at(i);
      if(mcs.TrackID() != mcp_tid) continue;
      if(mcs.PdgCode() != mcp_pdg) {
	std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
		  << "ERROR: MCShower PDG: " << mcs.PdgCode() << " does not match MCParticle PDG: " << mcp_pdg << "\n";
	exit(1);
      }
      mcs_index = i;
    }
    size_t mctr_index = SIZE_MAX;
    for(size_t i = 0; i < ev_mctr->size(); ++i) {
      sim::MCTrack const & mctr = ev_mctr->at(i);
      if(mctr.TrackID() != mcp_tid) continue;
      if(mctr.PdgCode() != mcp_pdg) {
	std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
		  << "ERROR: MCTrack PDG: " << mctr.PdgCode() << " does not match MCParticle PDG: " << mcp_pdg << "\n";
	exit(1);
      }
      mctr_index = i;
    }
    if(mcs_index != SIZE_MAX && mctr_index != SIZE_MAX) {
      std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
		<< "ERROR: found MCShower and MCTrack matched to MCParticle";
      exit(1);
    }
    shower_matched_to_mcshower = 0;
    shower_matched_to_mctrack = 0;
    shower_matched_to_mcparticle = 0;
    if(mcs_index != SIZE_MAX) {
      shower_matched_to_mcshower = 1;
      sim::MCShower const & mcs = ev_mcs->at(mcs_index);
      shower_true_pdg = mcs.PdgCode();
      shower_true_parent_pdg = mcs.MotherPdgCode();
      shower_true_ancestor_pdg = mcs.AncestorPdgCode();
      shower_true_origin = mcs.Origin();
      shower_true_startx = mcs.Start().Position().X(); 
      shower_true_starty = mcs.Start().Position().Y(); 
      shower_true_startz = mcs.Start().Position().Z();
      shower_true_endx = mcs.End().Position().X(); 
      shower_true_endy = mcs.End().Position().Y();
      shower_true_endz = mcs.End().Position().Z();
      if(shower_true_origin == 1) {
	shower_from_ncdeltarad = 0;
	if(mct.key() == delta_rad_mct_index) shower_from_ncdeltarad = 1;
	shower_is_ncdeltarad_shower = 0;
	if(mcs_index == delta_mcshower_index) shower_is_ncdeltarad_shower = 1;
	geoalgo::Point_t const nuvert(mct->GetNeutrino().Nu().Position(0));
	shower_true_dist = nuvert.Dist(mcs.Start().Position());
	shower_true_distx = nuvert.at(0) - mcs.Start().Position().X();
	shower_true_disty = nuvert.at(1) - mcs.Start().Position().Y();
	shower_true_distz = nuvert.at(2) - mcs.Start().Position().Z();
      }
      geoalgo::Point_t const shower_dir(mcs.Start().Momentum());
      shower_true_thetayx = atan(shower_dir.at(1)/shower_dir.at(0));
      shower_true_thetaxz = atan(shower_dir.at(0)/shower_dir.at(2));
      shower_true_thetayz = atan(shower_dir.at(1)/shower_dir.at(2));
      shower_true_energy = mcs.Start().E() * 1e-3;
    }
    else if(mctr_index != SIZE_MAX) {
      shower_matched_to_mctrack = 1;
      sim::MCTrack const & mctr = ev_mctr->at(mctr_index);
      shower_true_pdg = mctr.PdgCode();
      shower_true_parent_pdg = mctr.MotherPdgCode();
      shower_true_ancestor_pdg = mctr.AncestorPdgCode();
      shower_true_origin = mctr.Origin();
      shower_true_startx = mctr.Start().Position().X(); 
      shower_true_starty = mctr.Start().Position().Y(); 
      shower_true_startz = mctr.Start().Position().Z();
      shower_true_endx = mctr.End().Position().X(); 
      shower_true_endy = mctr.End().Position().Y();
      shower_true_endz = mctr.End().Position().Z();
      geoalgo::Point_t const track_dir(mctr.Start().Momentum());
      shower_true_thetayx = atan(track_dir.at(1)/track_dir.at(0));
      shower_true_thetaxz = atan(track_dir.at(0)/track_dir.at(2));
      shower_true_thetayz = atan(track_dir.at(1)/track_dir.at(2));
      shower_true_energy = mctr.Start().E() * 1e-3;
    }
    else if(mcs_index == SIZE_MAX && mctr_index == SIZE_MAX) {
      shower_matched_to_mcparticle = 1;
      shower_true_pdg = mcp.PdgCode();
      shower_true_origin = mct->Origin();      
      shower_true_startx = mcp.Position(0).X();
      shower_true_starty = mcp.Position(0).Y();
      shower_true_startz = mcp.Position(0).Z(); 
      shower_true_startx = mcp.Position(mcp.NumberTrajectoryPoints()).X();
      shower_true_starty = mcp.Position(mcp.NumberTrajectoryPoints()).Y();
      shower_true_startz = mcp.Position(mcp.NumberTrajectoryPoints()).Z(); 
      geoalgo::Point_t const particle_dir(mcp.Momentum(0));
      shower_true_thetayx = atan(particle_dir.at(1)/particle_dir.at(0));
      shower_true_thetaxz = atan(particle_dir.at(0)/particle_dir.at(2));
      shower_true_thetayz = atan(particle_dir.at(1)/particle_dir.at(2));
      shower_true_energy = mcp.E(0) * 1e-3;
    }
  }
  
}



void FillTreeVariables::FillTrackRecoMCMatching(art::Event const & e,
						size_t const longest_asso_track_index,
						size_t const delta_rad_mct_index,
						size_t const delta_mctrack_index,
						lar_pandora::MCParticlesToPFParticles const & matchedMCToPFParticles) {
  size_t mcparticle_index = SIZE_MAX;
  for(auto const & p : matchedMCToPFParticles) {
    if(p.second.key() == longest_asso_track_index) {
      mcparticle_index = p.first.key();
      break;
    }
  }
  if(mcparticle_index != SIZE_MAX) {
    art::ValidHandle<std::vector<simb::MCParticle>> const & ev_mcp =
      e.getValidHandle<std::vector<simb::MCParticle>>("largeant");
    art::ValidHandle<std::vector<sim::MCShower>> const & ev_mcs =
      e.getValidHandle<std::vector<sim::MCShower>>("mcreco");
    art::ValidHandle<std::vector<sim::MCTrack>> const & ev_mctr =
      e.getValidHandle<std::vector<sim::MCTrack>>("mcreco");
    simb::MCParticle const & mcp = ev_mcp->at(mcparticle_index);
    unsigned int const mcp_tid = mcp.TrackId();
    int const mcp_pdg = mcp.PdgCode();

    art::ServiceHandle<cheat::BackTracker> bt;
    art::Ptr<simb::MCTruth> const mct = bt->TrackIDToMCTruth(mcp_tid);

    size_t mcs_index = SIZE_MAX;
    for(size_t i = 0; i < ev_mcs->size(); ++i) {
      sim::MCShower const & mcs = ev_mcs->at(i);
      if(mcs.TrackID() != mcp_tid) continue;
      if(mcs.PdgCode() != mcp_pdg) {
	std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
		  << "ERROR: MCShower PDG: " << mcs.PdgCode() << " does not match MCParticle PDG: " << mcp_pdg << "\n";
	exit(1);
      }
      mcs_index = i;
    }
    size_t mctr_index = SIZE_MAX;
    for(size_t i = 0; i < ev_mctr->size(); ++i) {
      sim::MCTrack const & mctr = ev_mctr->at(i);
      if(mctr.TrackID() != mcp_tid) continue;
      if(mctr.PdgCode() != mcp_pdg) {
	std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
		  << "ERROR: MCTrack PDG: " << mctr.PdgCode() << " does not match MCParticle PDG: " << mcp_pdg << "\n";
	exit(1);
      }
      mctr_index = i;
    }
    if(mcs_index != SIZE_MAX && mctr_index != SIZE_MAX) {
      std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
		<< "ERROR: found MCShower and MCTrack matched to MCParticle";
      exit(1);
    }
    longest_asso_track_matched_to_mcshower = 0;
    longest_asso_track_matched_to_mctrack = 0;
    longest_asso_track_matched_to_mcparticle = 0;
    if(mcs_index != SIZE_MAX) {
      longest_asso_track_matched_to_mcshower = 1;
      sim::MCShower const & mcs = ev_mcs->at(mcs_index);
      longest_asso_track_true_pdg = mcs.PdgCode();
      longest_asso_track_true_parent_pdg = mcs.MotherPdgCode();
      longest_asso_track_true_ancestor_pdg = mcs.AncestorPdgCode();
      longest_asso_track_true_origin = mcs.Origin();
      longest_asso_track_true_startx = mcs.Start().Position().X(); 
      longest_asso_track_true_starty = mcs.Start().Position().Y(); 
      longest_asso_track_true_startz = mcs.Start().Position().Z();
      longest_asso_track_true_endx = mcs.End().Position().X(); 
      longest_asso_track_true_endy = mcs.End().Position().Y();
      longest_asso_track_true_endz = mcs.End().Position().Z();
      geoalgo::Point_t const track_dir(mcs.Start().Momentum());
      longest_asso_track_true_thetayx = atan(track_dir.at(1)/track_dir.at(0));
      longest_asso_track_true_thetaxz = atan(track_dir.at(0)/track_dir.at(2));
      longest_asso_track_true_thetayz = atan(track_dir.at(1)/track_dir.at(2));
      longest_asso_track_true_energy = mcs.Start().E() * 1e-3;
    }
    else if(mctr_index != SIZE_MAX) {
      longest_asso_track_matched_to_mctrack = 1;
      sim::MCTrack const & mctr = ev_mctr->at(mctr_index);
      longest_asso_track_true_pdg = mctr.PdgCode();
      longest_asso_track_true_parent_pdg = mctr.MotherPdgCode();
      longest_asso_track_true_ancestor_pdg = mctr.AncestorPdgCode();
      longest_asso_track_true_origin = mctr.Origin();
      longest_asso_track_true_startx = mctr.Start().Position().X(); 
      longest_asso_track_true_starty = mctr.Start().Position().Y(); 
      longest_asso_track_true_startz = mctr.Start().Position().Z();
      longest_asso_track_true_endx = mctr.End().Position().X(); 
      longest_asso_track_true_endy = mctr.End().Position().Y();
      longest_asso_track_true_endz = mctr.End().Position().Z();
      if(longest_asso_track_true_origin == 1) {
	longest_asso_track_from_ncdeltarad = 0;
	if(mct.key() == delta_rad_mct_index) longest_asso_track_from_ncdeltarad = 1;
	longest_asso_track_is_ncdeltarad_track = 0;
	if(mctr_index == delta_mctrack_index) longest_asso_track_is_ncdeltarad_track = 1;
      }
      geoalgo::Point_t const track_dir(mctr.Start().Momentum());
      longest_asso_track_true_thetayx = atan(track_dir.at(1)/track_dir.at(0));
      longest_asso_track_true_thetaxz = atan(track_dir.at(0)/track_dir.at(2));
      longest_asso_track_true_thetayz = atan(track_dir.at(1)/track_dir.at(2));
      longest_asso_track_true_energy = mctr.Start().E() * 1e-3;
    }
    else if(mcs_index == SIZE_MAX && mctr_index == SIZE_MAX) {
      longest_asso_track_matched_to_mcparticle = 1;
      longest_asso_track_true_pdg = mcp.PdgCode();
      longest_asso_track_true_origin = mct->Origin();      
      longest_asso_track_true_startx = mcp.Position(0).X();
      longest_asso_track_true_starty = mcp.Position(0).Y();
      longest_asso_track_true_startz = mcp.Position(0).Z(); 
      longest_asso_track_true_endx = mcp.Position(mcp.NumberTrajectoryPoints()).X();
      longest_asso_track_true_endy = mcp.Position(mcp.NumberTrajectoryPoints()).Y();
      longest_asso_track_true_endz = mcp.Position(mcp.NumberTrajectoryPoints()).Z(); 
      geoalgo::Point_t const particle_dir(mcp.Momentum(0));
      longest_asso_track_true_thetayx = atan(particle_dir.at(1)/particle_dir.at(0));
      longest_asso_track_true_thetaxz = atan(particle_dir.at(0)/particle_dir.at(2));
      longest_asso_track_true_thetayz = atan(particle_dir.at(1)/particle_dir.at(2));
      longest_asso_track_true_energy = mcp.E(0) * 1e-3;
    }
  }

}




void FillTreeVariables::FillVertexTree(art::Event const & e,
				       ParticleAssociations const & pas,
				       size_t const pn,
				       size_t const delta_rad_mct_index,
				       size_t const delta_mcshower_index,
				       size_t const delta_mctrack_index,
				       lar_pandora::MCParticlesToPFParticles const & matchedMCToPFParticles) {

  ResetVertex();

  art::ValidHandle<std::vector<recob::Track>> const & ev_t =
    e.getValidHandle<std::vector<recob::Track>>(ftrack_producer);
  art::ValidHandle<std::vector<recob::Shower>> const & ev_s =
    e.getValidHandle<std::vector<recob::Shower>>(fshower_producer);
  
  DetectorObjects const & detos = pas.GetDetectorObjects();
  ParticleAssociation const & pa = pas.GetAssociations().at(pn);
  geoalgo::Point_t const & reco_vertex = pa.GetRecoVertex();
  
  reco_nuvertx = reco_vertex.at(0);
  reco_nuverty = reco_vertex.at(1);
  reco_nuvertz = reco_vertex.at(2);

  reco_nu_vtx_dist_to_closest_tpc_wall = DistToClosestTPCWall(reco_vertex);
  if(ffiducial_volume.Contain(reco_vertex)) reco_nu_vtx_fid_contained = 1;
  else reco_nu_vtx_fid_contained = 0;
    
  reco_asso_tracks = 0;
  reco_asso_showers = 0;

  if(fverbose) std::cout << "Find most energetic shower\n";

  double most_shower_energy = 0;
  size_t most_energetic_associated_shower_index = SIZE_MAX;
  for(size_t const n : pa.GetObjectIndices()) {
    size_t const original_index = detos.GetDetectorObject(n).foriginal_index;
    if(fverbose) std::cout << "\tsize_t: " << n << " original_index: " << original_index << "\n";
    if(detos.GetRecoType(n) == detos.fshower_reco_type) {
      if(fverbose) std::cout << "\tis a shower\n";
      recob::Shower const & s = ev_s->at(original_index);
      double shower_energy = -1;
      shower_energy = s.Energy().at(fwire_plane);
      if(fverbose) {
	std::cout << "\tCurrent shower energy: " << shower_energy << "\n"
		  << "\tIs current energy: " << shower_energy << " > most shower energy: " << most_shower_energy << "?\n";
      }
      if(shower_energy > most_shower_energy) {
	if(fverbose) std::cout << "\t\tYes\n";
	most_shower_energy = shower_energy;
	most_energetic_associated_shower_index = original_index;
      }
      else if(most_energetic_associated_shower_index == SIZE_MAX) {
	most_shower_energy = -1;
	most_energetic_associated_shower_index = original_index;
      }
    }
  }
    
  if(fverbose) std::cout << "End, most energetic shower index: " << most_energetic_associated_shower_index  << " with energy: " << most_shower_energy << "\n";

  double track_length = 0;
  summed_associated_reco_shower_energy = 0;
  summed_associated_helper_shower_energy = 0;
  summed_associated_helper_track_energy = 0;
  closest_asso_shower_dist_to_flashzcenter = DBL_MAX;
  for(size_t const n : pa.GetObjectIndices()) {
    if(fverbose) std::cout << "\tObject index: " << n << "\n";
    size_t const original_index = detos.GetDetectorObject(n).foriginal_index;
    if(fverbose) std::cout << "\tOriginal index: " << n << "\n";
    if(detos.GetRecoType(n) == detos.ftrack_reco_type) {
      ++reco_asso_tracks;
      recob::Track const & t = ev_t->at(original_index);
      track_length = geoalgo::Point_t(t.Vertex()).Dist(t.End());
      if(track_length > longest_asso_track_length) {
	longest_asso_track_length = track_length;
	longest_asso_track_reco_dirx = t.VertexDirection().X();
	longest_asso_track_reco_diry = t.VertexDirection().Y();
	longest_asso_track_reco_dirz = t.VertexDirection().Z();
	longest_asso_track_thetayx = atan(longest_asso_track_reco_diry/longest_asso_track_reco_dirx);
	longest_asso_track_thetaxz = atan(longest_asso_track_reco_dirx/longest_asso_track_reco_dirz);
	longest_asso_track_thetayz = atan(longest_asso_track_reco_diry/longest_asso_track_reco_dirz);
      }
    }
    if(detos.GetRecoType(n) == detos.fshower_reco_type) {
      ++reco_asso_showers;
      recob::Shower const & s = ev_s->at(original_index);
      if(s.Energy().at(fwire_plane) > 0) summed_associated_reco_shower_energy += s.Energy().at(fwire_plane);
      summed_associated_helper_shower_energy += GetShowerHelperEnergy(e, original_index);
      double const dist = ShowerZDistToClosestFlash(e, original_index);
      if(dist < closest_asso_shower_dist_to_flashzcenter) closest_asso_shower_dist_to_flashzcenter = dist;
    }
  }

  if(fverbose) std::cout << "Get shortest shower to vertex distance\n";

  size_t closest_associated_shower_index = SIZE_MAX;
  shortest_asso_shower_to_vert_dist = DBL_MAX;
  for(size_t const n : pa.GetObjectIndices()) {
    size_t const original_index = detos.GetDetectorObject(n).foriginal_index;
    if(detos.GetRecoType(n) != detos.fshower_reco_type) continue;
    recob::Shower const & s = ev_s->at(original_index);
    if(geoalgo::Point_t(s.ShowerStart()).Dist(pa.GetRecoVertex()) < shortest_asso_shower_to_vert_dist) {
      shortest_asso_shower_to_vert_dist = geoalgo::Point_t(s.ShowerStart()).Dist(pa.GetRecoVertex());
      closest_associated_shower_index = original_index;
    }
  }

  if(shortest_asso_shower_to_vert_dist == DBL_MAX) shortest_asso_shower_to_vert_dist = -1;

  if(closest_asso_shower_dist_to_flashzcenter == DBL_MAX) closest_asso_shower_dist_to_flashzcenter = -1;

  if(fverbose) std::cout << "Most energetic shower index: " << most_energetic_associated_shower_index << "\n";

  if(most_energetic_associated_shower_index != SIZE_MAX) {

    recob::Shower const & sh = ev_s->at(most_energetic_associated_shower_index);
    geoalgo::Point_t const shower_start = sh.ShowerStart();
    geoalgo::Point_t const shower_dir = sh.Direction();

    most_energetic_shower_reco_startx = shower_start.at(0);
    most_energetic_shower_reco_starty = shower_start.at(1);
    most_energetic_shower_reco_startz = shower_start.at(2);
    if(reco_asso_tracks > 0) most_energetic_shower_reco_dist = shower_start.Dist(pa.GetRecoVertex());
    most_energetic_shower_reco_distx = shower_start.at(0) - reco_nuvertx;
    most_energetic_shower_reco_disty = shower_start.at(1) - reco_nuverty;
    most_energetic_shower_reco_distz = shower_start.at(2) - reco_nuvertz;
    most_energetic_shower_reco_thetayx = atan(shower_dir.at(1)/shower_dir.at(0));
    most_energetic_shower_reco_thetaxz = atan(shower_dir.at(0)/shower_dir.at(2));
    most_energetic_shower_reco_thetayz = atan(shower_dir.at(1)/shower_dir.at(2)); 
    //most_energetic_shower_reco_width0 = sh.Width()[0];
    //most_energetic_shower_reco_width1 = sh.Width()[1];
    //most_energetic_shower_reco_opening_angle = sh.OpeningAngle();
    most_energetic_shower_reco_length = sh.Length();
    most_energetic_shower_reco_dirx = shower_dir.at(0);
    most_energetic_shower_reco_diry = shower_dir.at(1);
    most_energetic_shower_reco_dirz = shower_dir.at(2);
    most_energetic_shower_reco_energy = sh.Energy().at(fwire_plane);
    most_energetic_shower_helper_energy = GetShowerHelperEnergy(e, most_energetic_associated_shower_index);
    if(ftpc_volume.Contain(shower_start) && shower_start.at(0) != 0 && shower_start.at(1) != 0 && shower_start.at(2) != 0) {
      std::vector<geoalgo::Point_t> const pv = falgo.Intersection(ftpc_volume, geoalgo::HalfLine(shower_start, shower_dir), true);
      if(pv.empty()) {
	std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\nERROR: shower backwards projection does not intersect with tpc boundary:\n"
		  << "shower_start: " << shower_start << " shower_dir: " << shower_dir << "\n";
	exit(1);
      }
      else if(pv.size() > 1) {
	std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\nERROR: shower backwards projection intersects tpc boundary more than once:\n"
		  << "shower_start: " << shower_start << " shower_dir: " << shower_dir << "\n"
		  << "intersections: ";
	for(geoalgo::Point_t const & p : pv) {
	  std::cout << p << " ";
	}
	std::cout << "\n";
	exit(1);
      }
      else most_energetic_shower_bp_dist_to_tpc = pv.front().Dist(shower_start);
    }
    if(most_energetic_shower_reco_energy < 0) most_energetic_shower_reco_energy = -1;

    FilldEdx(e, most_energetic_associated_shower_index, closest_associated_shower_index);

    shower_dist_to_flashzcenter = ShowerZDistToClosestFlash(e, most_energetic_associated_shower_index);

  }

  if(fmcordata == "mc") {
    
    art::ValidHandle<std::vector<simb::MCTruth>> const & ev_mct =
      e.getValidHandle<std::vector<simb::MCTruth>>("generator");      

    reco_true_nuvert_dist = reco_vertex.Dist(ev_mct->at(delta_rad_mct_index).GetNeutrino().Nu().Position(0));
    
    if(most_energetic_associated_shower_index != SIZE_MAX && fmcrecomatching) {
      FillShowerRecoMCMatching(e,
			       most_energetic_associated_shower_index,
			       delta_rad_mct_index,
			       delta_mcshower_index,
			       matchedMCToPFParticles);
    }
  
  }

  fvertex_tree->Fill();
  
}


void FillTreeVariables::Fill(art::Event const & e,
			     ParticleAssociations const & pas) {
    
  ResetEvent();

  nue_xsec::recotruehelper _recotruehelper_instance;
  lar_pandora::MCParticlesToPFParticles matchedMCToPFParticles;
  lar_pandora::MCParticlesToHits matchedParticleHits;

  size_t delta_rad_mct_index = SIZE_MAX;
  int delta_mcshower_index = -1;
  int delta_mctrack_index = -1;
  
  if(fmcordata == "mc") {
    
    art::ValidHandle<std::vector<simb::MCTruth>> const & ev_mct =
      e.getValidHandle<std::vector<simb::MCTruth>>("generator");  
    art::ValidHandle<std::vector<sim::MCTrack>> const & ev_mctr =
      e.getValidHandle<std::vector<sim::MCTrack>>("mcreco");
    art::ValidHandle<std::vector<sim::MCShower>> const & ev_mcs =
      e.getValidHandle<std::vector<sim::MCShower>>("mcreco");

    FillWeights(e);

    if(ev_mct->front().GetNeutrino().Nu().Mother() == -1) 
      FillTruth(e, delta_rad_mct_index);
    
    mctracknumber = ev_mctr->size();
    mcshowernumber = ev_mcs->size();

    int delta_photon_index = -1;
    int delta_proton_index = -1;
    
    if(is_delta_rad == 1) {
      GetDeltaMCShowerMCTrackIndices(e,
				     delta_rad_mct_index,
				     delta_photon_index,
				     delta_mcshower_index,
				     delta_proton_index,
				     delta_mctrack_index);
    }

    _recotruehelper_instance.Configure(e, ftrack_producer, ftrack_producer, "pandoraCosmicHitRemoval", "largeant");
    _recotruehelper_instance.GetRecoToTrueMatches(matchedMCToPFParticles, matchedParticleHits);

  }

  art::ValidHandle<std::vector<recob::Track>> const & ev_t =
    e.getValidHandle<std::vector<recob::Track>>(ftrack_producer);
  art::ValidHandle<std::vector<recob::Shower>> const & ev_s =
    e.getValidHandle<std::vector<recob::Shower>>(fshower_producer);
  art::ValidHandle<std::vector<recob::OpFlash>> const & ev_opf =
    e.getValidHandle<std::vector<recob::OpFlash>>(fopflash_producer);

  run_number = e.id().run();
  subrun_number = e.id().subRun();
  event_number = e.id().event();

  tracknumber = ev_t->size();
  showernumber = ev_s->size();

  if(PassedSWTrigger(e)) {
    passed_swtrigger = 1;
  }
  else {
    passed_swtrigger = 0;
  }

  totalpe_sum = 0;
  totalpe_ibg_sum = 0;
  totalpe_bbg_sum = 0;

  for(recob::OpFlash const & opf : *ev_opf) {
    totalpe_sum += opf.TotalPE();
    if(opf.Time() > 3.2 && opf.Time() < 4.8) {
      totalpe_ibg_sum += opf.TotalPE();
    }
    else if(opf.Time() < 3.2) {
      totalpe_bbg_sum += opf.TotalPE();
    }
  }

  for(recob::Shower const & s : *ev_s) {
    double shower_energy = -1;
    shower_energy = s.Energy().at(fwire_plane);
    if(shower_energy > most_energetic_reco_shower) {
      second_most_energetic_reco_shower = most_energetic_reco_shower;
      most_energetic_reco_shower = shower_energy;
    }
    else if(shower_energy > second_most_energetic_reco_shower) {
      second_most_energetic_reco_shower = shower_energy;
    }
  }
    
  number_of_selected_vertices_cut = 0;
  for(size_t const pn : pas.GetSelectedAssociations()) {
    FillVertexTree(e, pas, pn, delta_rad_mct_index, delta_mcshower_index, delta_mctrack_index, matchedMCToPFParticles);
  } 

  number_of_selected_vertices = pas.GetAssociations().size();

  fevent_tree->Fill();

}

#endif
