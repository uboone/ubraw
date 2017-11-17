

#ifndef PARTICLEASSOCIATIONS_CXX
#define PARTICLEASSOCIATIONS_CXX

#include "ParticleAssociations.h"


ParticleAssociation::ParticleAssociation(std::vector<size_t> const & indices,
					 std::vector<geoalgo::Point_t> const & vertices,
					 geoalgo::Point_t const & vertex,
					 Double_t const goodness) :
  findices(indices),
  fvertices(vertices),
  fvertex(vertex),
  fgoodness(goodness){}


void ParticleAssociation::PrintAssociation() const {
  
  for(size_t i = 0; i < findices.size(); ++i)
    std::cout << "Object index: " << findices.at(i) << std::endl;
  
  std::cout << "\nVertex: " << fvertex << ", goodness: "
	    << fgoodness << std::endl;
  
  std::cout << "\nConnections:\n";
  for(std::pair<size_t, size_t> const & pair : fconnected_associations) 
    std::cout << "Association index: " << pair.first << " object index: " << pair.second << std::endl;
  
}


ParticleAssociations::ParticleAssociations() :
  fverbose(false){}


void ParticleAssociations::Reset() {

  fassociations.clear();
  fobject_index_v.clear();
  fassociation_index_vec.clear();

}


void ParticleAssociations::AddAssociation(std::vector<size_t> const & nodes,
					  std::vector<geoalgo::Point_t> const & vertices,
					  geoalgo::Point_t const & vertex,
					  Double_t const goodness) {
  
  fassociations.push_back(ParticleAssociation(nodes, vertices, vertex, goodness));
  
  size_t const pos = fassociations.size() - 1;
  
  for(size_t const n : nodes) {
    
    auto const nv_itb = fobject_index_v.begin();
    auto const nv_ite = fobject_index_v.end();
    
    for(auto nv_it = nv_itb; nv_it != nv_ite; ++nv_it) {
      
      nv_it = std::find(nv_it, nv_ite, n);
      
      if(nv_it != nv_ite) {
	
	size_t const index = fassociation_index_vec.at(nv_it - nv_itb);
	
	fassociations.at(index).AddConnection(pos, n);
	fassociations.at(pos).AddConnection(index, n);
	
      }
      
      else break;
      
    }
    
    fobject_index_v.push_back(n);
    fassociation_index_vec.push_back(pos);
    
  }      

  for(size_t const i : nodes) {  
    fdetos.SetAssociated(i);
  }

  
}


void ParticleAssociations::AddShower(size_t const index,
				     size_t const n,
				     geoalgo::Point_t const & vert) {
  
  if(index > fassociations.size() - 1 || index < 0) {
    std::cout << "No association with index: " << index << std::endl;
    return;
  }
  
  ParticleAssociation & association = fassociations.at(index);
  std::vector<size_t> const & group = association.GetObjectIndices();
  
  if(std::find(group.begin(), group.end(), n) != group.end()) {
    std::cout << "Object ID: " << n << " already added\n";
    return;
  }
  
  fobject_index_v.push_back(n);
  fassociation_index_vec.push_back(index);
  
  association.AddShower(n, vert);

  fdetos.SetAssociated(n);
  
}


std::vector<size_t> ParticleAssociations::GetAssociationIndicesFromObject(size_t const n) {

  auto const nv_itb = fobject_index_v.begin();
  auto const nv_ite = fobject_index_v.end();
  auto nv_it = nv_itb;
  
  std::vector<size_t> indices;
  
  while(nv_it != nv_ite) {
    
    nv_it = std::find(nv_it, nv_ite, n);
    
    if(nv_it != nv_ite) {
      indices.push_back(nv_it - nv_itb);
      ++nv_it;
    }
    
  }
  
  return indices;
  
}


bool ParticleAssociations::Ignore(size_t const i) const {
  auto const sk_it =
    std::find(fignore_association_vec.begin(), fignore_association_vec.end(), i);
  if(sk_it == fignore_association_vec.end()) return false;
  else return true;
}


/*
bool ParticleAssociations::HandleLoop(std::vector<size_t> const & previously_considered) {

  if(fverbose) std::cout << "Handle loop\n";

  size_t delete_association = SIZE_MAX;
  double largest_sphere = -1;
  for(size_t const s : previously_considered) {
    ParticleAssociation const & pa = fassociations.at(s);
    if(pa.GetObjectIndices().size() == 2 && pa.GetConnections().size() == 2 && pa.GetGoodness() > largest_sphere) {
      delete_association = s;
      largest_sphere = pa.GetGoodness();
    }
  }

  if(delete_association == SIZE_MAX) return false;
  DeleteAssociation(delete_association);
  return true;

}
*/


void ParticleAssociations::IgnoreThis(size_t const to_ignore, size_t const connected_index, std::vector<size_t> & previously_considered) {

  if(std::find(previously_considered.begin(), previously_considered.end(), to_ignore) != previously_considered.end()) return;
  
  previously_considered.push_back(to_ignore);
  ParticleAssociation const & pa = fassociations.at(to_ignore);
  fignore_association_vec.push_back(to_ignore);
  
  for(std::pair<size_t, size_t> const & p : pa.GetConnections()) {
    if(p.first == connected_index) continue;
    IgnoreThis(p.first, to_ignore, previously_considered);
  }

}


void ParticleAssociations::IgnoreAssociationsConnectedTo(size_t const i) {
  
  ParticleAssociation const & pa = fassociations.at(i);
  
  for(std::pair<size_t, size_t> const & p : pa.GetConnections()) {
    std::vector<size_t> previously_considered;  
    IgnoreThis(p.first, i, previously_considered);
  }
  
}


void ParticleAssociations::PrintAssociations() const {

  std::cout << "----------------------------------------\n\n";
  
  for(size_t i = 0; i < fassociations.size(); ++i) {
 
    std::cout << "Association: " << i << std::endl;
    
    fassociations.at(i).PrintAssociation();
    
    std::cout << std::endl;
    
  }
  
  std::cout << std::endl;
  
}


void ParticleAssociations::PrintAssociations(std::vector<size_t> const & associations_to_print) const {

  std::cout << "----------------------------------------\n\n";
  
  for(size_t i = 0; i < fassociations.size(); ++i) {
    
    if(std::find(associations_to_print.begin(), associations_to_print.end(), i) == associations_to_print.end()) continue;

    std::cout << "Association: " << i << std::endl;
    
    fassociations.at(i).PrintAssociation();
    
    std::cout << std::endl;
    
  }
  
  std::cout << std::endl;
  
}


void ParticleAssociations::PrintNodes() const  {
  
  std::cout << "Nodes:\n";
  
  for(size_t i = 0; i < fobject_index_v.size(); ++i)
    std::cout << "Nodes: " << fobject_index_v.at(i) << " Index: "
	      << fassociation_index_vec.at(i) << std::endl;
  
}


void ParticleAssociations::Test() const {

  for(ParticleAssociation const & pae : fassociations) {
    
    for(std::pair<size_t, size_t> const & pair : pae.GetConnections()) {
      
      size_t const i = pair.first;
      
      std::vector<size_t> const & c = fassociations.at(i).GetObjectIndices();
      auto c_itb = c.begin();
      auto c_ite = c.end();
      
      auto c_it = c_ite;
      
      for(size_t const n : pae.GetObjectIndices()) {
	
	c_it = std::find(c_itb, c_ite, n);
	
	if(c_it != c_ite) break;
	
      }
      
      if(c_it == c_ite) std::cout << "Complain\n";
      
    }
    
  }
  
}


void ParticleAssociations::NodeCheck() {

  std::vector<size_t> nodes;

  for(size_t const n : fobject_index_v) {
    if(nodes.size() < n+1) nodes.resize(n+1, 0);    
    ++nodes.at(n);
  }

  for(size_t i = 0; i < nodes.size(); ++i) {

    size_t const s = nodes.at(i);
    if(s == 0) continue;

    DetectorObject const & deto = fdetos.GetDetectorObject(i);

    if(deto.freco_type == fdetos.ftrack_reco_type && s > 2)
      std::cout << "track > 2 entries: " << s << std::endl;
    if(deto.freco_type == fdetos.fshower_reco_type && s > 1)
      std::cout << "shower > 1 entries: " << s << std::endl;

  }
  
}


void ParticleAssociations::GetShowerAssociations() {

  if(fverbose) std::cout << "GetShowerAssociations\n";

  std::multimap<double, size_t> pa_map;

  if(fverbose) std::cout << "Number of particle associations: " << fassociations.size() << "\n";

  for(size_t i = 0; i < fassociations.size(); ++i) {
    if(fverbose) std::cout << "\tAssociation: " << i << "\n";
    ParticleAssociation const & pa = fassociations.at(i);
    bool asso_shower = false;
    for(size_t const s : pa.GetObjectIndices()) {
      if(fverbose) std::cout << "\t\tObject index: " << s << "\n";
      if(fdetos.GetRecoType(s) == fdetos.fshower_reco_type) {
	if(fverbose) std::cout << "\t\tis shower\n";
	asso_shower = true;
	break;
      }
    }
    if(asso_shower) {
      pa_map.emplace(pa.GetRecoVertex().at(2), i);
    }
  }
  
  if(fverbose) std::cout << "Loop over filled map\n";

  for(std::pair<double, size_t> const & p : pa_map) {
    if(fverbose) std::cout << "\tAssociation: " << p.second << " " << "z-position: " << p.first << "\n";
    if(Ignore(p.second)) continue;
    IgnoreAssociationsConnectedTo(p.second);
    fselected_associations.push_back(p.second);
  }

  if(fverbose) std::cout << "ClearIgnored\n";

  ClearIgnored();

}


/*
void ParticleAssociations::DeleteAssociation(size_t const s) {
  
  if(fverbose) {
    std::cout << "Delete association " << s << "\nDelete other association connections to this association\n";
  }

  std::vector<size_t> considered_connected_associations;
  for(std::pair<size_t, size_t> const & p : fassociations.at(s).GetConnections()) {  
    if(std::find(considered_connected_associations.begin(), considered_connected_associations.end(), p.first) != considered_connected_associations.end()) continue;
    std::multimap<size_t, size_t> & connections = fassociations.at(p.first).fconnected_associations;
    auto c_it = connections.find(s);
    if(c_it == connections.end()) {
      std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\nNo connections found\n";
      exit(1);
    }
    while(c_it != connections.end()) {
      connections.erase(c_it);
      c_it = connections.find(s);
    }
    considered_connected_associations.push_back(p.first);
  }

  if(fverbose) std::cout << "Delete association in ParticleAssociations vector\n";

  fassociations.erase(fassociations.begin()+s);

  if(fverbose) std::cout << "Delete ParticleAssociations bookeeping of association and object indices\n";

  auto av_it = std::find(fassociation_index_vec.begin(), fassociation_index_vec.end(), s);
  while(av_it != fassociation_index_vec.end()) {
    fobject_index_v.erase(fobject_index_v.begin()+size_t(av_it-fassociation_index_vec.begin()));
    fassociation_index_vec.erase(av_it);
  }

  if(fverbose) {
    std::cout << "End delete association\n";
  }
    
}
*/


#endif
