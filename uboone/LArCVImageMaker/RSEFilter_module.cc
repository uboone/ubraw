////////////////////////////////////////////////////////////////////////
// Class:       RSEFilter
// Module Type: filter
// File:        RSEFilter_module.cc
//
// Generated at Fri Jul 28 01:52:57 2017 by Kazuhiro Terao using artmod
// from cetpkgsupport v1_11_00.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
//#include "art/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <exception>
#include <memory>
#include <set>
#include <map>

#include "SuperaCSVReader.h"

class RSEFilter;

class RSEFilter : public art::EDFilter {
public:
  explicit RSEFilter(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  RSEFilter(RSEFilter const &) = delete;
  RSEFilter(RSEFilter &&) = delete;
  RSEFilter & operator = (RSEFilter const &) = delete;
  RSEFilter & operator = (RSEFilter &&) = delete;

  // Required functions.
  bool filter(art::Event & e) override;


private:

  // Declare member data here.
  std::map<supera::RSEID,std::array<double,3> > _event_m;
};


RSEFilter::RSEFilter(fhicl::ParameterSet const & p)
{

  std::string runlist;

  cet::search_path finder("FW_SEARCH_PATH");

  if( !finder.find_file(p.get<std::string>("CSVName"),runlist) )
    throw cet::exception("LArSoftSuperaSriver") << "Unable to find supera cfg in "  << finder.to_string() << "\n";

  supera::csvreader::read_constraint_file(runlist, _event_m);
}

bool RSEFilter::filter(art::Event & e)
{
  int run = e.id().run();
  int subrun = e.id().subRun();
  int event = e.id().event();

  supera::RSEID id(run,subrun,event);
  return _event_m.find(id) != _event_m.end();
}

DEFINE_ART_MODULE(RSEFilter)