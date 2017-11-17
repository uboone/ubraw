////////////////////////////////////////////////////////////////////////
// Class:       GENIETest
// Plugin Type: analyzer (art v2_05_00)
// File:        GENIETest_module.cc
//
// Generated at Thu Jul  6 07:04:02 2017 by Robert Murrells using cetskelgen
// from cetlib version v1_21_00.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"

#include "TTree.h"
#include "TFile.h"

#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCParticle.h"


class GENIETest : public art::EDAnalyzer {

  bool fverbose;

  TTree * ftree;

  int fnupdg;
  double fnuenergy;
  int fleppdg;
  double flepenergy;
  int fccnc;
  int fmode;
  int finteractiontype;

  int fis_ncdeltarad;
  int fnphotons;

  int fdpdg;
  double fdenergy;

  int fppdg;
  double fpenergy;
  double fpmomx;
  double fpmomy;
  double fpmomz;
  double fpmom;
  double fpcth;

  int fnpdg;
  double fnenergy;
  double fnmomx;
  double fnmomy;
  double fnmomz;
  double fnmom;

  double fdm;

public:

  explicit GENIETest(fhicl::ParameterSet const & p);

  GENIETest(GENIETest const &) = delete;
  GENIETest(GENIETest &&) = delete;
  GENIETest & operator = (GENIETest const &) = delete;
  GENIETest & operator = (GENIETest &&) = delete;

  void reconfigure(fhicl::ParameterSet const & p);
  void reset();
  void fill_dr_vars(art::Event const & e, size_t const mct_index, size_t const dr_delta, size_t const dr_photon, size_t const dr_nucleon);
  size_t deltarad_filter(art::Event const & e);
  void cout_particle_list(simb::MCTruth const & mct);
  void analyze(art::Event const & e) override;

};


void GENIETest::reconfigure(fhicl::ParameterSet const & p) {

  fverbose = p.get<bool>("verbose");

}


GENIETest::GENIETest(fhicl::ParameterSet const & p) :
  EDAnalyzer(p),
  fverbose(false),
  ftree(nullptr) {
  
  art::ServiceHandle< art::TFileService > tfs;
  ftree = tfs->make<TTree>("GENIETest", "");

  ftree->Branch("nupdg", &fnupdg, "nupdg/I");
  ftree->Branch("nuenergy", &fnuenergy, "nuenergy/D");
  ftree->Branch("leppdg", &fleppdg, "leppdg/I");
  ftree->Branch("lepenergy", &flepenergy, "lepenergy/D");
  ftree->Branch("ccnc", &fccnc, "ccnc/I");
  ftree->Branch("mode", &fmode, "mode/I");
  ftree->Branch("interactiontype", &finteractiontype, "interactiontype/I");

  ftree->Branch("nphotons", &fnphotons, "nphotons/I");

  ftree->Branch("dpdg", &fdpdg, "dpdg/I");
  ftree->Branch("denergy", &fdenergy, "denergy/D");

  ftree->Branch("ppdg", &fppdg, "ppdg/I");
  ftree->Branch("penergy", &fpenergy, "penergy/D");
  ftree->Branch("pmomx", &fpmomx, "pmomx/D");
  ftree->Branch("pmomy", &fpmomy, "pmomy/D");
  ftree->Branch("pmomz", &fpmomz, "pmomz/D");
  ftree->Branch("pmom", &fpmom, "pmom/D");
  ftree->Branch("pcth", &fpcth, "pcth/D");

  ftree->Branch("npdg", &fnpdg, "npdg/I");
  ftree->Branch("nenergy", &fnenergy, "nenergy/D");
  ftree->Branch("nmomx", &fnmomx, "nmomx/D");
  ftree->Branch("nmomy", &fnmomy, "nmomy/D");
  ftree->Branch("nmomz", &fnmomz, "nmomz/D");
  ftree->Branch("nmom", &fnmom, "nmom/D");

  ftree->Branch("dm", &fdm, "dm/D");

  this->reconfigure(p);
  
}


void GENIETest::reset() {

  fnupdg = 0;
  fnuenergy = -1;
  fleppdg = 0;
  flepenergy = -1;
  fccnc = -1;
  fmode = -1;
  finteractiontype = -1;

  fnphotons = -1;

  fdpdg = 0;
  fdenergy = -1;

  fppdg = 0;
  fpenergy = -1;
  fpmomx = -10000;
  fpmomy = -10000;
  fpmomz = -10000;
  fpmom = -10000;
  fpcth = -10000;

  fnpdg = 0;
  fnenergy = -1;
  fnmomx = -10000;
  fnmomy = -10000;
  fnmomz = -10000;
  fnmom = -10000;

  fdm = -10000;

}


void GENIETest::cout_particle_list(simb::MCTruth const & mct) {

  if(fverbose) std::cout << __PRETTY_FUNCTION__ << "\n";

  std::cout << "-------------------------------\n";
  for(int j = 0; j < mct.NParticles(); ++j) {
    simb::MCParticle const & mcp = mct.GetParticle(j);
    std::cout << j << " " << mcp.PdgCode() << " " << mcp.Mother() << " " << mcp.StatusCode() << "\n";
  }

}


void GENIETest::fill_dr_vars(art::Event const & e, size_t const mct_index, size_t const dr_delta, size_t const dr_photon, size_t const dr_nucleon) {

  if(fverbose) std::cout << __PRETTY_FUNCTION__ << "\n";

  art::ValidHandle<std::vector<simb::MCTruth>> const & ev_mct =
    e.getValidHandle<std::vector<simb::MCTruth>>("generator");

  simb::MCTruth const & mct = ev_mct->at(mct_index);

  simb::MCParticle const & mcp_dr_delta = mct.GetParticle(dr_delta);

  fdpdg = mcp_dr_delta.PdgCode();
  fdenergy = mcp_dr_delta.E(0);

  simb::MCParticle const & mcp_dr_photon = mct.GetParticle(dr_photon);

  fppdg = mcp_dr_photon.PdgCode();
  fpenergy = mcp_dr_photon.E(0);
  fpmomx = mcp_dr_photon.Px(0);
  fpmomy = mcp_dr_photon.Py(0);
  fpmomz = mcp_dr_photon.Pz(0);
  fpmom = mcp_dr_photon.P(0);

  if(dr_nucleon != SIZE_MAX) {

    simb::MCParticle const & mcp_dr_nucleon = mct.GetParticle(dr_nucleon);
    
    fnpdg = mcp_dr_nucleon.PdgCode();
    fnenergy = mcp_dr_nucleon.E(0);
    fnmomx = mcp_dr_nucleon.Px(0);
    fnmomy = mcp_dr_nucleon.Py(0);
    fnmomz = mcp_dr_nucleon.Pz(0);
    fnmom = mcp_dr_nucleon.P(0);
    
    double const pm = 0.938;
    TVector3 const photon_mom(fpmomx, fpmomy, fpmomz);
    TVector3 const track_mom(fnmomx, fnmomy, fnmomz);
    fdm = sqrt(pm*pm + 2*fpenergy*(fnenergy-photon_mom.Dot(track_mom)/(photon_mom.Mag()*track_mom.Mag())*fnenergy));
    
  }

}


size_t GENIETest::deltarad_filter(art::Event const & e) {

  if(fverbose) std::cout << __PRETTY_FUNCTION__ << "\n";

  art::ValidHandle<std::vector<simb::MCTruth>> const & ev_mct =
    e.getValidHandle<std::vector<simb::MCTruth>>("generator");

  size_t mct_index = SIZE_MAX;
  size_t dr_delta = SIZE_MAX;
  size_t dr_photon = SIZE_MAX;
  size_t dr_nucleon = SIZE_MAX;

  for(size_t i = 0; i < ev_mct->size(); ++i) {

    simb::MCTruth const & mct = ev_mct->at(i);
    if(mct.GetNeutrino().CCNC() != 1) continue;

    if(fverbose) std::cout << "Fill exiting particles\n";
   
    std::vector<size_t> exiting_particles;
    std::vector<size_t> exiting_particle_parents;
    fnphotons = 0;
    for(int j = 0; j < mct.NParticles(); ++j) {
      simb::MCParticle const & mcp = mct.GetParticle(j);
      if(mcp.TrackId() != j) {
	std::cout << "ERROR: " << __LINE__ << " " << __PRETTY_FUNCTION__ << "\nTrackId does not match index\n";
	cout_particle_list(mct);
	exit(1);
      }
      if(mcp.StatusCode() != 1) continue;
      if(mcp.PdgCode() == 22) ++fnphotons;
      exiting_particles.push_back(mcp.TrackId());
      exiting_particle_parents.push_back(mcp.Mother());
    }

    if(fverbose) std::cout << "Fill in nucleus particles\n";

    std::vector<size_t> in_nucleus_particles;
    for(size_t j = 0; j < exiting_particle_parents.size(); ++j) {
      size_t const s = exiting_particle_parents.at(j);
      if(s != SIZE_MAX) {
	simb::MCParticle const & mcp = mct.GetParticle(s);
	if(abs(mcp.PdgCode()) == 2214 || abs(mcp.PdgCode()) == 2114) {
	  if(mct.GetParticle(exiting_particles.at(j)).PdgCode() == 22) {
	    mct_index = i;
	    dr_delta = mcp.TrackId();
	    dr_photon = exiting_particles.at(j);
	  }
	  else if(abs(mct.GetParticle(exiting_particles.at(j)).PdgCode()) == 2212 ||
		  abs(mct.GetParticle(exiting_particles.at(j)).PdgCode()) == 2112) {
	    dr_nucleon = exiting_particles.at(j);
	  }
	}
	in_nucleus_particles.push_back(mcp.Mother());
      }
      else in_nucleus_particles.push_back(SIZE_MAX);
    }
    
    if(fverbose) std::cout << "Check in nucleus particles\n";

    for(size_t j = 0; j < in_nucleus_particles.size(); ++j) {
      size_t const s = in_nucleus_particles.at(j);
      if(s == SIZE_MAX) continue;
      simb::MCParticle const & mcp = mct.GetParticle(s);
      if(abs(mcp.PdgCode()) == 2214 || abs(mcp.PdgCode()) == 2114) {
	if(mct.GetParticle(exiting_particles.at(j)).PdgCode() == 22) {
	  if(dr_delta != SIZE_MAX) {
	    std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
		      << "ERROR: dr_delta already set as: " << dr_delta << " being changed to: " << mcp.TrackId() << "\n";
	    cout_particle_list(mct);
	    exit(1);
	  }
	  mct_index = i;
	  dr_delta = mcp.TrackId();
	  dr_photon = exiting_particles.at(j);
	}
	else if((abs(mct.GetParticle(exiting_particles.at(j)).PdgCode()) == 2212 ||
		 abs(mct.GetParticle(exiting_particles.at(j)).PdgCode()) == 2112) &&
		mct.GetParticle(exiting_particles.at(j)).PdgCode() == mct.GetParticle(exiting_particle_parents.at(j)).PdgCode()) {
	  dr_nucleon = exiting_particles.at(j);
	}
      }
    }

  }

  if(fverbose) std::cout << "Finish ev_mct loop\n";

  if(mct_index != SIZE_MAX) fill_dr_vars(e, mct_index, dr_delta, dr_photon, dr_nucleon);

  return mct_index;

}


void GENIETest::analyze(art::Event const & e) {
  
  if(fverbose) std::cout << __PRETTY_FUNCTION__ << "\n";

  art::ValidHandle<std::vector<simb::MCTruth>> const & ev_mct =
    e.getValidHandle<std::vector<simb::MCTruth>>("generator");
  
  reset();

  size_t deltarad_mct_index = deltarad_filter(e);
  if(deltarad_mct_index == SIZE_MAX) {
    std::cout << "ERROR: " << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
	      << "No deltarad_mct_index found\n";
    for(simb::MCTruth const & mct : *ev_mct)
      cout_particle_list(mct);
    exit(1);
  }
  if(fverbose) std::cout << "deltarad_mct_index: " << deltarad_mct_index << " ev_mct->size(): " << ev_mct->size() << "\n";
  simb::MCNeutrino const & mcn = ev_mct->at(deltarad_mct_index).GetNeutrino();
  
  fnupdg = mcn.Nu().PdgCode();
  fnuenergy = mcn.Nu().Trajectory().E(0);
  fleppdg = mcn.Lepton().PdgCode();
  flepenergy = mcn.Lepton().Trajectory().E(0);
  fccnc = mcn.CCNC();
  fmode = mcn.Mode();
  finteractiontype = mcn.InteractionType();

  ftree->Fill();

}


DEFINE_ART_MODULE(GENIETest)
