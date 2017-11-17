

#ifndef PARTICLEASSOCIATIONS_H
#define PARTICLEASSOCIATIONS_H

#include "DetectorObjects.h"


class ParticleAssociations;


class ParticleAssociation {
  
  friend ParticleAssociations;

  std::vector<size_t> findices;
  std::vector<geoalgo::Point_t> fvertices;
  geoalgo::Point_t fvertex;
  double fgoodness;
  
  std::multimap<size_t, size_t> fconnected_associations;
  
public:
  
  ParticleAssociation(std::vector<size_t> const & indices,
		      std::vector<geoalgo::Point_t> const & vertices,
		      geoalgo::Point_t const & vertex,
		      double const goodness);

  void AddConnection(size_t const i, size_t const n) {
    fconnected_associations.emplace(i, n);
  }
  
  void AddShower(size_t const n, geoalgo::Point_t const & vert) {
    findices.push_back(n);
    fvertices.push_back(vert);
  }
  
  std::vector<size_t> const & GetObjectIndices() const {
    return findices;
  }
  
  std::vector<geoalgo::Point_t> const & GetVertices() const {
    return fvertices;
  }
  
  geoalgo::Point_t const & GetVertexFromNode(size_t const n) const {
    
    auto const nv_itb = findices.begin();
    auto const nv_ite = findices.end();
    
    return fvertices.at(std::find(nv_itb, nv_ite, n) - nv_itb);
    
  }
  
  geoalgo::Point_t const & GetRecoVertex() const {
    return fvertex;
  }
  
  double GetGoodness() const {
    return fgoodness;
  }
  
  std::multimap<size_t, size_t> const & GetConnections() const {
    return fconnected_associations;
  }
  
  void PrintAssociation() const;
  
};


class ParticleAssociations {

  DetectorObjects fdetos;

  std::vector<ParticleAssociation> fassociations;
  std::vector<size_t> fobject_index_v;
  std::vector<size_t> fassociation_index_vec;
  std::vector<size_t> fignore_association_vec;
  std::vector<size_t> fselected_associations;

  bool fverbose;

public:

  ParticleAssociations();

  DetectorObjects & GetDetectorObjects() {return fdetos;}
  DetectorObjects const & GetDetectorObjects() const {return fdetos;}

  void Reset();

  void AddAssociation(std::vector<size_t> const & nodes,
		      std::vector<geoalgo::Point_t> const & vertices,
		      geoalgo::Point_t const & vertex,
		      double const goodness = 0);
    
  void AddShower(size_t const index,
		 size_t const n,
		 geoalgo::Point_t const & vert);

  std::vector<ParticleAssociation> const & GetAssociations() const {
    return fassociations;
  }

  std::vector<size_t> const & GetObjectIndices() const {
    return fobject_index_v;
  }
  
  std::vector<size_t> const & GetAssociationIndices() const {
    return fassociation_index_vec;
  }

  std::vector<size_t> GetAssociationIndicesFromObject(size_t const n);

  void IgnoreAssociation(size_t const n) {
    fignore_association_vec.push_back(n);
  }
  bool Ignore(size_t const i) const;
  //bool HandleLoop(std::vector<size_t> const & previously_considered);
  void IgnoreThis(size_t const to_ignore, size_t const connected_index, std::vector<size_t> & previously_considered);
  void IgnoreAssociationsConnectedTo(size_t const i);

  void ClearIgnored() {
    fignore_association_vec.clear();
  }

  void PrintAssociations() const;
  void PrintAssociations(std::vector<size_t> const & associations_to_print) const;

  void PrintNodes() const;

  void Test() const;

  void NodeCheck();

  void GetShowerAssociations();

  std::vector<size_t> & GetSelectedAssociations() {return fselected_associations;}
  std::vector<size_t> const & GetSelectedAssociations() const {return fselected_associations;}

  //void DeleteAssociation(size_t const s);

  void SetVerbose(bool const verbose = true) {fverbose = verbose;}

};



#endif
