

#ifndef VERTEXBUILDER_CXX
#define VERTEXBUILDER_CXX

#include "VertexBuilder.h"


VertexBuilder::VertexBuilder() :
  fobject_id(0),
  fstart_prox(-1),
  fshower_prox(-1),
  fcpoa_vert_prox(-1),
  fcpoa_trackend_prox(-1),
  fdetos(nullptr),
  fvbt(nullptr),
  fverbose(true) {}


void VertexBuilder::CheckSetVariables() {

  if(fstart_prox == -1) {
    std::cout << "fstart_prox not set\n";
    exit(1);
  } 

  if(fshower_prox == -1) {
    std::cout << "fshower_prox not set\n";
    exit(1);
  } 

  if(fcpoa_vert_prox == -1) {
    std::cout << "fcpoa_vert_prox not set\n";
    exit(1);
  } 

  if(fcpoa_trackend_prox == -1) {
    std::cout << "fcpoa_trackend_prox not set\n";
    exit(1);
  } 
  
}


void VertexBuilder::Erase(std::multimap<size_t, geoalgo::Point_t const *> & pn,
			  std::multimap<size_t, geoalgo::Point_t const *>::iterator const best_it,
			  geoalgo::Point_t const & sv) {

  size_t const index = best_it->first;    
  pn.erase(best_it);  

  if(fdetos->GetRecoType(index) == fdetos->ftrack_reco_type) {
    auto const pn_it = pn.find(index);
    if(fdetos->GetTrack(index).ftrajectory.Length() < fstart_prox ||
       (pn_it != pn.end() && pn_it->second->Dist(sv) < fstart_prox)) {
      pn.erase(pn_it);
    }
  }  
  
}


void VertexBuilder::AssociateTracks(ParticleAssociations & pas) {

  std::multimap<size_t, geoalgo::Point_t const *> pn;

  for(size_t const i : fdetos->GetTrackIndices()) {

    Track const & t = fdetos->GetTrack(i);
    pn.emplace(t.fid, &t.ftrajectory.front());
    pn.emplace(t.fid, &t.ftrajectory.back());

  }
 
  if(fverbose) {
    
    std::cout << "Track pn size: " << pn.size() << "\n";

    if(pn.size()) {
      
      size_t last_id = pn.end()->first;
      for(auto p : pn) {
	if(last_id != p.first) {
	  std::cout << std::endl << p.first << " "
		    << *p.second;
	}
	else std::cout << " " << *p.second;
	std::cout << fdetos->GetTrack(p.first).ftrajectory.Length();
	last_id = p.first;
      }
    
      std::cout << "\n";
  
    }
        
  }

  if(fverbose)
    std::cout << "wloop, pn.size() == " << pn.size() << std::endl;

  while(pn.size() > 1) {

    if(fverbose)
      std::cout <<  "\twloop start, pn.size() == " << pn.size() << std::endl;

    auto best_m = pn.end();
    auto best_c = best_m;

    double best_dist = fstart_prox;

    if(fverbose)
      std::cout << "\tFind the two closest vertices, pn.size() == "
		<< pn.size() << std::endl;
    
    for(auto m_it = pn.begin(); m_it != pn.end(); ++m_it) {
      
      if(fverbose)
	std::cout << "\t\tMain loop start\n";
      
      size_t const mid = m_it->first;
      geoalgo::Point_t const * mvert = m_it->second;

      if(fverbose)
	std::cout << "\t\tMain object id: " << mid << std::endl;
      
      for(auto c_it = pn.begin(); c_it != pn.end(); ++c_it) {
	
	if(fverbose)
	  std::cout << "\t\t\tComparison object id: " << c_it->first;

	if(c_it->first == mid) {
	  if(fverbose)
	    std::cout << " Skip\n";
	  continue;
	}

	double const dist = c_it->second->Dist(*mvert);

	if(fverbose)
	  std::cout << " dist: " << dist << " best_dist: "
		    << best_dist << std::endl;

	if(dist < best_dist) {
	  if(fverbose)
	    std::cout << "\t\t\t\tAccepted\n";	
	  best_m = m_it;
	  best_c = c_it;
	  best_dist = dist;
	}

      }
  
      if(fverbose)
	std::cout << "\t\tMain loop end\n";

    }

    if(best_m == pn.end()) {
      if(fverbose)
	std::cout << "\tNo match found, ending\n";
      return;
    }

    std::vector<size_t> vc;
    vc.push_back(best_m->first);
    vc.push_back(best_c->first);
    
    std::vector<geoalgo::Point_t> vcp;
    vcp.push_back(*best_m->second);
    vcp.push_back(*best_c->second);
    
    geoalgo::Sphere s(falgo.boundingSphere(vcp));
    
    Erase(pn, best_m, s.Center());
    Erase(pn, best_c, s.Center());      

    if(fverbose)
      std::cout << "\tAttempt to add objects to sphere\n";

    do {

      auto best_o = pn.end();
      double sbest_dist = fstart_prox;
      
      geoalgo::Point_t const & sv = s.Center();

      if(fverbose)
	std::cout << "\t\tFind distance between vertices and sphere centre, pn.size == " << pn.size() << std::endl;

      for(auto o_it = pn.begin(); o_it != pn.end(); ++o_it) {

	if(fverbose)
	  std::cout << "\t\t\tobject id: " << o_it->first;

	if(std::find(vc.begin(), vc.end(), o_it->first) != vc.end()) {
	  if(fverbose)
	    std::cout << " Skip\n";
	  continue;
	}

	double const dist = o_it->second->Dist(sv);

	if(fverbose)
	  std::cout << " dist: " << dist << " sbest_dist: "
		    << sbest_dist << std::endl;
	
	if(dist < sbest_dist) {
	  if(fverbose)
	    std::cout << "\t\t\t\tAccepted\n";
	  best_o = o_it;
	  sbest_dist = dist;
	}

      }

      if(best_o == pn.end()) {
	if(fverbose)
	  std::cout << "\t\tNo match found, end loop\n";
	break;
      }
      
      vc.push_back(best_o->first);
      vcp.push_back(*best_o->second);

      Erase(pn, best_o, *best_o->second);
      
      //s = algo.boundingSphere(vcp);

    } while(true);

    pas.AddAssociation(vc, vcp, s.Center(), falgo.boundingSphere(vcp).Radius());
    
  }
   
  return;
 
}


double VertexBuilder::FindClosestApproach(const geoalgo::HalfLine_t & shr1,
					  const geoalgo::HalfLine_t & shr2,
					  geoalgo::Point_t & vtx) const {
  // Find mininum impact parameter between a two showers
  // flip their directions and look backwards

  // Create a half-line pointing backwards from the shower
  geoalgo::HalfLine_t shr1Bkwd(shr1.Start(), shr1.Dir() * (-1));
  geoalgo::HalfLine_t shr2Bkwd(shr2.Start(), shr2.Dir() * (-1));

  // coordinates for closest points on the two objects
  geoalgo::Point_t PtShr1(3);
  geoalgo::Point_t PtShr2(3);
  double IP = falgo.SqDist(shr1Bkwd, shr2Bkwd, PtShr1, PtShr2);
  // build candidate vertex
  vtx = (PtShr1 + PtShr2) / 2.;

  return sqrt(IP);
}


void VertexBuilder::AssociateShowers(ParticleAssociations & pas) {
  
  std::map<size_t, Shower const *> shower_map;
  
  for(size_t const i : fdetos->GetShowerIndices()) { 
    shower_map.emplace(i, &pas.GetDetectorObjects().GetShower(i));
  }

  std::vector<ParticleAssociation> const & associations =
    pas.GetAssociations();

  while(shower_map.size()) {
    
    if(fverbose)
      std::cout << "\tshower_map wloop, size: " << shower_map.size() << std::endl;

    size_t best_shower_id = SIZE_MAX;
    size_t best_other_id = SIZE_MAX;
    size_t index = SIZE_MAX;
    Double_t best_dist = fshower_prox;
    geoalgo::Point_t best_vert(2000, 2000, 2000);    
    geoalgo::Point_t temp_vert(2000, 2000, 2000);

    for(auto const & c : shower_map) {

      if(fverbose)
	std::cout << "\t\tshower_map primary floop, id: " << c.first << std::endl;
      
      geoalgo::Point_t const & c_start = c.second->fcone.Start();
      geoalgo::Vector_t const & c_dir = c.second->fcone.Dir();

      for(auto const & c2 : shower_map) {

	if(fverbose)
	  std::cout << "\t\t\tshower_map secondary floop, id: " << c2.first
		    << std::endl;
	
	if(c2.first == c.first) {
	  if(fverbose) std::cout << "\t\t\t\tmatching id, continue\n";
	  continue;
	}
	
	double dist = FindClosestApproach(c2.second->fcone, c.second->fcone, temp_vert);

	if(fverbose)
	  std::cout << "\t\t\tdist: " << dist << " < best-dist: "
		    << best_dist << " ?\n";

	double temp_dist = best_dist;

	if(dist < temp_dist) {
	  	  
	  if(fverbose) std::cout << "\t\t\t\tyes\n";

	  best_shower_id = c.first;
	  best_other_id = c2.first;
	  best_vert = temp_vert;
	  best_dist = dist;
	  index = SIZE_MAX;
	      
	}

      }

      if(fverbose) std::cout << "\t\tshower_map secondary floop end\n";

      for(size_t const i : fdetos->GetTrackIndices()) {

	if(fverbose)
	  std::cout << "\t\t\ttrack secondary floop, id: " << i << std::endl;

	Track const & t = fdetos->GetTrack(i);
	
	geoalgo::Point_t dont_care;
 
	Double_t dist =
	  sqrt(falgo.SqDist(t.ftrajectory, 
			    geoalgo::HalfLine(c_start,
					      c_dir*-1),
			    temp_vert,
			    dont_care));

	if(fverbose)
	  std::cout << "\t\t\tdist: " << dist << " < best_dist: "
		    << best_dist << " ?\n";

	if(dist < best_dist) {
	  
	  if(fverbose) std::cout << "\t\t\t\tyes\n";

	  best_shower_id = c.first;
	  best_other_id = i;
	  best_vert = temp_vert;
	  best_dist = dist;
	  index  = SIZE_MAX;
	  
	}

      }

      if(fverbose) std::cout << "\t\ttrack secondary floop end\n";

      for(size_t i = 0; i < associations.size(); ++i) {

	if(fverbose)
	  std::cout << "\t\t\tassociation secondary floop, index: "
		    << i << std::endl;

	ParticleAssociation const & pa = associations.at(i);
	
	double dist =
	  sqrt(falgo.SqDist(pa.GetRecoVertex(),
			    geoalgo::HalfLine(c_start,
					      c_dir*-1)));

	if(fverbose)
	  std::cout << "\t\t\tdist: " << dist << " < best-dist: "
		    << best_dist << " ?\n";
	
	if(dist < best_dist) {
	
	  if(fverbose) std::cout << "\t\t\t\tyes\n";
  
	  best_shower_id = c.first;
	  best_other_id = SIZE_MAX;
	  best_vert = falgo.ClosestPt(pa.GetRecoVertex(), c.second->fcone);
	  best_dist = dist;
	  index = i;

	}
	
      }

      if(fverbose) std::cout << "\t\tassociation secondary floop end\n";

    }

    if(fverbose) std::cout << "\tshower_map primary floop end\n"
			   << "\tbest_dist: " << best_dist << " >= "
			   << fshower_prox << " ?\n";

    if(best_dist >= fshower_prox) {
      if(fverbose) std::cout << "\t\tyes, return\n";
      return;
    }

    if(fverbose) std::cout << "\tbest_shower_id: " << best_shower_id
			   << " best_dist: " << best_dist
			   << "\n\tindex: " << index << " == -1 ?\n";
    
    if(index == SIZE_MAX) {

      if(fverbose) std::cout << "\t\tyes\n";

      size_t association_index = SIZE_MAX;
      double best_association_dist = fcpoa_vert_prox;
      
      if(fverbose)
	std::cout << "\t\tassociation floop, size: "
		  << associations.size() << std::endl;

      for(size_t i = 0; i < associations.size(); ++i) {
	
	if(fverbose)
	  std::cout << "\t\t\tassociation floop, index: "
		    << i << std::endl;

	double const dist =
	  best_vert.Dist(associations.at(i).GetRecoVertex());
	
	if(fverbose)
	  std::cout << "\t\t\tdist: " << dist
		    << " < best_association_dist: "
		    << best_association_dist << std::endl;

	if(dist < best_association_dist) {
	  
	  if(fverbose)
	    std::cout << "\t\t\t\tyes\n";

	  association_index = i;
	  best_association_dist = dist;
	  
	}
	
      }

      int const reco_type = fdetos->GetRecoType(best_other_id);
      
      if(fverbose) std::cout << "\t\tOther reco type:\n";

      if(reco_type == fdetos->fshower_reco_type) {

	if(fverbose)
	  std::cout << "\t\t\tshower\n"
		    << "\t\t\tbest_association_dist: "
		    << best_association_dist
		    << " < fcpoa_vert_prox: "
		    << fcpoa_vert_prox << " ?\n";

	if(best_association_dist < fcpoa_vert_prox) {
	  if(fverbose) std::cout << "\t\t\t\tyes, add showers to association: "
				 << association_index << std::endl;
	  pas.AddShower(association_index, best_shower_id, best_vert);
	  pas.AddShower(association_index, best_other_id, best_vert);
	}

	else {

	  size_t best_track = SIZE_MAX;
	  geoalgo::Point_t const * best_tp = nullptr;
	  geoalgo::Point_t const * best_other_tp = nullptr;
	  double best_showerp_dist = fcpoa_vert_prox;

	  for(size_t const i : fdetos->GetTrackIndices()) {

	    geoalgo::Trajectory const & t = fdetos->GetTrack(i).ftrajectory;

	    double const trackend_dist = t.back().Dist(best_vert);

	    if(trackend_dist < best_showerp_dist) {
	      best_track = i;
	      best_tp = &t.back();
	      best_other_tp = &t.front();
	      best_showerp_dist = trackend_dist;
	    }
	       
	    double const trackstart_dist = t.front().Dist(best_vert);
	    if(trackstart_dist < best_showerp_dist) {
	      best_track = i;
	      best_tp = &t.front();
	      best_other_tp = &t.back();
	      best_showerp_dist = trackstart_dist;
	    }
	          
	  }

	  if(best_showerp_dist < fcpoa_vert_prox) {

	    std::vector<size_t> const & index_positions =
	      pas.GetAssociationIndicesFromObject(best_track);
	          
	    if(index_positions.size() == 0) {

	      if(fverbose) std::cout << "\t\t\t\tno, create new association\n";

	      std::vector<size_t> showers;
	      showers.push_back(best_shower_id);
	      showers.push_back(best_other_id);
	      showers.push_back(best_track);
	      std::vector<geoalgo::Point_t> verts(2, best_vert);
	      if(best_tp == nullptr) std::cout << "best_tp == nullptr\n";
	      verts.push_back(*best_tp);
	      pas.AddAssociation(showers,
				 verts,
				 geoalgo::Sphere(*best_tp, best_dist).Center(),
				 best_dist);
	      
	    }
	    
	    else if(index_positions.size() == 1) {

	      size_t const index =
		pas.GetAssociationIndices().at(index_positions.front());
	      
	      geoalgo::Point_t const & added_point =
		associations.at(index).GetVertexFromNode(best_track);
	      
	      double const point_dist = added_point.Dist(*best_tp);
	      double const otherpoint_dist = added_point.Dist(*best_other_tp);
	            
	      if(otherpoint_dist < point_dist) {
		  
		if(associations.at(index).GetRecoVertex().
		   Dist(*best_tp) < fstart_prox) {
		  pas.AddShower(index, best_shower_id, best_vert);
		  pas.AddShower(index, best_other_id, best_vert);
		}

		else {

		  if(fverbose) std::cout << "\t\t\t\tno, create new association\n";

		  std::vector<size_t> showers;
		  showers.push_back(best_shower_id);
		  showers.push_back(best_other_id);
		  showers.push_back(best_track);
		  std::vector<geoalgo::Point_t> verts(2, best_vert);
		  if(best_tp == nullptr) std::cout << "best_tp == nullptr\n";
		  verts.push_back(*best_tp);
		  pas.AddAssociation(showers,
				     verts,
				     geoalgo::Sphere(*best_tp, best_dist).Center(),
				     best_dist);
		  
		}
		  
	      }
	      
	      else {
		pas.AddShower(index, best_shower_id, best_vert);
		pas.AddShower(index, best_other_id, best_vert);
	      }

	    }

	    else if(index_positions.size() == 2) {
	      
	      size_t const indexa =
		pas.GetAssociationIndices().at(index_positions.front());
	      double dista = 
		associations.at(indexa).GetRecoVertex().Dist(best_vert);
	      
	      size_t const indexb =
		pas.GetAssociationIndices().at(index_positions.back());   
	      double distb = 
		associations.at(indexb).GetRecoVertex().Dist(best_vert);
	      
	      if(dista < distb) {
		pas.AddShower(indexa, best_shower_id, best_vert);
		pas.AddShower(indexa, best_other_id, best_vert);
	      }

	      else {
		pas.AddShower(indexb, best_shower_id, best_vert);      
		pas.AddShower(indexb, best_other_id, best_vert);
	      }

	    }
	         
	  }

	  else {

	    std::vector<size_t> showers;
	    showers.push_back(best_shower_id);
	    showers.push_back(best_other_id);
	    std::vector<geoalgo::Point_t> verts(2, best_vert);
	    pas.AddAssociation(showers,
			       verts,
			       geoalgo::Sphere(best_vert, best_dist).Center(),
			       best_dist);
	    
	  }

	}

	if(fverbose)
	  std::cout << "\t\t\terase other: " << best_other_id << std::endl;
	
	shower_map.erase(best_other_id);

      }

      else if(reco_type == fdetos->ftrack_reco_type) {
		  
	if(fverbose) std::cout << "\t\t\ttrack\n";

	geoalgo::Trajectory const & t = fdetos->GetTrack(best_other_id).ftrajectory;
	  
	double best_trackend_dist = t.front().Dist(best_vert);
	geoalgo::Point_t const * point = &t.front();
	geoalgo::Point_t const * otherpoint = &t.back();

	double const trackend_dist = t.back().Dist(best_vert);
	if(trackend_dist < best_trackend_dist) {
	  best_trackend_dist = trackend_dist;
	  point = &t.back();
	  otherpoint = &t.front();
	}
	  
	if(fverbose)
	  std::cout << "\t\t\tbest_trackend_dist: "
		    << best_trackend_dist
		    << " < fcpoa_vert_prox: "
		    << fcpoa_vert_prox << " ?\n";

	if(best_trackend_dist < fcpoa_trackend_prox) {

	  std::vector<size_t> const index_positions =
	    pas.GetAssociationIndicesFromObject(best_other_id);
	       	      
	  if(fverbose)
	    std::cout << "\t\t\t\tyes\n"
		      << "\t\t\t\tindex_positions.size(): "
		      << index_positions.size() << std::endl;

	  if(index_positions.size() == 0) {

	    if(fverbose)
	      std::cout << "\t\t\t\t\tsize 0\n";

	    std::vector<size_t> objects;
	    objects.push_back(best_shower_id);
	    objects.push_back(best_other_id);
	    std::vector<geoalgo::Point_t> verts;
	    verts.push_back(best_vert);
	    verts.push_back(*point);
	    pas.AddAssociation(objects,
			       verts,
			       geoalgo::Sphere(*point, best_dist).Center(),
			       best_dist);      
	    

	  }

	  else if(index_positions.size() == 1) {
	    
	    if(fverbose)
	      std::cout << "\t\t\t\t\tsize 1\n";

	    size_t const index =
	      pas.GetAssociationIndices().at(index_positions.front());
	    
	    geoalgo::Point_t const & added_point =
	      associations.at(index).GetVertexFromNode(best_other_id);
	    
	    double const point_dist = added_point.Dist(*point);
	    double const otherpoint_dist = added_point.Dist(*otherpoint);
	    
	    if(fverbose)
	      std::cout << "\t\t\t\t\totherpoint_dist: "
			<< otherpoint_dist
			<< " < point_dist: "
			<< point_dist << " ?\n";

	    if(otherpoint_dist < point_dist) {

	      if(fverbose)
		std::cout << "\t\t\t\t\t\tyes\n"
			  << "\t\t\t\t\t\tcenter_point_dist: "
			  << associations.at(index).GetRecoVertex().Dist(*point)
			  << " < fstart_prox: " << fstart_prox << " ?\n";

	      if(associations.at(index).GetRecoVertex().
		 Dist(*point) < fstart_prox) {
		if(fverbose) std::cout << "\t\t\t\t\t\t\tyes\n";
		pas.AddShower(index, best_shower_id, best_vert);
	      }
	      
	      else {
		
		std::vector<size_t> objects;
		objects.push_back(best_shower_id);
		objects.push_back(best_other_id);
		std::vector<geoalgo::Point_t> verts;
		verts.push_back(best_vert);
		verts.push_back(*point);
		pas.AddAssociation(objects,
				   verts,
				   geoalgo::Sphere(*point, best_dist).Center(),
				   best_dist);      
		
	      }
	      
	    }
	    
	    else {

	      if(fverbose)
		std::cout << "\t\t\t\t\t\tno\n"
			  << "\t\t\t\t\t\tadd id: " << best_shower_id
			  << " to association: " << index << std::endl;

	      pas.AddShower(index, best_shower_id, best_vert); 

	    }
	    
	  }
	  
	  else if(index_positions.size() == 2) {

	    if(fverbose)
	      std::cout << "\t\t\t\t\tsize 2\n";
	          
	    size_t const indexa =
	      pas.GetAssociationIndices().at(index_positions.front());
	    double dista = 
	      associations.at(indexa).GetRecoVertex().Dist(best_vert);
	    
	    size_t const indexb = pas.GetAssociationIndices().at(index_positions.back());   
	    double distb = 
	      associations.at(indexb).GetRecoVertex().Dist(best_vert);
	    
	    if(dista < distb) {
	      pas.AddShower(indexa,
			    best_shower_id,
			    best_vert);
	    }
	    
	    else {
	      pas.AddShower(indexb,
			    best_shower_id,
			    best_vert);
	    }	    

	  }
	  
	  else if(index_positions.size() > 2)
	    std::cout << "Warning: more than two indices found, node: "
		      << best_other_id << std::endl;
	  
	}
	
	else {

	  if(fverbose) std::cout << "\t\t\t\tno\n";

	  pas.GetDetectorObjects().SetAssociated(best_shower_id);

	}
	
      }
      
    }

    else {
      pas.AddShower(index, best_shower_id, best_vert);
    }    

    shower_map.erase(best_shower_id);

  }
  
}


void VertexBuilder::AddLoneTracks(ParticleAssociations & pas) {

  for(size_t const gn : fdetos->GetTrackIndices()) {
      
    Track const & t = fdetos->GetTrack(gn);
    
    if(t.fis_associated) continue;
    
    geoalgo::Point_t const * track_end = nullptr;
    double zmin = 2000;

    geoalgo::Point_t const & front = t.ftrajectory.front();
    if(front.at(2) < zmin) {
      track_end = &front;
      zmin = front.at(2);
    }

    geoalgo::Point_t const & back = t.ftrajectory.back();
    if(back.at(2) < zmin) {
      track_end = &back;
      zmin = back.at(2);
    }
    
    if(track_end) {
      
      pas.AddAssociation(std::vector<size_t>(1, gn),
			 std::vector<geoalgo::Point_t>(1, *track_end),
			 geoalgo::Sphere(*track_end, 0).Center(),
			 0);
      
    }
      
    else std::cout << "Warning: No track end pointer\n";
      
  }

}


void VertexBuilder::AddLoneShowers(ParticleAssociations & pas) {

  for(size_t const gn : fdetos->GetShowerIndices()) {
      
    Shower const & s = fdetos->GetShower(gn);
      
    if(s.fis_associated) continue;

    geoalgo::Point_t const p = s.fcone.Start();

    pas.AddAssociation(std::vector<size_t>(1, gn),
		       std::vector<geoalgo::Point_t>(1, p),
		       geoalgo::Sphere(p, 0).Center(),
		       0);
    
  }

}


void VertexBuilder::FillVBT(ParticleAssociations & pas) {
  
  fvbt->ftrack_number = fdetos->GetTrackIndices().size();
  fvbt->fshower_number = fdetos->GetShowerIndices().size();
  fvbt->ftree->Fill();

}


void VertexBuilder::Run(ParticleAssociations & pas) {
  
  CheckSetVariables();

  fdetos = &pas.GetDetectorObjects();

  if(fverbose) std::cout << "Associate tracks\n";
  AssociateTracks(pas);
  if(fvbt) fvbt->fassociation_track_number = pas.GetAssociations().size();
  if(fverbose) std::cout << "Associate showers\n";
  AssociateShowers(pas);
  if(fvbt) fvbt->fassociation_shower_number = pas.GetAssociations().size();

  if(fverbose) std::cout << "Add lone tracks\n";
  AddLoneTracks(pas);
  if(fverbose) std::cout << "Add lone showers\n";
  AddLoneShowers(pas);
  if(fverbose) std::cout << "Get shower associations\n";
  pas.GetShowerAssociations();
  if(fvbt) fvbt->fassociation_final_number = pas.GetSelectedAssociations().size();
  
  if(fvbt) {
    if(fverbose) std::cout << "Fill VBT\n";
    FillVBT(pas);
  }

  pas.NodeCheck();

}


#endif
