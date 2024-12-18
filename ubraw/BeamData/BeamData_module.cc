////////////////////////////////////////////////////////////////////////
// Class:       BeamData
// Module Type: producer
// File:        BeamData_module.cc
//
// Generated at Thu Sep  3 16:51:01 2015 by Zarko Pavlovic using artmod
// from cetpkgsupport v1_08_06.
////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/FileBlock.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"

#include "ifdh_art/IFDHService/IFDH_service.h"

#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "larcoreobj/SummaryData/POTSummary.h"
#include "lardataobj/RawData/BeamInfo.h"
#include "lardataobj/RawData/DAQHeader.h"
#include "lardataobj/RawData/TriggerData.h"
#include "datatypes/raw_data_access.h"

#include "../BeamDAQ/beamRun.h"
#include "../BeamDAQ/beamRunHeader.h"
#include "../BeamDAQ/beamDAQConfig.h"
#include "getFOM.h"
#include "getFOM2.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <bitset>
#include <algorithm>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <ctime>

#include <memory>

#include "TTree.h"

namespace {

  // Local function to return the name of the raw ancestor of a file
  // with the specified run and subrun.

  std::string get_raw_ancestor(const std::string& filename, uint32_t run, uint32_t subrun,
                               const std::string& child, std::string& swizzler_version)
  {
    swizzler_version = std::string();
    art::ServiceHandle<ifdh_ns::IFDH> ifdh;
    std::ostringstream dim;
    dim << "isparentof: ( file_name " << filename << ")"
        << " and run_number " << run << "." << subrun
        << " and availability: anylocation";
    std::vector<std::string> parents = ifdh->translateConstraints(dim.str());

    // If there are exactly two parents, determine if one is the parent of the other.

    if(parents.size() == 2) {

      std::string parent1 = parents[0];
      std::string parent2 = parents[1];

      //std::cout << "parent1=" << parent1 << std::endl;
      //std::cout << "parent2=" << parent2 << std::endl;

      // is parent1 a parent of parent2?

      std::ostringstream dim12;
      dim12 << "isparentof: ( file_name " << parent2 << ")"
            << " and file_name " << parent1
            << " and availability: anylocation";
      std::vector<std::string> parents12 = ifdh->translateConstraints(dim12.str());
      size_t n12 = parents12.size();

      // is parent2 a parent of parent1?

      std::ostringstream dim21;
      dim21 << "isparentof: ( file_name " << parent1 << ")"
            << " and file_name " << parent2
            << " and availability: anylocation";
      std::vector<std::string> parents21 = ifdh->translateConstraints(dim21.str());
      size_t n21 = parents21.size();

      // Maybe filter parents.

      if(n12 == 1 and n21 == 0)
        parents.pop_back();
      else if(n12 == 0 and n21 == 1)
        parents.erase(parents.begin(), parents.begin()+1);
    }


    if(parents.size() == 0) {

      // If there are no parents, assume this is the raw ancestor.
      // Child file, if known, is the first generation swizzled file.

      if(child.size() > 0) {
        mf::LogInfo log(__FUNCTION__);
        log << "Found raw ancestor file = " << filename << "\n"
            << "Swizzled file = " << child << "\n";
        art::ServiceHandle<ifdh_ns::IFDH> ifdh;
        std::string md = ifdh->getMetadata(child);
        log << "Swizzled file metadata:\n";
        log << md;

        // Get application version of child file.
        
        size_t n1 = md.find("Application:");
        if(n1 < std::string::npos) {
          n1 += 12;
          size_t n2 = md.find("\n", n1);
          std::string app = md.substr(n1, n2-n1);
          size_t n3 = app.rfind(" ");
          std::string app_version = app.substr(n3+1);
          log << "\nApplication version = " << "\"" << app_version << "\"";
          swizzler_version = app_version;
        }
      }

      return filename;
    }

    else if(parents.size() == 1)

      // If there is a single parent, return its raw ancestor.

      return get_raw_ancestor(parents.front(), run, subrun, filename, swizzler_version);

    else

      // Don't know what to do if there is more than one parent.

      return std::string();
  }
}


class BeamData;

class BeamData : public art::EDProducer {
public:
  explicit BeamData(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  BeamData(BeamData const &) = delete;
  BeamData(BeamData &&) = delete;
  BeamData & operator = (BeamData const &) = delete;
  BeamData & operator = (BeamData &&) = delete;

  // Required functions.
  void beginSubRun(art::SubRun & sr) override;
  void produce(art::Event & e) override;
  void endSubRun(art::SubRun & sr) override;
  void endJob() override;
  void respondToOpenInputFile(art::FileBlock const& fb) override;	
  void respondToCloseInputFile(art::FileBlock const& fb) override;

  bool nextBeamEvent(std::string beamline, ub_BeamHeader &bh, std::vector<ub_BeamData> &bd);
  bool rewindBeamFile(std::string beam, const ub_BeamHeader& bh, const std::vector<ub_BeamData> &bd); //goes back one event in the file
  int compareTime(ub_BeamHeader& bh, art::Event& e, float dt, float offsetT);
  void createBranches(std::string beam);
  void fillTreeData(std::string beam, const ub_BeamHeader& bh, const std::vector<ub_BeamData>& bd);
  void addPOT(std::string beam, const ub_BeamHeader& bh, const std::vector<ub_BeamData>& bd);
  void removePOT(std::string beam, const ub_BeamHeader& bh, const std::vector<ub_BeamData>& bd);

private:

  // Declare member data here.
  struct BeamConf_t {
    int fBeamID;
    std::ifstream* fBeamStream;
    std::string fFilePath;
    std::string fFileName;
    std::map<std::string, float> fTotalSums;
    std::map<std::string, float> fGoodSums;
    uint32_t fTotalSpillCount;
    uint32_t fGoodSpillCount;
    uint32_t fMergedEventCount;
    uint32_t fNonMergedEventCount;
    float fOffsetT;
    float fDt;
    uint32_t fTriggerMask;
    bool fWriteBeamData;
    int fFOMversion;
    float fFOMcut;
    TTree* fTree;
    std::map<std::string, double> fTreeVar;
    std::map<std::string, double*> fTreeArr;
  };

  std::vector<std::string> fBeams;
  std::map<std::string, BeamConf_t>  fBeamConf;
  uint32_t fRun;
  uint32_t fSubRun;
  uint32_t fNonBeamCount;
  uint32_t fSeconds;
  uint32_t fMilliSeconds;
  float fFOM;
  std::string fInputFileName;
  float fGPSOffset;

  std::map<std::string,bool> fCreateBranches;
  bool fFetchBeamData;
  bool fUseAutoTune;
  std::string fBDAQfhicl;

  boost::posix_time::ptime fSubrunT0;
  boost::posix_time::ptime fSubrunT1;
  std::map<std::string, boost::posix_time::ptime> fTLast;
  bmd::autoTunes fHistory; 
};

BeamData::BeamData(fhicl::ParameterSet const & p)
  : EDProducer{p}
{
  // Set timezone to Fermilab.
  setenv("TZ", "CST+6CDT", 1);
  tzset();
  
  // Call appropriate produces<>() functions here.
  fBeams=p.get<std::vector<std::string> >("beams");
  for (unsigned int i=0;i<fBeams.size();i++) {
    fhicl::ParameterSet pbeam=p.get<fhicl::ParameterSet>(fBeams[i]);
    BeamConf_t bconf;
    bconf.fBeamID=i;
    std::vector<std::string> sum_devices=pbeam.get<std::vector<std::string> >("sum_devices");
    for (auto it=sum_devices.begin();it!=sum_devices.end();it++) {
      bconf.fTotalSums[*it]=0;
      bconf.fGoodSums[*it]=0;
    }
    bconf.fTriggerMask=pbeam.get<uint32_t>("trigger_mask");
    bconf.fOffsetT=pbeam.get<float>("time_offset");
    bconf.fDt=pbeam.get<float>("merge_time_tolerance");
    bconf.fFOMversion=pbeam.get<int>("FOM_version");
    bconf.fFOMcut=pbeam.get<float>("FOM_cut");
    bconf.fFilePath=pbeam.get<std::string>("path_to_beam_file");
    bconf.fWriteBeamData=pbeam.get<bool>("write_beam_data");
    std::pair<std::string, BeamConf_t> p(fBeams[i],bconf);
    fBeamConf.insert(p);
    std::pair<std::string, bool> p2(fBeams[i],true);
    fCreateBranches.insert(p2);
  }
  
  fFetchBeamData=p.get<bool>("fetch_beam_data");
  if (fFetchBeamData) {
    fBDAQfhicl=p.get<std::string>("bdaq_fhicl_file");
  }
  fUseAutoTune=p.get<bool>("use_autotune");
  if (fUseAutoTune) {
    fHistory = bmd::cacheAutoTuneHistory();
  }
  for (auto& it_beamline : fBeamConf) {
    if (it_beamline.second.fWriteBeamData) {
      art::ServiceHandle<art::TFileService> tfs;
      it_beamline.second.fTree = tfs->make<TTree>(it_beamline.first.c_str(),(it_beamline.first+" beam data").c_str());
      //create branches once the file is open
    }
  }
  
  produces< raw::BeamInfo >();  
  for (auto& it_beamline : fBeamConf ) {
    for (auto& it_dev : it_beamline.second.fTotalSums ) {
      std::string varname=it_beamline.first+it_dev.first;
      varname.erase(std::remove(varname.begin(), varname.end(), ':'), varname.end());
      produces< sumdata::POTSummary, art::InSubRun >(varname);
    }
  }
}

void BeamData::beginSubRun(art::SubRun & sr)
{

  fRun=sr.run();
  fSubRun=sr.subRun();

  if (fFetchBeamData) {
    mf::LogInfo(__FUNCTION__)<<"Fetching beam files for run "<<sr.run()
			     <<" subrun "<<sr.subRun();

    setenv("BEAMDAQ_CONFIG_FILE", fBDAQfhicl.c_str(), 1);
    
    gov::fnal::uboone::beam::beamDAQConfig* bdconfig=gov::fnal::uboone::beam::beamDAQConfig::GetInstance();
    if (!bdconfig) 
      mf::LogError(__FUNCTION__) <<"Failed to initialize beamDAQConfig";
    
    // Get sam metadata for input file.
    art::ServiceHandle<ifdh_ns::IFDH> ifdh;
    boost::filesystem::path inputPath(fInputFileName);
    std::string swizzler_version;
    std::string raw_ancestor = get_raw_ancestor(inputPath.filename().string(), fRun, fSubRun,
                                                std::string(), swizzler_version);
    //std::cout << "raw ancestor = " << raw_ancestor << std::endl;
    std::string md = ifdh->getMetadata(raw_ancestor);
    mf::LogInfo(__FUNCTION__)<< "BeamData: metadata" << std::endl<< md;

    // Check the timezone, which we set in the constructor.
    // Throw an exception if the timezone is not Fermilab's.
    // If the timezone is wrong, then someone else is trying to hijack the 
    // timezone (not good).

    const char* tz = getenv("TZ");
    if(tz != 0 && *tz != 0) {

      // Timezone has been set.
      // Make sure it is correct.

      std::string tzs(tz);
      if(tzs != std::string("CST+6CDT")) {

	// Timezone is wrong, throw exception.

	throw cet::exception("BeamData") << "Wrong timezone: " << tzs;
      }
    }
    else {

      // Timezone is not set.  Throw exception.

      throw cet::exception("BeamData") << "Timezone not set.";
    }

    size_t n1 = md.find("Start Time:");
    n1 += 11;
    size_t n2 = md.find("\n", n1);
    //std::cout << "Start time = " << md.substr(n1, n2-n1) << std::endl;
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    tm.tm_isdst = -1;
    strptime(md.substr(n1, n2-n1).c_str(), "%Y-%m-%dT%H:%M:%S%Z", &tm);
    //    tm.tm_hour -= 1;
    time_t tstart = mktime(&tm);
    //std::cout << "Start time seconds = " << tstart << std::endl;
    n1 = md.find("End Time:");
    n1 += 9;
    n2 = md.find("\n", n1);
    //std::cout << "End time = " << md.substr(n1, n2-n1) << std::endl;
    memset(&tm, 0, sizeof(tm));
    tm.tm_isdst = -1;
    strptime(md.substr(n1, n2-n1).c_str(), "%Y-%m-%dT%H:%M:%S%Z", &tm);
    //tm.tm_hour -= 1;
    time_t tend = mktime(&tm);

    //std::cout << "End time seconds = " << tend << std::endl;
    n1 = md.find("online.start_time_usec:");
    n1 = md.find(":",n1)+1;
    n2 = md.find("\n", n1);
    //std::cout << "Usec start time = " << md.substr(n1, n2-n1) << std::endl;
    long tstart_us=stol(md.substr(n1, n2-n1));
    n1 = md.find("online.end_time_usec:");
    n1 = md.find(":",n1)+1;
    n2 = md.find("\n", n1);
    //std::cout << "Usec end time = " << md.substr(n1, n2-n1) << std::endl;
    long tend_us=stol(md.substr(n1, n2-n1));
 
    boost::posix_time::time_duration zoneOffset = boost::posix_time::second_clock::local_time()-boost::posix_time::second_clock::universal_time();

    fSubrunT0= boost::posix_time::from_time_t(tstart)+ boost::posix_time::hours(zoneOffset.hours())+ boost::posix_time::microseconds(tstart_us);
    fSubrunT1= boost::posix_time::from_time_t(tend)+ boost::posix_time::hours(zoneOffset.hours())+ boost::posix_time::microseconds(tend_us);
    gov::fnal::uboone::beam::beamRun brm;
    gov::fnal::uboone::beam::beamRunHeader rh;
    rh.fRun=fRun;
    rh.fSubRun=fSubRun;
    brm.StartRun(rh,fSubrunT0);
    brm.EndRun(fSubrunT1);
    
    //need to pad beginning and end time to account for possible time diff between detector and beam time and make 
    //sure first and last event are not discarded in if (tevent<fSubrunT0 || tevent>fSubrunT1) statement
    fSubrunT0=fSubrunT0- boost::posix_time::microseconds(20000);
    fSubrunT1=fSubrunT1+ boost::posix_time::microseconds(20000);

    // Maybe extract gps offset from raw ancestor metadata.

    fGPSOffset = 0.;
    mf::LogInfo log(__FUNCTION__);
    log << "Checking for GPS offset" << "\n";
    log << "Swizzler version = " << swizzler_version << "\n";
    if(swizzler_version == "v04_26_04_07" || swizzler_version == "v04_26_04_06") {
      n1 = md.find("gps.offset:");
      if(n1 < std::string::npos) {
        n1 += 11;
        n2 = md.find("\n", n1);
        std::string gps_offset_s = md.substr(n1, n2-n1);
        log << "Found GPS offset = " << gps_offset_s;
        fGPSOffset = std::stof(gps_offset_s);
      }
      else {
        log << "No GPS offset in metadata.";
      }
    }
    else {
      log << "Skipping GPS offset check because swizzler version is not v04_26_04_06 or v04_26_04_07";
    }
  }
  
  mf::LogInfo(__FUNCTION__)<<"Open beam files for run "<<sr.run()
			   <<" subrun "<<sr.subRun();
  std::stringstream ss;
  for (int i=0;i<120;i++) ss<<"=";
  ss<<std::endl;
  ss<<std::setw(10)<<"Beam"
    <<std::setw(40)<<"File"
    <<std::setw(30)<<"Time offset (ms)"
    <<std::setw(30)<<"Time tolerance (ms)"
    <<std::setw(10)<<"Found"
    <<std::endl;
  for (int i=0;i<120;i++) ss<<"-";
  ss<<std::endl;  
  for (unsigned int ibeam=0;ibeam<fBeams.size();ibeam++) {
    std::stringstream fname;
    fname<<"beam_"<<fBeams[ibeam]<<"_"
	 <<std::setfill('0') << std::setw(7) << fRun<<"_"
	 <<std::setfill('0') << std::setw(5) << fSubRun<<".dat";
    ss<<std::setw(10)<<fBeams[ibeam]
      <<std::setw(40)<<fname.str()
      <<std::setw(30)<<fBeamConf[fBeams[ibeam]].fOffsetT
      <<std::setw(30)<<fBeamConf[fBeams[ibeam]].fDt;
    std::ifstream *fin=new std::ifstream(fBeamConf[fBeams[ibeam]].fFilePath+
			       fname.str(), 
			       std::ios::binary);
    if ( !fin->is_open() ) {
      fBeamConf.erase(fBeams[ibeam]);
      delete fin;
      ss<<std::setw(10)<<"No";
    } else {      
      fBeamConf[fBeams[ibeam]].fFileName=fname.str();
      fBeamConf[fBeams[ibeam]].fTotalSpillCount=0;
      fBeamConf[fBeams[ibeam]].fGoodSpillCount=0;
      for (auto it=fBeamConf[fBeams[ibeam]].fTotalSums.begin();
	   it!=fBeamConf[fBeams[ibeam]].fTotalSums.end();it++) {
	fBeamConf[fBeams[ibeam]].fTotalSums[it->first]=0;
	fBeamConf[fBeams[ibeam]].fGoodSums[it->first]=0;
      }
      fBeamConf[fBeams[ibeam]].fNonMergedEventCount=0;
      fBeamConf[fBeams[ibeam]].fMergedEventCount=0;
      fBeamConf[fBeams[ibeam]].fBeamStream=fin;
      ss<<std::setw(10)<<"Yes";
    }
    ss<<std::endl;
  }
  fNonBeamCount=0;
  for (int i=0;i<120;i++) ss<<"=";
  ss<<std::endl;
  mf::LogInfo(__FUNCTION__) <<ss.str();

  for (auto& it : fBeamConf) 
    if (fCreateBranches[it.first] && it.second.fWriteBeamData) createBranches(it.first);
}

void BeamData::endSubRun(art::SubRun & sr)
{
  //loop over remaining beam events

  for (auto it=fBeamConf.begin();it!=fBeamConf.end();it++) {
    mf::LogInfo(__FUNCTION__) <<"Loop through the remaining "
			      <<it->first<<" beam events";
    while (true) {
      ub_BeamHeader bh;
      std::vector<ub_BeamData> bd;
      if (nextBeamEvent(it->first,bh,bd)) {
	/*
	boost::posix_time::time_duration zoneOffset = boost::posix_time::second_clock::local_time()-boost::posix_time::second_clock::universal_time();
        boost::posix_time::ptime tevnt= boost::posix_time::from_time_t(bh.getSeconds())
          + boost::posix_time::hours(zoneOffset.hours())
          + boost::posix_time::microseconds(bh.getMilliSeconds()*1000)
          + boost::posix_time::microseconds(static_cast<int>(fBeamConf[it->first].fOffsetT*1000));
	*/
	//calculate FOM
        bnb::bnbAutoTune settings = bnb::bnbAutoTune();
        if(fUseAutoTune) settings = bmd::getSettings(fHistory, bh);
	if (fBeamConf[it->first].fFOMversion==1) {
	  fFOM=bmd::getFOM(it->first,bh,bd);
	} else if(fBeamConf[it->first].fFOMversion==2) {
	  fFOM=bmd::getFOM2(it->first,bh,bd, settings, fUseAutoTune);
	} else {
	  mf::LogError(__FUNCTION__)<<"Unkown FOM version!";
	}
	//no need to check for subrun boundaries since pot count comes from
	//offline counting
	//the boundary check may not work correctly for runs where gps drifted
	//	if (tevnt>fSubrunT0 && tevnt<=fSubrunT1) {
	if (fBeamConf[it->first].fWriteBeamData) 
	  fillTreeData(it->first,bh,bd);
	addPOT(it->first,bh,bd);
	//}
      } else {
	break;
      }
    }
  }
  for (auto& it_beamline : fBeamConf ) {
    for (auto& it_dev : it_beamline.second.fTotalSums ) {
      std::unique_ptr<sumdata::POTSummary> pot(new sumdata::POTSummary);    
      pot->totpot = it_dev.second;
      pot->totgoodpot = it_beamline.second.fGoodSums[it_dev.first];  
      pot->totspills = it_beamline.second.fTotalSpillCount;
      pot->goodspills = it_beamline.second.fGoodSpillCount;
      std::string varname=it_beamline.first+it_dev.first;
      varname.erase(std::remove(varname.begin(), varname.end(), ':'), varname.end());
      sr.put(std::move(pot),varname, art::subRunFragment());
    }
  }

  for (auto it=fBeamConf.begin();it!=fBeamConf.end();it++) {
    (it->second).fBeamStream->close();
    delete (it->second).fBeamStream;
  }
  std::stringstream ss;
  ss<<"Non beam events: "<<fNonBeamCount<<std::endl;
  for (int i=0;i<120;i++) ss<<"=";
  ss<<std::endl;  
  ss<<std::setw(10)<<"Beam"
    <<std::setw(50)<<"File"
    <<std::setw(20)<<"Merged events"
    <<std::setw(20)<<"Missing beam info"
    <<std::setw(20)<<"Total spills"
    <<std::endl;
  for (int i=0;i<120;i++) ss<<"-";
  ss<<std::endl;
  for (auto& it : fBeamConf ) {
    ss<<std::setw(10)<<fBeams[it.second.fBeamID]
      <<std::setw(50)<<it.second.fFileName
      <<std::setw(20)<<it.second.fMergedEventCount
      <<std::setw(20)<<it.second.fNonMergedEventCount
      <<std::setw(20)<<it.second.fTotalSpillCount
      <<std::endl;
  }
  for (int i=0;i<120;i++) ss<<"=";
  ss<<std::endl;
  ss<<std::endl;
  for (int i=0;i<100;i++) ss<<"=";
  ss<<std::endl;
  ss<<std::setw(20)<<"Beam"
    <<std::setw(20)<<"Device"
    <<std::setw(30)<<"Total Sum (Spills)"
    <<std::setw(30)<<"Good Sum (Good spills)"
    <<std::endl;
  for (int i=0;i<100;i++) ss<<"-";
  ss<<std::endl;
  for (auto& it : fBeamConf ) {
    ss<<std::setw(20)<<it.first;
    bool ifirst=true;
    for (auto& itdev : it.second.fTotalSums) {
      if (!ifirst) ss<<std::setw(20)<<" ";	
      ss<<std::setw(20)<<itdev.first
	<<std::setw(20)<<it.second.fTotalSums[itdev.first]
	<<std::setw(10)<<std::string(" (")+std::to_string(it.second.fTotalSpillCount)+std::string(") ")
	<<std::setw(20)<<it.second.fGoodSums[itdev.first]
	<<std::setw(10)<<std::string(" (")+std::to_string(it.second.fGoodSpillCount)+std::string(") ")
	<<std::endl;
      ifirst=false;
    }      
  }
  for (int i=0;i<100;i++) ss<<"=";
  ss<<std::endl;

  mf::LogInfo(__FUNCTION__)<<ss.str();

}

void BeamData::endJob()
{
  fBeamConf.clear();
}

void BeamData::respondToOpenInputFile(art::FileBlock const& fb)
{
  fInputFileName = fb.fileName();
}

void BeamData::respondToCloseInputFile(art::FileBlock const& fb)
{
  fInputFileName.clear();
}


void BeamData::produce(art::Event & e)
{
  // Implementation of required member function here.
  std::unique_ptr<raw::BeamInfo> beam_info(new raw::BeamInfo);
  
  auto triggerHandle = e.getHandle<std::vector<raw::Trigger>>("daq");
  if (!triggerHandle) {
    mf::LogWarning(__FUNCTION__) << "Missing trigger info. Skipping event.";
    return;
  }

  //Argh... bits.... should double check this
  /*
  union {
    struct {
      uint16_t pmt_trig_data :8;
      uint16_t pc            :1;
      uint16_t external      :1;
      uint16_t active        :1;
      uint16_t gate2         :1;
      uint16_t gate1         :1;
      uint16_t veto          :1;
      uint16_t calib         :1;
      uint16_t phase0        :1;
    } trig_struct;
    uint16_t trig_data_1=0x0;
  } trig_union;
  trig_union.trig_data_1=(uint16_t)(trigInfo[0]->TriggerBits()&0x0000FFFF);
  mf::LogInfo(__FUNCTION__) <<trig_union.trig_struct.pc<<"\n"
			    <<trig_union.trig_struct.external<<"\n"
			    <<trig_union.trig_struct.active<<"\n"
			    <<trig_union.trig_struct.gate2<<"\n"
			    <<trig_union.trig_struct.gate1<<"\n"
			    <<trig_union.trig_struct.veto<<"\n"
			    <<trig_union.trig_struct.calib<<"\n"
			    <<trig_union.trig_struct.phase0<<"\n";
  */

  raw::Trigger const& trigInfo = triggerHandle->front();
  std::string beam_name="";
  std::bitset<16> trigbit(trigInfo.TriggerBits());
  for (auto& it : fBeamConf) {
    if (trigInfo.TriggerBits() & it.second.fTriggerMask) {
      beam_name=it.first;
    }
  }
  if (beam_name=="") {
    mf::LogInfo(__FUNCTION__) << "Trigger not matching any of the beam(s). Skipping beam merging. trigger bits= "<<trigInfo.TriggerBits()<<" "<<trigbit;
    fNonBeamCount++;

  } else {
  mf::LogInfo(__FUNCTION__) <<"Looking for "<<beam_name<<" event (trigger bits= "<<trigInfo.TriggerBits()<<" "<<trigbit<<" )";

  art::Handle< raw::DAQHeader > daqHeaderHandle;
  e.getByLabel("daq", daqHeaderHandle);
  time_t dettime64  = daqHeaderHandle->GetTimeStamp();
   
  uint32_t detsec = uint32_t(dettime64>>32);
  uint32_t detmsec =uint32_t((dettime64 & 0xFFFFFFFF)/1000000);
    
  mf::LogInfo(__FUNCTION__)<<"Looking for a match to det event "<<detsec<<"\t"<<detmsec;

  bool ready_for_next_det_event=false;
  while ( !ready_for_next_det_event ) {
    ub_BeamHeader bh;
    std::vector<ub_BeamData> bd;
    if (nextBeamEvent(beam_name,bh,bd)) {
      int comp=compareTime(bh,e,fBeamConf[beam_name].fDt, fBeamConf[beam_name].fOffsetT);
      //no need to check for subrun boundaries since pot count comes from
      //offline counting. 
      //when gps drifted away these boundaries are off
      /*
      boost::posix_time::time_duration zoneOffset = boost::posix_time::second_clock::local_time()-boost::posix_time::second_clock::universal_time();
      boost::posix_time::ptime tevnt = boost::posix_time::from_time_t(bh.getSeconds())
        + boost::posix_time::hours(zoneOffset.hours())
        + boost::posix_time::microseconds(bh.getMilliSeconds()*1000)
        + boost::posix_time::microseconds(static_cast<int>(fBeamConf[beam_name].fOffsetT*1000));
      
      if (tevnt<fSubrunT0 || tevnt>fSubrunT1) {
	mf::LogInfo(__FUNCTION__)<<"Event time not consistent with subrun begin/end "
				 <<tevnt<<"\t"<<fSubrunT0<<"\t"<<fSubrunT1;
	continue;
      }
      */
      //calculate FOM
      bnb::bnbAutoTune settings = bnb::bnbAutoTune();
      if(fUseAutoTune) settings = bmd::getSettings(fHistory, bh);
      if (fBeamConf[beam_name].fFOMversion==1) {
	fFOM=bmd::getFOM(beam_name,bh,bd);
      } else if(fBeamConf[beam_name].fFOMversion==2) {
	fFOM=bmd::getFOM2(beam_name,bh,bd, settings, fUseAutoTune);
      } else {
	mf::LogError(__FUNCTION__)<<"Unkown FOM version!";
      }
      switch (comp) {
      case -1:
	mf::LogDebug(__FUNCTION__)<<"Beam event before";
	ready_for_next_det_event=false;

	if (fBeamConf[beam_name].fWriteBeamData) 
	  fillTreeData(beam_name,bh,bd);
	addPOT(beam_name,bh,bd);

	break;
      case 0:
	mf::LogDebug(__FUNCTION__)<<"Found beam match";
	fBeamConf[beam_name].fMergedEventCount+=1;
        if (bd.size()>0) {
	  beam_info->SetRecordType(bh.getRecordType());
	  beam_info->SetSeconds(bh.getSeconds());
	  beam_info->SetMilliSeconds(bh.getMilliSeconds());
	  beam_info->SetNumberOfDevices(bh.getNumberOfDevices());
      
	  for (int i=0;i<bh.getNumberOfDevices();i++) 
	    beam_info->Set(bd[i].getDeviceName(),bd[i].getData());

	  beam_info->Set("FOM",fFOM);
	}
	ready_for_next_det_event=true;
	if (fBeamConf[beam_name].fWriteBeamData) 
	  fillTreeData(beam_name,bh,bd);
	addPOT(beam_name,bh,bd);
	break;
      case 1:
	mf::LogDebug(__FUNCTION__)<<"Beam event after";
	fBeamConf[beam_name].fNonMergedEventCount+=1;
	rewindBeamFile(beam_name, bh, bd);
	ready_for_next_det_event=true;
	break;
      }
    } else {
      ready_for_next_det_event=true;
      fBeamConf[beam_name].fNonMergedEventCount+=1;
    }
  }
  }
  e.put(std::move(beam_info));

}


bool BeamData::nextBeamEvent(std::string beamline, ub_BeamHeader &bh, std::vector<ub_BeamData> &bd)
{
  std::ifstream* file_in=fBeamConf[beamline].fBeamStream;
  bool result=true;
  try {
    std::streampos begpos=file_in->tellg();
    boost::archive::binary_iarchive ia_beam(*file_in);
    ia_beam>>bh;
    for (unsigned int i=0;i<bh.getNumberOfDevices();i++) {
      ub_BeamData bdata;
      ia_beam>>bdata;
      bd.push_back(bdata);
    }
    std::streampos endpos=file_in->tellg();
    bh.setNumberOfBytesInRecord(endpos-begpos);
  } catch ( ... ) {
    mf::LogInfo(__FUNCTION__)<<"Reached end of beam file "<<fBeamConf[beamline].fFileName;
    result=false;
  }
  mf::LogDebug(__FUNCTION__)<<"Returning beam event "<<bh.getSeconds()<<"\t"<<bh.getMilliSeconds();
  return result;
}

void BeamData::addPOT(std::string beamline, const ub_BeamHeader& bh, const std::vector<ub_BeamData>& bd)
{
  for (auto& bdata : bd) {
    for (auto it=fBeamConf[beamline].fTotalSums.begin();
	 it!=fBeamConf[beamline].fTotalSums.end();it++) {
      if (bdata.getDeviceName().find(it->first)!=std::string::npos) {
	fBeamConf[beamline].fTotalSums[it->first]+=bdata.getData()[0];
	if (fFOM>fBeamConf[beamline].fFOMcut) 
	  fBeamConf[beamline].fGoodSums[it->first]+=bdata.getData()[0];
      }
    }
  }
  fBeamConf[beamline].fTotalSpillCount+=1;
  if (fFOM>fBeamConf[beamline].fFOMcut) 
    fBeamConf[beamline].fGoodSpillCount+=1;
}
void BeamData::removePOT(std::string beam_name, const ub_BeamHeader& bh, const std::vector<ub_BeamData>& bd)
{
  for (auto& bdata : bd) {
    for (auto it=fBeamConf[beam_name].fTotalSums.begin();
	 it!=fBeamConf[beam_name].fTotalSums.end();it++) {
      if (bdata.getDeviceName().find(it->first)!=std::string::npos) {
	fBeamConf[beam_name].fTotalSums[it->first]-=bdata.getData()[0];
	if (fFOM>fBeamConf[beam_name].fFOMcut) 
	  fBeamConf[beam_name].fGoodSums[it->first]-=bdata.getData()[0];
      }
    }
  }
  fBeamConf[beam_name].fTotalSpillCount-=1;
  if (fFOM>fBeamConf[beam_name].fFOMcut) 
    fBeamConf[beam_name].fGoodSpillCount-=1;     
}
bool BeamData::rewindBeamFile(std::string beam_name, const ub_BeamHeader& bh, const std::vector<ub_BeamData>& bd)
{
  bool result=true;
  try {
    //rewind beam file
    std::streampos nbytes=bh.getNumberOfBytesInRecord();
    fBeamConf[beam_name].fBeamStream->seekg(fBeamConf[beam_name].fBeamStream->tellg()-nbytes);
  } catch ( ... ) {
    mf::LogError(__FUNCTION__)<<"Failed to rewind file "<<fBeamConf[beam_name].fFileName;
    result=false;
  }

  return result;
}

int BeamData::compareTime(ub_BeamHeader& bh, art::Event& e, float dt, float offsetT)
{
  //Return 0 if dettime and beamtime within dt milliseconds
  //       1 if dettime < beamtime 
  //      -1 if dettime > beamtime 
  art::Handle< raw::DAQHeader > daqHeaderHandle;
  e.getByLabel("daq", daqHeaderHandle);

  if ( !daqHeaderHandle.isValid() ) {
    mf::LogWarning(__FUNCTION__) << "Missing daq header info. Can't compare event time.";
    return 0;
  }
  // art::Timestamp is an unsigned long long. The conventional 
  // use is for the upper 32 bits to have the seconds since 1970 epoch 
  // and the lower 32 bits to be the number of nanoseconds within the 
  // current second.
  // (time_t is a 64 bit word)
  time_t dettime64  = daqHeaderHandle->GetTimeStamp();
   
  uint32_t detsec = uint32_t(dettime64>>32);
  uint32_t detmsec =uint32_t((dettime64 & 0xFFFFFFFF)/1000000);
    
  time_t dettime  = time_t(detsec)*1000.+time_t(detmsec)-time_t(1000.*fGPSOffset);
  time_t beamtime = time_t(bh.getSeconds())*1000 + time_t(bh.getMilliSeconds())+time_t(offsetT);

  mf::LogInfo(__FUNCTION__)<<"Detector vs beam time "<<detsec<<"\t"<<detmsec<<"\t"<<bh.getSeconds()<<"\t"<<bh.getMilliSeconds()<<"\t"<<dettime-beamtime;
  int comp=1;
  if ( dettime>=beamtime && dettime - beamtime < dt ) {comp = 0;}
  else if ( dettime<beamtime && beamtime - dettime < dt ) {comp = 0;}
  else if (dettime < beamtime ) comp =1;
  else comp = -1;

  return comp;
}

void BeamData::fillTreeData(std::string beam, const ub_BeamHeader& bh, const std::vector<ub_BeamData>& bd)
{
  mf::LogDebug(__FUNCTION__)<<"Filling "<<beam<<" ntuple";
  fSeconds=bh.getSeconds();
  fMilliSeconds=bh.getMilliSeconds();

  boost::posix_time::time_duration zoneOffset = boost::posix_time::second_clock::local_time()-boost::posix_time::second_clock::universal_time();

  boost::posix_time::ptime tevnt= boost::posix_time::from_time_t(fSeconds)+ boost::posix_time::hours(zoneOffset.hours())+ boost::posix_time::microseconds(fMilliSeconds*1000);

  if (!fTLast[beam].is_not_a_date_time() && tevnt<fTLast[beam]) {
    mf::LogInfo(__FUNCTION__)<<"Event already written";
    return;
  } 

  for (int i=0;i<bh.getNumberOfDevices();i++) {
    std::string varname=bd[i].getDeviceName();
    varname.erase(std::remove(varname.begin(), varname.end(), ':'), varname.end());
    if (!(fBeamConf[beam].fTreeVar.find(varname)==fBeamConf[beam].fTreeVar.end() &&
	  fBeamConf[beam].fTreeArr.find(varname)==fBeamConf[beam].fTreeArr.end())) {
      if (bd[i].getData().size()==1) {

	fBeamConf[beam].fTreeVar[varname]=bd[i].getData()[0];
      } else {
	for (unsigned int j=0;j<bd[i].getData().size();j++) {
	  fBeamConf[beam].fTreeArr[varname][j]=bd[i].getData()[j];
	}
      }
    } else {
      std::stringstream ss;
      if (bd[i].getData().size()==1) {
	fBeamConf[beam].fTreeVar[varname]=-999;
	ss <<"Adding scalar branch while filling for device "<<varname<<" and filling first "<<fBeamConf[beam].fTree->GetEntries()<<" entries with -999"<<std::endl;
	fBeamConf[beam].fTree->Branch(varname.c_str(),&fBeamConf[beam].fTreeVar[varname],
				      (varname+"/D").c_str());
	for (int ientry=0;ientry<fBeamConf[beam].fTree->GetEntries();ientry++) 
	  fBeamConf[beam].fTree->GetBranch(varname.c_str())->Fill();
	fBeamConf[beam].fTreeVar[varname]=bd[i].getData()[0];
      } else {
	double* x=new double[bd[i].getData().size()];
	for (unsigned int ii=0;ii<bd[i].getData().size();ii++) x[ii]=-999;
	fBeamConf[beam].fTreeArr[varname]=x;
	ss <<"Adding vector branch while filling for device "<<varname<<" and filling first "<<fBeamConf[beam].fTree->GetEntries()<<" entries with -999"<<std::endl;
	fBeamConf[beam].fTree->Branch(varname.c_str(),fBeamConf[beam].fTreeArr[varname],
				      (varname+"["+std::to_string(bd[i].getData().size())+"]/D").c_str());
	for (int ientry=0;ientry<fBeamConf[beam].fTree->GetEntries();ientry++) {
	  fBeamConf[beam].fTree->GetBranch(varname.c_str())->Fill();
	}
	for (unsigned int ii=0;ii<bd[i].getData().size();ii++) x[ii]=bd[i].getData()[ii];;
      }
      mf::LogDebug(__FUNCTION__)<<ss.str();
    }

  }
  fBeamConf[beam].fTree->Fill();
  fTLast[beam]=tevnt;
}

void BeamData::createBranches(std::string beam) 
{
  //read events and figure out which variables to output
  //and if a var is an array
  std::map<std::string, unsigned int> vars;
  std::vector<std::streampos> rwbyt;
  bool ret=true;
  while (ret) {
    ub_BeamHeader bh;
    std::vector<ub_BeamData> bd;
    ret=nextBeamEvent(beam, bh,bd); 
    for (int i=0;i<bh.getNumberOfDevices();i++) {
      std::string varname=bd[i].getDeviceName();
      varname.erase(std::remove(varname.begin(), varname.end(), ':'), varname.end());
      std::pair<std::string,unsigned int> p(varname, bd[i].getData().size());
      vars.insert(p);
    }    
    rwbyt.push_back(bh.getNumberOfBytesInRecord());
  }
  
  // This does not work ???
  //  for (unsigned int i=rwbyt.size()-1;i>=0;i--) {
  //  rewindBeamFile(beam,rwbyt[i]);
  // }

  fBeamConf[beam].fBeamStream->clear();
  fBeamConf[beam].fBeamStream->seekg(0);
  fBeamConf[beam].fTotalSpillCount=0;
  fBeamConf[beam].fGoodSpillCount=0;
  for (auto it=fBeamConf[beam].fTotalSums.begin();
       it!=fBeamConf[beam].fTotalSums.end();it++) {
    it->second=0;
  }
  for (auto it=fBeamConf[beam].fGoodSums.begin();
       it!=fBeamConf[beam].fGoodSums.end();it++) {
    it->second=0;
  }

  fBeamConf[beam].fTree->Branch("run",&fRun,"run/i");
  fBeamConf[beam].fTree->Branch("subrun",&fSubRun,"subrun/i");
  fBeamConf[beam].fTree->Branch("seconds",&fSeconds,"seconds/i");
  fBeamConf[beam].fTree->Branch("milliseconds",&fMilliSeconds,"milliseconds/i");
  fBeamConf[beam].fTree->Branch("FOM",&fFOM,"FOM/F");

  std::stringstream ss;
  for (auto& it : vars) {
    // ss<<it.first<<"\t"<<it.second<<std::endl;
    if (it.second==1) {
	fBeamConf[beam].fTreeVar[it.first]=-999;
	ss <<"Creating scalar branch for device "<<it.first.c_str()<<std::endl;
	fBeamConf[beam].fTree->Branch(it.first.c_str(),&fBeamConf[beam].fTreeVar[it.first],
				      (it.first+"/D").c_str());
      } else {
	double* x=new double[it.second];
	fBeamConf[beam].fTreeArr[it.first]=x;
	ss <<"Creating vector branch for device "<<it.first.c_str()<<std::endl;
	fBeamConf[beam].fTree->Branch(it.first.c_str(),fBeamConf[beam].fTreeArr[it.first],
				      (it.first+"["+std::to_string(it.second)+"]/D").c_str());
      }

  }
  mf::LogDebug(__FUNCTION__)<<ss.str();
  fCreateBranches[beam]=false;
}

DEFINE_ART_MODULE(BeamData)
