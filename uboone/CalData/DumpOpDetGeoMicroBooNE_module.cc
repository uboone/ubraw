////////////////////////////////////////////////////////////////////////
// Class:       DumpOpDetGeoMicroBooNE
// Module Type: analyzer
// File:        DumpOpDetGeoMicroBooNE_module.cc
//
// Generated at Wed Jan  7 08:56:55 2015 by Matthew Toups using artmod
// from cetpkgsupport v1_07_00.
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

#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/OpDetGeo.h"
#include "larcorealg/Geometry/geo.h"

#include <iostream>

//class DumpOpDetGeoMicroBooNE;
namespace caldata {

class DumpOpDetGeoMicroBooNE : public art::EDAnalyzer {
public:
  explicit DumpOpDetGeoMicroBooNE(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  DumpOpDetGeoMicroBooNE(DumpOpDetGeoMicroBooNE const &) = delete;
  DumpOpDetGeoMicroBooNE(DumpOpDetGeoMicroBooNE &&) = delete;
  DumpOpDetGeoMicroBooNE & operator = (DumpOpDetGeoMicroBooNE const &) = delete;
  DumpOpDetGeoMicroBooNE & operator = (DumpOpDetGeoMicroBooNE &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;


private:

  // Declare member data here.

};


DumpOpDetGeoMicroBooNE::DumpOpDetGeoMicroBooNE(fhicl::ParameterSet const & p)
  :
  EDAnalyzer(p)  // ,
 // More initializers here.
{}

void DumpOpDetGeoMicroBooNE::analyze(art::Event const & e)
{
  art::ServiceHandle<geo::Geometry> geoHandle;
  geo::Geometry const& geo(*geoHandle);
  double xyz[3] = {0.};
  int NOpDets = (int)geo.Cryostat(0).NOpDet();
  std::cout << "OpDet (x,y,z) positions" << std::endl;
  std::cout << "-----------------------" << std::endl;
  for(int i = 0; i<NOpDets; ++i) {
    geo.Cryostat(0).OpDet(i).GetCenter(xyz);
    std::cout << "OptDet " << i << ": (" << xyz[0] << "," << xyz[1] << "," << xyz[2] << ")" << std::endl;
  }
  // Implementation of required member function here.
}

DEFINE_ART_MODULE(DumpOpDetGeoMicroBooNE)

}
