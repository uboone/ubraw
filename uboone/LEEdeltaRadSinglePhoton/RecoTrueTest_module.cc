////////////////////////////////////////////////////////////////////////
// Class:       RecoTrueTest
// Plugin Type: producer (art v2_05_00)
// File:        RecoTrueTest_module.cc
//
// Generated at Fri Aug 18 08:43:34 2017 by Marco Del Tutto using cetskelgen
// from cetlib version v1_21_00.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "lardata/Utilities/AssociationUtil.h"

// Data product include
#include "larcore/Geometry/Geometry.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/Seed.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/Wire.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#include "lardata/DetectorInfoServices/DetectorClocksService.h"
#include "lardataobj/Simulation/SimChannel.h"
#include "larsim/MCCheater/BackTracker.h"
//#include "MCGhost.h"

#include <memory>

// Algorithms include
#include "RecoTrueHelper.h"

namespace xsecAna {
class RecoTrueTest;
}

class xsecAna::RecoTrueTest : public art::EDProducer {
public:
explicit RecoTrueTest(fhicl::ParameterSet const & p);
// The compiler-generated destructor is fine for non-base
// classes without bare pointers or other resource use.

// Plugins should not be copied or assigned.
RecoTrueTest(RecoTrueTest const &) = delete;
RecoTrueTest(RecoTrueTest &&) = delete;
RecoTrueTest & operator = (RecoTrueTest const &) = delete;
RecoTrueTest & operator = (RecoTrueTest &&) = delete;

// Required functions.
void produce(art::Event & e) override;

private:

std::string _pfp_producer;
std::string _spacepointLabel;
std::string _hitfinderLabel;
std::string _geantModuleLabel;

bool _is_data;
bool _debug;
bool _cosmic_only;
};


xsecAna::RecoTrueTest::RecoTrueTest(fhicl::ParameterSet const & p) {

	_pfp_producer                   = p.get<std::string>("PFParticleProducer");
	_hitfinderLabel                 = p.get<std::string>("HitProducer");
	_geantModuleLabel               = p.get<std::string>("GeantModule");
	_spacepointLabel                = p.get<std::string>("SpacePointProducer");

	_debug                          = p.get<bool>("Debug", true);
	_cosmic_only                    = p.get<bool>("CosmicOnly", false);

}

void xsecAna::RecoTrueTest::produce(art::Event & e)
{
	nue_xsec::recotruehelper _recotruehelper_instance;

	if(_debug) std::cout << "[RecoTrueTest] Starts" << std::endl;
	if(_debug) std::cout << "[RecoTrueTest] event: " << e.id().event() << std::endl;

	std::cout << _pfp_producer << " " << _spacepointLabel << " " << _hitfinderLabel << " " << _geantModuleLabel << "\n";
	_recotruehelper_instance.Configure(e, _pfp_producer, _spacepointLabel, _hitfinderLabel, _geantModuleLabel);

	lar_pandora::MCParticlesToPFParticles matchedMCToPFParticles; // This is a map: MCParticle to matched PFParticle
	lar_pandora::MCParticlesToHits matchedParticleHits;

	_recotruehelper_instance.GetRecoToTrueMatches(matchedMCToPFParticles, matchedParticleHits);

	std::cout << "[RecoTrueTest] Generating " << matchedMCToPFParticles.size() << " MCGhosts." << std::endl;

}

DEFINE_ART_MODULE(xsecAna::RecoTrueTest)
