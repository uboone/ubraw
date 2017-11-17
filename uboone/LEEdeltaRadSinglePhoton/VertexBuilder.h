

#ifndef VERTEXBUILDER_H
#define VERTEXBUILDER_H

#include "ParticleAssociations.h"

#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"

#include "TTree.h"


struct VertexBuilderTree {

  TTree * ftree;
  int frun_number;
  int fsubrun_number;
  int fevent_number;
  int ftrack_number;
  int fshower_number;
  int fassociation_track_number;
  int fassociation_shower_number;
  int fassociation_final_number;
  
  VertexBuilderTree() :
    ftree(nullptr){}

  void Setup() {
    art::ServiceHandle< art::TFileService > tfs;
    ftree = tfs->make<TTree>("VertexBuilder", "");
    ftree->Branch("run_number", &frun_number, "run_number/I");
    ftree->Branch("subrun_number", &fsubrun_number, "subrun_number/I");
    ftree->Branch("event_number", &fevent_number, "event_number/I");  
    ftree->Branch("track_number", &ftrack_number, "track_number/I");
    ftree->Branch("shower_number", &fshower_number, "shower_number/I");
    ftree->Branch("association_track_number", &fassociation_track_number, "association_track_number/I");
    ftree->Branch("association_shower_number", &fassociation_shower_number, "association_shower_number/I");
    ftree->Branch("association_final_number", &fassociation_final_number, "association_final_number/I");
  }

};


class VertexBuilder {

  geoalgo::GeoAlgo const falgo;

  size_t fobject_id;

  double fstart_prox;
  double fshower_prox;
  double fcpoa_vert_prox; 
  double fcpoa_trackend_prox;

  DetectorObjects const * fdetos;

  VertexBuilderTree * fvbt;

  bool fverbose;

  void CheckSetVariables();

  void Erase(std::multimap<size_t, geoalgo::Point_t const *> & pn,
	     std::multimap<size_t, geoalgo::Point_t const *>::iterator const best_it,
	     geoalgo::Point_t const & sv);
  
  void AssociateTracks(ParticleAssociations & pas);
  double FindClosestApproach(const geoalgo::HalfLine_t & shr1,
			     const geoalgo::HalfLine_t & shr2,
			     geoalgo::Point_t & vtx) const;
  void AssociateShowers(ParticleAssociations & pas);
  void AddLoneTracks(ParticleAssociations & pas);
  void AddLoneShowers(ParticleAssociations & pas);
  void FillVBT(ParticleAssociations & pas);

public:

  VertexBuilder();

  void SetVerbose(bool const verbose = true) {
    fverbose = verbose;
  }

  void SetMaximumTrackEndProximity(double const start_prox) {
    fstart_prox = start_prox;
  }

  void SetMaximumShowerIP(double const shower_prox) {
    fshower_prox = shower_prox;
  }

  void CPOAToVert(double const cpoa_vert_prox) {
    fcpoa_vert_prox = cpoa_vert_prox;
  }

  void SetMaximumTrackEndProx(double const cpoa_trackend_prox) {
    fcpoa_trackend_prox = cpoa_trackend_prox;
  }

  void SetVBT(VertexBuilderTree * vbt) {fvbt = vbt;}

  void AddTracks(art::ValidHandle<std::vector<recob::Track>> const & ev_t);
  void AddShowers(art::ValidHandle<std::vector<recob::Shower>> const & ev_s);

  void Run(ParticleAssociations & pas);

};


#endif
