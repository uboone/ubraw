
#ifndef DETECTOROBJECTS_CXX
#define DETECTOROBJECTS_CXX

#include "DetectorObjects.h"


DetectorObjects::DetectorObjects() :
  fobject_id(0),
  ftrack_reco_type(2),
  fshower_reco_type(1){}


void DetectorObjects::AddTracks(art::ValidHandle<std::vector<recob::Track>> const & ev_t,
				bool const track_original_indices) {
  for(size_t i = 0; i < ev_t->size(); ++i) {
    recob::Track const & t = ev_t->at(i);
    fobject_m.emplace(fobject_id, new Track(fobject_id, i, ftrack_reco_type, t)); 
    ftrack_index_v.push_back(fobject_id);
    if(track_original_indices) foriginal_track_index_m.emplace(i, fobject_id);
    ++fobject_id;
  }
}

  
void DetectorObjects::AddShowers(art::ValidHandle<std::vector<recob::Shower>> const & ev_s,
				 bool const track_original_indices) {
  for(size_t i = 0; i < ev_s->size(); ++i) {
    recob::Shower const & s = ev_s->at(i);
    fobject_m.emplace(fobject_id, new Shower(fobject_id, i, fshower_reco_type, s));
    fshower_index_v.push_back(fobject_id);
    if(track_original_indices) foriginal_shower_index_m.emplace(i, fobject_id);
    ++fobject_id;
  }
}  


void DetectorObjects::SetAssociated(size_t const i) {

  auto om_it = fobject_m.find(i);
  
  if(om_it == fobject_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object: " << i << std::endl;
    exit(1);
  }
  
  om_it->second->fis_associated = true;
  
}


int DetectorObjects::GetRecoType(size_t const i) const {

  auto const om_it = fobject_m.find(i);
  
  if(om_it == fobject_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object: " << i << std::endl;
    exit(1);
  }

  return om_it->second->freco_type;

}


DetectorObject const & DetectorObjects::GetDetectorObject(size_t const i) const {

  auto const om_it = fobject_m.find(i);
  
  if(om_it == fobject_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object: " << i << std::endl;
    exit(1);
  }
  
  return *om_it->second;

}


Track & DetectorObjects::GetTrack(size_t const i) {
  
  auto const om_it = fobject_m.find(i);
  
  if(om_it == fobject_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object: " << i << std::endl;
    exit(1);
  }
  
  Track * t = dynamic_cast<Track *>(fobject_m.find(i)->second);
  
  if(!t) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not convert: " << i << std::endl;
    exit(1);
  }
  
  return *t;

}


Track const & DetectorObjects::GetTrack(size_t const i) const {
  
  auto const om_it = fobject_m.find(i);
  
  if(om_it == fobject_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object: " << i << std::endl;
    exit(1);
  }
  
  Track * t = dynamic_cast<Track *>(fobject_m.find(i)->second);
  
  if(!t) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not convert: " << i << std::endl;
    exit(1);
  }
  
  return *t;

}


Shower & DetectorObjects::GetShower(size_t const i) {
  
  auto const om_it = fobject_m.find(i);
  
  if(om_it == fobject_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object: " << i << std::endl;
    exit(1);
  }
  
  Shower * s = dynamic_cast<Shower *>(fobject_m.find(i)->second);
  
  if(!s) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find convert: " << i << std::endl;
    exit(1);
  }
  
  return *s;

}


Shower const & DetectorObjects::GetShower(size_t const i) const {
  
  auto const om_it = fobject_m.find(i);
  
  if(om_it == fobject_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object: " << i << std::endl;
    exit(1);
  }
  
  Shower * s = dynamic_cast<Shower *>(fobject_m.find(i)->second);
  
  if(!s) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find convert: " << i << std::endl;
    exit(1);
  }
  
  return *s;

}


size_t DetectorObjects::GetTrackIndexFromOriginalIndex(size_t const i) const {

  auto om_it = foriginal_track_index_m.find(i);
  
  if(om_it == foriginal_track_index_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object associated to original index: " << i << std::endl;
    exit(1);
  }

  return om_it->second;

}


size_t DetectorObjects::GetShowerIndexFromOriginalIndex(size_t const i) const {

  auto om_it = foriginal_shower_index_m.find(i);
  
  if(om_it == foriginal_shower_index_m.end()) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find object associated to original index: " << i << std::endl;
    exit(1);
  }

  return om_it->second;

}


#endif
