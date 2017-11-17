

#ifndef DETECTOROBJECTS_H
#define DETECTOROBJECTS_H

#include <map>

#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/Track.h"

#include "art/Framework/Principal/Handle.h"

#include "../LLBasicTool/GeoAlgo/GeoVector.h"
#include "../LLBasicTool/GeoAlgo/GeoSphere.h"
#include "../LLBasicTool/GeoAlgo/GeoTrajectory.h"
#include "../LLBasicTool/GeoAlgo/GeoCone.h"
#include "../LLBasicTool/GeoAlgo/GeoAlgo.h"
#include "../LLBasicTool/GeoAlgo/GeoAABox.h"


struct DetectorObject {

  size_t const fid;
  size_t const foriginal_index;
  int const freco_type;
  bool fis_associated;
  
  DetectorObject(size_t const id, size_t const original_index, int const reco_type) :
    fid(id),
    foriginal_index(original_index),
    freco_type(reco_type),
    fis_associated(false) {}
  
  virtual ~DetectorObject(){}
  
};


struct Track : public DetectorObject {
  
  geoalgo::Trajectory ftrajectory;
  
  Track(size_t const id, size_t const original_index, int const reco_type, recob::Track const & t) :
    DetectorObject(id, original_index, reco_type) {
    for(size_t i = 0; i < t.NumberTrajectoryPoints(); ++i)
      ftrajectory.push_back(t.LocationAtPoint(i)); 
  }
  
};


struct Shower : public DetectorObject {
  
  geoalgo::Cone fcone;
  
  Shower(size_t const id, size_t const original_index, int const reco_type, recob::Shower const & s) :
    DetectorObject(id, original_index, reco_type) {
    fcone = geoalgo::Cone(s.ShowerStart(),
			  s.Direction(),
			  s.Length(),
			  0);
  }
  
};


class DetectorObjects {

  std::map<size_t, DetectorObject *> fobject_m;
  std::vector<size_t> ftrack_index_v;
  std::vector<size_t> fshower_index_v;
  size_t fobject_id;

  std::map<size_t, size_t> foriginal_track_index_m;
  std::map<size_t, size_t> foriginal_shower_index_m;

public:

  int const ftrack_reco_type;
  int const fshower_reco_type;

  DetectorObjects();

  ~DetectorObjects() {
    for(std::pair<size_t, DetectorObject *> const & p : fobject_m) delete p.second;
  }

  void AddTracks(art::ValidHandle<std::vector<recob::Track>> const & ev_t, bool const track_original_indices = false);
  void AddShowers(art::ValidHandle<std::vector<recob::Shower>> const & ev_s, bool const track_original_indices = false);

  void SetAssociated(size_t const i);
  
  int GetRecoType(size_t const i) const;
  std::vector<size_t> const & GetTrackIndices() const {return ftrack_index_v;}
  std::vector<size_t> const & GetShowerIndices() const {return fshower_index_v;}

  DetectorObject const & GetDetectorObject(size_t const i) const;

  Track & GetTrack(size_t const i);
  Track const & GetTrack(size_t const i) const;

  Shower & GetShower(size_t const i);
  Shower const & GetShower(size_t const i) const;
  
  size_t GetTrackIndexFromOriginalIndex(size_t const i) const;
  size_t GetShowerIndexFromOriginalIndex(size_t const i) const;

};


#endif
