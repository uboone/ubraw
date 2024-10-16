////////////////////////////////////////////////////////////////////////
// Class:       BeamDataQualityFilter
// Module Type: filter
// File:        BeamDataQualityFilter_module.cc
//
// Generated at Tue May 10 13:33:22 2016 by Zarko Pavlovic using artmod
// from cetpkgsupport v1_10_01.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "lardataobj/RawData/BeamInfo.h"
#include "lardataobj/RawData/TriggerData.h"
#include <bitset>
#include <memory>
#include "getFOM.h"
#include "getFOM2.h"

#include "datatypes/ub_BeamHeader.h"
#include "datatypes/ub_BeamData.h"

#include "TTree.h"

class BeamDataQualityFilter;

class BeamDataQualityFilter : public art::EDFilter {
public:
  explicit BeamDataQualityFilter(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  BeamDataQualityFilter(BeamDataQualityFilter const &) = delete;
  BeamDataQualityFilter(BeamDataQualityFilter &&) = delete;
  BeamDataQualityFilter & operator = (BeamDataQualityFilter const &) = delete;
  BeamDataQualityFilter & operator = (BeamDataQualityFilter &&) = delete;

  // Required functions.
  bool filter(art::Event & e) override;

  // Selected optional functions.
  bool beginSubRun(art::SubRun & sr) override;
  void beginJob() override;
  void endJob() override;

private:

  // Declare member data here.
  typedef struct {
    std::string fBeamName;
    bool fRecalculateFOM;
    std::vector<double> fHornCurrentRange;
    std::vector<double> fIntensityRange;
    std::vector<double> fFOMRange;

    int fNHornCurrentCut;
    int fNIntensityCut;
    int fNFOMCut;
    int fFOMversion;
  } beamcuts_t;
  std::map<uint32_t, beamcuts_t> fBeamCutMap;
  TTree* fTree;
  uint32_t fRun;
  uint32_t fSubRun;
  uint32_t fEvent;
  uint32_t fSec;
  uint32_t fMSec;
  double fTor;
  double fHorn;
  double fFOM;
  uint32_t fTrigger;
  bool fResult;
  bool fUseAutoTune;
  bmd::autoTunes fHistory; 
};


BeamDataQualityFilter::BeamDataQualityFilter(fhicl::ParameterSet const & p)
  : EDFilter{p}
// Initialize member data here.
{
  // Call appropriate produces<>() functions here.
  std::vector<std::string> event_types=p.get<std::vector<std::string> >("apply_to_events");

  for (auto et : event_types) {
    fhicl::ParameterSet eps=p.get<fhicl::ParameterSet>(et);
    beamcuts_t bc;
    uint32_t trigger_mask=eps.get<uint32_t>("trigger_mask");    
    bc.fRecalculateFOM=eps.get<bool>("recalculate_fom");
    bc.fBeamName=eps.get<std::string>("beam_name");
    bc.fHornCurrentRange=eps.get<std::vector<double> >("horn_current_range");
    bc.fIntensityRange=eps.get<std::vector<double> >("intensity_range");
    bc.fFOMversion=eps.get<int>("fom_version");
    bc.fFOMRange=eps.get<std::vector<double> >("fom_range");
    std::pair<uint32_t, beamcuts_t> bcpair(trigger_mask, bc);
    fBeamCutMap.insert(bcpair);
  }
  fUseAutoTune=p.get<bool>("use_autotune");
  if (fUseAutoTune) {
    fHistory = bmd::cacheAutoTuneHistory();
  }
  art::ServiceHandle<art::TFileService> tfs;
  fTree = tfs->make<TTree>("bdq","Beam Data Quality Filter Summary");

  fTree->Branch("run",&fRun,"run/i");
  fTree->Branch("subrun",&fSubRun,"subrun/i");
  fTree->Branch("sec",&fSec,"sec/i");
  fTree->Branch("msec",&fMSec,"msec/i");
  fTree->Branch("event",&fEvent,"event/i");
  fTree->Branch("tor",&fTor,"tor/D");
  fTree->Branch("horn",&fHorn,"horn/D");
  fTree->Branch("fom",&fFOM,"fom/D");
  fTree->Branch("trigger",&fTrigger,"trigger/i");
  fTree->Branch("result",&fResult,"result/O");
}

bool BeamDataQualityFilter::beginSubRun(art::SubRun & sr)
{
  fRun=sr.run();
  fSubRun=sr.subRun();
  
  return true;
}

bool BeamDataQualityFilter::filter(art::Event & e)
{
   // Implementation of required member function here.
  fResult=true;
  fTrigger=-1;
  fFOM=-99999;
  fHorn=-99999;
  fTor=-99999;   
  art::Timestamp ts = e.time();
  fSec=ts.timeHigh();
  fMSec=ts.timeLow()/1e6;
  fEvent=e.event();

  auto triggerHandle = e.getHandle<std::vector<raw::Trigger>>("daq");
  if (!triggerHandle) {
    mf::LogWarning(__FUNCTION__) << "Missing trigger info. Skipping event.";
    fTree->Fill();
    return fResult;
  }

  raw::Trigger const& triggerInfo = triggerHandle->front();
  fTrigger=triggerInfo.TriggerBits();
  std::bitset<16> trigbit(triggerInfo.TriggerBits());
  if (fBeamCutMap.find(triggerInfo.TriggerBits())==fBeamCutMap.end()) {
    mf::LogInfo(__FUNCTION__) << "Trigger not matching any of the beam(s). Skipping beam quality filter. trigger bits= "<<triggerInfo.TriggerBits()<<" "<<trigbit;
    fTree->Fill();
    return fResult;
  }
  beamcuts_t* bc=&fBeamCutMap[triggerInfo.TriggerBits()];
  art::Handle< raw::BeamInfo > beam;
  if (e.getByLabel("beamdata",beam)){
    std::map<std::string, std::vector<double>> datamap = beam->GetDataMap();
    if (datamap.size()==0) {
      mf::LogWarning(__FUNCTION__)<<"Event missing beam data, but this was beam event. Fail beam data quality";
      fResult=false;
      fTree->Fill();
      return fResult;
    }
    if (bc->fRecalculateFOM) {
      gov::fnal::uboone::datatypes::ub_BeamHeader ubbh;
      std::vector<gov::fnal::uboone::datatypes::ub_BeamData> ubbdvec;
      //currently beamHeader, timestamp not required by getFOM function
      //filling only data which is required 
      for (auto& bdata : datamap) {
	gov::fnal::uboone::datatypes::ub_BeamData ubbd;
	ubbd.setDeviceName(bdata.first);
	ubbd.setData(bdata.second);
	ubbdvec.push_back(ubbd);
      } 
      bnb::bnbAutoTune settings = bnb::bnbAutoTune();
      if(fUseAutoTune) settings = bmd::getSettings(fHistory, *beam);
      
      if (bc->fFOMversion==1)
	fFOM=bmd::getFOM(bc->fBeamName,ubbh,ubbdvec);
      else if (bc->fFOMversion==2) 
	fFOM=bmd::getFOM2(bc->fBeamName,ubbh,ubbdvec, settings, fUseAutoTune);
      else
	mf::LogError(__FUNCTION__)<<"Invalid FOM version"<<fFOM;

      mf::LogDebug(__FUNCTION__)<<"Recalculated fom="<<fFOM;
    } else {
      //get it from BeamInfo
      if (datamap["FOM"].size()>0)
	fFOM=datamap["FOM"][0];
      else
	mf::LogError(__FUNCTION__)<<"recalculate_fom set to false, but FOM is not in beamdata product";
    }
    if ( fFOM<bc->fFOMRange[0] || fFOM>bc->fFOMRange[1]) {
      fResult=false;
      bc->fNFOMCut+=1;
    }
    if (bc->fBeamName=="bnb" && datamap["E:TOR860"].size()>0) fTor=datamap["E:TOR860"][0];
    else if (bc->fBeamName=="bnb" && datamap["E:TOR875"].size()>0) fTor=datamap["E:TOR875"][0];
    else if (bc->fBeamName=="numi" && datamap["E:TORTGT"].size()>0) fTor=datamap["E:TORTGT"][0];
    else if (bc->fBeamName=="numi" && datamap["E:TOR101"].size()>0) fTor=datamap["E:TOR101"][0];
    if (bc->fBeamName=="bnb" && fRun<3700 && (datamap["E:TOR860"][0]>6 || fabs(datamap["E:TOR860"][0]-datamap["E:TOR875"][0])>0.3 || ((datamap["E:TOR860"][0]<0.3 || datamap["E:TOR875"][0]<0.3))) && datamap["E:HI860"].size()>0 && datamap["E:HI860"][0]>0) {
      fTor=1.613e-4*datamap["E:HI860"][0]-0.003852;
      //tor860[0]=1.613e-4*hi860[0]-0.003852
      //tor875[0]=1.806e-4*hitg2[0]+0.02174
    }
    if ( fTor<bc->fIntensityRange[0] || fTor>bc->fIntensityRange[1]) {
      fResult=false;
      bc->fNIntensityCut+=1;
    }
    if (bc->fBeamName=="bnb" && datamap["E:THCURR"].size()>0) fHorn=datamap["E:THCURR"][0];

    else if (bc->fBeamName=="numi" && datamap["E:NSLINA"].size()>0 &&
	     datamap["E:NSLINB"].size()>0 && datamap["E:NSLINC"].size()>0 &&
	     datamap["E:NSLIND"].size()>0) {
      //from http://nusoft.fnal.gov/nova/novasoft/doxygen/html/IFDBSpillInfo__module_8cc_source.html
      //it references maul from Jim Hylen from Oct 4 2013
      fHorn=0;
      fHorn+=(datamap["E:NSLINA"][0]-(+0.01))/0.9951;
      fHorn+=(datamap["E:NSLINB"][0]-(-0.14))/0.9957;
      fHorn+=(datamap["E:NSLINC"][0]-(-0.05))/0.9965;
      fHorn+=(datamap["E:NSLIND"][0]-(-0.07))/0.9945;
    }
    if ( fHorn<bc->fHornCurrentRange[0] || fHorn>bc->fHornCurrentRange[1]) {
      fResult=false;
      bc->fNHornCurrentCut+=1;
    }
  } else {
    mf::LogError(__FUNCTION__)<<"Running beam data quality filter, but missing beam data!";
  }
 
  fTree->Fill();
  return fResult;
}

void BeamDataQualityFilter::beginJob()
{
  // Implementation of optional member function here.
  for (auto& bc: fBeamCutMap) {
    bc.second.fNHornCurrentCut=0;
    bc.second.fNIntensityCut=0;
    bc.second.fNFOMCut=0;
  }
}

void BeamDataQualityFilter::endJob()
{
  // Implementation of optional member function here.
  std::stringstream ss;
  for (auto& bc: fBeamCutMap) {
    ss<<"Beam name: "<<bc.second.fBeamName<<std::endl;
    ss<<"Events failed due to intensity cut: "<<bc.second.fNIntensityCut<<std::endl;
    ss<<"Events failed due to horn current cut: "<<bc.second.fNHornCurrentCut<<std::endl;
    ss<<"Events failed due to FOM cut: "<<bc.second.fNFOMCut<<std::endl;
  }
  mf::LogInfo(__FUNCTION__)<<ss.str();
}

DEFINE_ART_MODULE(BeamDataQualityFilter)
