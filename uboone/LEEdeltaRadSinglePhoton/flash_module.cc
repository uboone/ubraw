////////////////////////////////////////////////////////////////////////
// Class:       flash
// Plugin Type: analyzer (art v2_05_00)
// File:        flash_module.cc
//
// Generated at Mon Sep 25 08:47:26 2017 by Robert Murrells using cetskelgen
// from cetlib version v1_21_00.
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

#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"

#include "TTree.h"

#include "lardataobj/RecoBase/OpFlash.h"

class flash;


class flash : public art::EDAnalyzer {

public:

  explicit flash(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  flash(flash const &) = delete;
  flash(flash &&) = delete;
  flash & operator = (flash const &) = delete;
  flash & operator = (flash &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;

private:

  std::string fopflash_producer;

  TTree * ftree;

  double ftime;
  double fpe;

  void reset();

};


flash::flash(fhicl::ParameterSet const & p) :
  EDAnalyzer(p) {

  fopflash_producer = p.get<std::string>("opflash_producer");
  
  art::ServiceHandle<art::TFileService> tfs;
  ftree = tfs->make<TTree>("flash_tree", "");
  ftree->Branch("time", &ftime, "time/D");
  ftree->Branch("pe", &fpe, "pe/D");

}


void flash::reset() {

  ftime = -1;
  fpe = -1;

}


void flash::analyze(art::Event const & e) {

  art::ValidHandle<std::vector<recob::OpFlash>> const & ev_opf =
    e.getValidHandle<std::vector<recob::OpFlash>>(fopflash_producer);
  
  for(recob::OpFlash const & opf : *ev_opf) {
    reset();
    ftime = opf.Time();
    fpe = opf.TotalPE();
    ftree->Fill();
  }
  
}

DEFINE_ART_MODULE(flash)
