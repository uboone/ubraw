// Framework includes
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "canvas/Persistency/Common/FindOneP.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "fhiclcpp/ParameterSet.h"


// LArSoft Includes
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h" // raw::ChannelID_t
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/CryostatGeo.h"
#include "larcore/Geometry/TPCGeo.h"
#include "larcore/Geometry/PlaneGeo.h"
#include "lardataobj/RecoBase/Wire.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardata/ArtDataHelper/HitCreator.h"

#include "waveform_tools.h"

#include "TTree.h"
#include "SnHitCompare_classes.h"

namespace sn{

  

class SnHitCompare : public art::EDAnalyzer {
  
public:
  explicit SnHitCompare(fhicl::ParameterSet const& pset); 
       
  void reconfigure(fhicl::ParameterSet const& pset);
  void analyze(const art::Event& evt);


protected:
  std::string fHit1;
  std::string fHit2;
  
  TTree* fTree;
  tuple_entry* fEntry;
  
  TTree* fTree_unmatched1;
  tuple_entry* fEntry_unmatched1;
  
  TTree* fTree_unmatched2;
  tuple_entry* fEntry_unmatched2;
  
  
}; // class GausHitFinder


// ctor
SnHitCompare::SnHitCompare(fhicl::ParameterSet const& pset)    :
    EDAnalyzer(pset)  
{
  reconfigure(pset);
  
  art::ServiceHandle<art::TFileService> tfs;
  fTree = tfs->make<TTree>("hits","hits");
  fEntry = new tuple_entry;
  fTree->Branch("p","sn::tuple_entry",&fEntry);

  fTree_unmatched1 = tfs->make<TTree>("unmatched1","Unmatched Hits 1");
  fEntry_unmatched1 = new tuple_entry;
  fTree_unmatched1->Branch("p","sn::tuple_entry",&fEntry_unmatched1);
  
  fTree_unmatched2 = tfs->make<TTree>("unmatched2","Unmatched Hits 2");
  fEntry_unmatched2 = new tuple_entry;
  fTree_unmatched2->Branch("p","sn::tuple_entry",&fEntry_unmatched2);
}


void SnHitCompare::reconfigure(fhicl::ParameterSet const& p)
{
  fHit1 = p.get< std::string  >("Hit1","sndaq");
  fHit2 = p.get< std::string  >("Hit2","gaus");
}


/////////////////////////////////////////////////
/// The meat //////

// jobstartend
void SnHitCompare::analyze(const art::Event& evt)
{
  
  art::Handle< std::vector<recob::Hit> > hits1;
  art::Handle< std::vector<recob::Hit> > hits2;
  evt.getByLabel(fHit1,hits1);
  evt.getByLabel(fHit2,hits2);
  
  // sort by wire to ease computational time.
  typedef std::map<geo::WireID, std::vector<art::Ptr<recob::Hit>> > wire_to_hit_t;
  wire_to_hit_t wire_to_hit;
  
  std::vector<size_t> hitlist1_matches(hits1->size());
  std::vector<size_t> hitlist2_matches(hits2->size());

  
  for(size_t ihit2 = 0; ihit2 < hits2->size(); ihit2++)
  {
    art::Ptr<recob::Hit>   hit2(hits2, ihit2);
    wire_to_hit[hit2->WireID()].push_back(hit2);
  }

  for(size_t ihit1 = 0; ihit1 < hits1->size(); ihit1++)
  {
    art::Ptr<recob::Hit>   hit1(hits1, ihit1);
    double t1 = hit1->PeakTime();
    geo::WireID  wid1 = hit1->WireID();
    fEntry->wire = wid1.Wire;
    fEntry->plane = wid1.Plane;
    fEntry->hit1.t = t1;
    fEntry->hit1.integral = hit1->Integral();
    fEntry->hit1.height = hit1->PeakAmplitude();
    fEntry->hit1.tstart = hit1->StartTick();
    fEntry->hit1.tend = hit1->EndTick();
    fEntry->hit1.tsigma = hit1->SigmaPeakTime();
    fEntry->hit1.heightsigma = hit1->RMS();
    fEntry->hit1.integralsigma = hit1->SigmaIntegral();
    
//    for(size_t ihit2 = 0; ihit2 < hits2->size(); ihit2++)
//    {
//      art::Ptr<recob::Hit>   hit2(hits2, ihit2);
    // geo::WireID  wid2 = hit2->WireID();
    // if(wid2 == wid1) {
    for(auto hit2: wire_to_hit[wid1]){  
        double t2 = hit2->PeakTime();
        double dt = t2-t1;
        if(abs(dt) < 20) {
          fEntry->dt = dt;
          fEntry->hit2.t = t2;
          fEntry->hit2.integral     = hit2->Integral();
          fEntry->hit2.height       = hit2->PeakAmplitude();
          fEntry->hit2.tstart       = hit2->StartTick();
          fEntry->hit2.tend         = hit2->EndTick();
          fEntry->hit2.tsigma       = hit2->SigmaPeakTime();
          fEntry->hit2.heightsigma  = hit2->RMS();
          fEntry->hit2.integralsigma= hit2->SigmaIntegral();
           fTree->Fill();  
           
          hitlist1_matches[hit1.key()]++;         
          hitlist2_matches[hit2.key()]++;         
        }          
      // }
      
      
    }
  }
  
  // Look for unmatched hits
  for(size_t key = 0; key < hitlist1_matches.size(); key++) {
    if(hitlist1_matches[key] == 0) {
      art::Ptr<recob::Hit>   hit1(hits1, key);
      geo::WireID  wid1 = hit1->WireID();
      fEntry_unmatched1->wire = wid1.Wire;
      fEntry_unmatched1->plane = wid1.Plane;
      fEntry_unmatched1->hit1.t            = hit1->PeakTime();
      fEntry_unmatched1->hit1.integral     = hit1->Integral();
      fEntry_unmatched1->hit1.height       = hit1->PeakAmplitude();
      fEntry_unmatched1->hit1.tstart       = hit1->StartTick();
      fEntry_unmatched1->hit1.tend         = hit1->EndTick();
      fEntry_unmatched1->hit1.tsigma       = hit1->SigmaPeakTime();
      fEntry_unmatched1->hit1.heightsigma  = hit1->RMS();
      fEntry_unmatched1->hit1.integralsigma= hit1->SigmaIntegral();
      fTree_unmatched1->Fill();
    }
  }
  
  for(size_t key = 0; key < hitlist2_matches.size(); key++) {
    if(hitlist2_matches[key] == 0) {
      art::Ptr<recob::Hit>   hit2(hits2, key);
      geo::WireID  wid = hit2->WireID();
      fEntry_unmatched2->wire = wid.Wire;
      fEntry_unmatched2->plane = wid.Plane;
      fEntry_unmatched2->hit2.t            = hit2->PeakTime();
      fEntry_unmatched2->hit2.integral     = hit2->Integral();
      fEntry_unmatched2->hit2.height       = hit2->PeakAmplitude();
      fEntry_unmatched2->hit2.tstart       = hit2->StartTick();
      fEntry_unmatched2->hit2.tend         = hit2->EndTick();
      fEntry_unmatched2->hit2.tsigma       = hit2->SigmaPeakTime();
      fEntry_unmatched2->hit2.heightsigma  = hit2->RMS();
      fEntry_unmatched2->hit2.integralsigma= hit2->SigmaIntegral();
      fTree_unmatched2->Fill();
    }
  }
  
  
  // Look for overmatched hits...?

  
  
}
  
  



DEFINE_ART_MODULE(SnHitCompare)

}