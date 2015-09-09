////////////////////////////////////////////////////////////////////////
// Class:       BeamData
// Module Type: producer
// File:        BeamData_module.cc
//
// Generated at Thu Sep  3 16:51:01 2015 by Zarko Pavlovic using artmod
// from cetpkgsupport v1_08_06.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Utilities/InputTag.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"

#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "SummaryData/POTSummary.h"
#include "RawData/BeamInfo.h"
#include "RawData/DAQHeader.h"
#include "RawData/TriggerData.h"
#include "datatypes/raw_data_access.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <memory>

#include "TTree.h"

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
 
  bool nextBeamEvent(std::string beamline, ub_BeamHeader &bh, std::vector<ub_BeamData> &bd);
  bool rewindBeamFile(std::string beam, const ub_BeamHeader& bh, const std::vector<ub_BeamData> &bd); //goes back one event in the file
  int compareTime(ub_BeamHeader& bh, art::Event& e, float dt, float offsetT);
  void createBranches(std::string beam);
  void fillTreeData(std::string beam, const ub_BeamHeader& bh, const std::vector<ub_BeamData>& bd);

  float getFOM(std::string beam, const ub_BeamHeader& bh, const std::vector<ub_BeamData>& bd);

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
    std::string fTriggerMask;
    bool fWriteBeamData;
    float fFOMcut;
    TTree* fTree;
    std::map<std::string, float> fTreeVar;
    std::map<std::string, float*> fTreeArr;
  };

  std::vector<std::string> fBeams;
  std::map<std::string, BeamConf_t>  fBeamConf;
  int fRun;
  int fSubRun;
  long long fTimestamp;
  float fFOM;

};

BeamData::BeamData(fhicl::ParameterSet const & p)
// :
// Initialize member data here.
{
  // Call appropriate produces<>() functions here.
  fBeams=p.get<std::vector<std::string> >("beams");
  for (uint i=0;i<fBeams.size();i++) {
    fhicl::ParameterSet pbeam=p.get<fhicl::ParameterSet>(fBeams[i]);
    BeamConf_t bconf;
    bconf.fBeamID=i;
    std::vector<std::string> sum_devices=pbeam.get<std::vector<std::string> >("sum_devices");
    for (auto it=sum_devices.begin();it!=sum_devices.end();it++) {
      bconf.fTotalSums[*it]=0;
      bconf.fGoodSums[*it]=0;
    }
    bconf.fTriggerMask=pbeam.get<std::string>("trigger_mask");
    bconf.fOffsetT=pbeam.get<float>("time_offset");
    bconf.fDt=pbeam.get<float>("merge_time_tolerance");
    bconf.fFOMcut=pbeam.get<float>("FOM_cut");
    bconf.fFilePath=pbeam.get<std::string>("path_to_beam_file");
    bconf.fWriteBeamData=pbeam.get<bool>("write_beam_data");
    std::pair<std::string, BeamConf_t> p(fBeams[i],bconf);
    fBeamConf.insert(p);
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
  //load beam file
  fRun=sr.run();
  fSubRun=sr.subRun();
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
  for (uint ibeam=0;ibeam<fBeams.size();ibeam++) {
    std::stringstream fname;
    fname<<"beam_"<<fBeams[ibeam]<<"_"
	 <<std::setfill('0') << std::setw(7) << fRun<<"_"
	 <<std::setfill('0') << std::setw(5) << fSubRun<<".dat";
    ss<<std::setw(10)<<fBeams[ibeam]
      <<std::setw(40)<<fname.str()
      <<std::setw(30)<<fBeamConf[fBeams[ibeam]].fOffsetT
      <<std::setw(30)<<fBeamConf[fBeams[ibeam]].fDt;
    ifstream *fin=new ifstream(fBeamConf[fBeams[ibeam]].fFilePath+
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
      fBeamConf[fBeams[ibeam]].fNonMergedEventCount=0;
      fBeamConf[fBeams[ibeam]].fMergedEventCount=0;
      fBeamConf[fBeams[ibeam]].fBeamStream=fin;
      ss<<std::setw(10)<<"Yes";
    }
    ss<<std::endl;
  }
  for (int i=0;i<120;i++) ss<<"=";
  ss<<std::endl;
  mf::LogInfo(__FUNCTION__) <<ss.str();

  for (auto& it : fBeamConf) 
    if (it.second.fWriteBeamData) createBranches(it.first);
}

void BeamData::endSubRun(art::SubRun & sr)
{
  //loop over remaining beam events

  for (auto it=fBeamConf.begin();it!=fBeamConf.end();it++) {
    mf::LogInfo(__FUNCTION__) <<"Loop through the remaining "
			      <<it->first<<" beam events";
    ub_BeamHeader bh;
    std::vector<ub_BeamData> bd;
    while (nextBeamEvent(it->first,bh,bd)) {
      if (fBeamConf[it->first].fWriteBeamData) 
	fillTreeData(it->first,bh,bd);
      mf::LogInfo(__FUNCTION__)<<bh.debugInfo();
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
      sr.put(std::move(pot),varname);   
    }
  }

  for (auto it=fBeamConf.begin();it!=fBeamConf.end();it++) {
    (it->second).fBeamStream->close();
    delete (it->second).fBeamStream;
  }
  
}

void BeamData::endJob()
{
  std::stringstream ss;
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
  for (int i=0;i<60;i++) ss<<"=";
  ss<<std::endl;
  ss<<std::setw(20)<<"Beam"
    <<std::setw(20)<<"Device"
    <<std::setw(20)<<"Sum"
    <<std::endl;
  for (int i=0;i<60;i++) ss<<"-";
  ss<<std::endl;
  for (auto& it : fBeamConf ) {
    ss<<std::setw(20)<<it.first;
    bool ifirst=true;
    for (auto& itdev : it.second.fGoodSums) {
      if (!ifirst) ss<<std::setw(20)<<" ";	
      ss<<std::setw(20)<<itdev.first
	<<std::setw(20)<<itdev.second
	<<std::endl;
      ifirst=false;
    }      
  }
  for (int i=0;i<60;i++) ss<<"=";
  ss<<std::endl;
  mf::LogInfo(__FUNCTION__)<<ss.str();
  fBeamConf.clear();
}

void BeamData::produce(art::Event & e)
{
  // Implementation of required member function here.
  std::unique_ptr<raw::BeamInfo> beam_info(new raw::BeamInfo);
  
  art::Handle< std::vector<raw::Trigger> > triggerHandle;
  std::vector<art::Ptr<raw::Trigger> > trigInfo;
  if (e.getByLabel("daq", triggerHandle))
    art::fill_ptr_vector(trigInfo, triggerHandle);
  else {
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
 
  //  if (!(trigInfo[0]->TriggerBits() & 0x800) && //gate2
  if (!(trigInfo[0]->TriggerBits() & 0x200) && //external
      !(trigInfo[0]->TriggerBits() & 0x1000) ) {
    mf::LogInfo(__FUNCTION__) << "Non beam event... Skipping.";
    return;
  }
  std::string beam_name;
  //  if (trigInfo[0]->TriggerBits() & 0x800 ) {
  if (trigInfo[0]->TriggerBits() & 0x200 ) {//actually external
    mf::LogDebug(__FUNCTION__) <<"Looking for BNB event";
    if (fBeamConf.find("bnb")!=fBeamConf.end() )
      beam_name="bnb";
    else {
      mf::LogError(__FUNCTION__) <<"Found BNB event, but bnb not selected in the fcl file of beam file missing.";
      return;
    }
  } else if (trigInfo[0]->TriggerBits() & 0x1000) {
    mf::LogDebug(__FUNCTION__) <<"Looking for NuMI event";
    if (fBeamConf.find("numi")!=fBeamConf.end() )
      beam_name="numi";
    else {
      mf::LogError(__FUNCTION__)<< "Found NuMI event, but numi not selected in the fcl file or beam file missing.";  
      return;
    }
  }

  bool ready_for_next_det_event=false;
  while ( !ready_for_next_det_event ) {
    ub_BeamHeader bh;
    std::vector<ub_BeamData> bd;
    if (nextBeamEvent(beam_name,bh,bd)) {
      int comp=compareTime(bh,e,fBeamConf[beam_name].fDt, fBeamConf[beam_name].fOffsetT);
      switch (comp) {
      case -1:
	mf::LogInfo(__FUNCTION__)<<"Beam event before";
	ready_for_next_det_event=false;
	if (fBeamConf[beam_name].fWriteBeamData) 
	  fillTreeData(beam_name,bh,bd);
	break;
      case 0:
	mf::LogInfo(__FUNCTION__)<<"Found beam match";
	fBeamConf[beam_name].fMergedEventCount+=1;
        if (bd.size()>0) {
	  beam_info->SetRecordType(bh.getRecordType());
	  beam_info->SetSeconds(bh.getSeconds());
	  beam_info->SetMilliSeconds(bh.getMilliSeconds());
	  beam_info->SetNumberOfDevices(bh.getNumberOfDevices());
      
	  for (int i=0;i<bh.getNumberOfDevices();i++) 
	    beam_info->Set(bd[i].getDeviceName(),bd[i].getData());
	}
	ready_for_next_det_event=true;
	if (fBeamConf[beam_name].fWriteBeamData) 
	  fillTreeData(beam_name,bh,bd);
	break;
      case 1:
	mf::LogInfo(__FUNCTION__)<<"Beam event after";
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
    for (uint i=0;i<bh.getNumberOfDevices();i++) {
      ub_BeamData bdata;
      ia_beam>>bdata;
      fFOM=getFOM(beamline,bh,bd);
      for (auto it=fBeamConf[beamline].fTotalSums.begin();
	   it!=fBeamConf[beamline].fTotalSums.end();it++) {
	if (bdata.getDeviceName().find(it->first)!=std::string::npos) {
	  fBeamConf[beamline].fTotalSums[it->first]+=bdata.getData()[0];
	  if (fFOM>fBeamConf[beamline].fFOMcut) 
	    fBeamConf[beamline].fGoodSums[it->first]+=bdata.getData()[0];
	}
      }
      bd.push_back(bdata);
    }
    std::streampos endpos=file_in->tellg();
    bh.setNumberOfBytesInRecord(endpos-begpos);
    fBeamConf[beamline].fTotalSpillCount+=1;
    if (fFOM>fBeamConf[beamline].fFOMcut) 
      fBeamConf[beamline].fGoodSpillCount+=1;
  } catch ( ... ) {
    mf::LogInfo()<<"Reached end of beam file "<<fBeamConf[beamline].fFileName;
    result=false;
  }
  return result;
}

bool BeamData::rewindBeamFile(std::string beam_name, const ub_BeamHeader& bh, const std::vector<ub_BeamData>& bd)
{
  bool result=true;
  try {
    //rewind beam file
    std::streampos nbytes=bh.getNumberOfBytesInRecord();
    fBeamConf[beam_name].fBeamStream->seekg(fBeamConf[beam_name].fBeamStream->tellg()-nbytes);
    fBeamConf[beam_name].fTotalSpillCount-=1;
    if (getFOM(beam_name,bh,bd)>fBeamConf[beam_name].fFOMcut) 
      fBeamConf[beam_name].fGoodSpillCount-=1;
  } catch ( ... ) {
    mf::LogError()<<"Failed to rewind file "<<fBeamConf[beam_name].fFileName;
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
  uint32_t detmsec =uint32_t((dettime64 & 0x0000FFFF)/1000);
  
  uint32_t beamsec = bh.getSeconds();
  uint32_t beammsec = bh.getMilliSeconds();
  
  time_t dettime  = time_t(detsec)*1000.+time_t(detmsec);
  time_t beamtime = time_t(bh.getSeconds())*1000 + time_t(bh.getMilliSeconds())+time_t(offsetT);
  
  mf::LogInfo(__FUNCTION__)<<detsec<<"\t"<<detmsec<<"\t"
			   <<beamsec<<"\t"<<beammsec<<"\t"<<offsetT;
  mf::LogInfo(__FUNCTION__)<<dettime<<" - "<<beamtime<<" = "
			   <<dettime-beamtime;
  int comp=1;
  if ( dettime>=beamtime && dettime - beamtime < dt ) {comp = 0;}
  else if ( dettime<beamtime && beamtime - dettime < dt ) {comp = 0;}
  else if (dettime < beamtime ) comp =1;
  else comp = -1;

  return comp;
}

float BeamData::getFOM(std::string beam, const ub_BeamHeader& bh, const std::vector<ub_BeamData>& bd)
{
  return 1.0;
}
void BeamData::fillTreeData(std::string beam, const ub_BeamHeader& bh, const std::vector<ub_BeamData>& bd)
{
  mf::LogInfo(__FUNCTION__)<<"==========> Writing beam event <==========";
  fFOM=getFOM(beam, bh,bd);
  fTimestamp=bh.getSeconds()*1000+bh.getMilliSeconds();

  for (int i=0;i<bh.getNumberOfDevices();i++) {
    std::string varname=bd[i].getDeviceName();
    varname.erase(std::remove(varname.begin(), varname.end(), ':'), varname.end());
    if (bd[i].getData().size()==1) {
      fBeamConf[beam].fTreeVar[varname]=bd[i].getData()[0];
    } else {
      for (uint j=0;j<bd[i].getData().size();j++) {
	fBeamConf[beam].fTreeArr[varname][j]=bd[i].getData()[j];
      }
    }
  }
  fBeamConf[beam].fTree->Fill();
}

void BeamData::createBranches(std::string beam) 
{
  mf::LogInfo(__FUNCTION__) <<"===> Creating branches for "<<beam;

  //read 10 events and figure out which variables to output
  //and if a var is an array
  std::map<std::string, uint> vars;
  std::vector<std::streampos> rwbyt;
  for (int iev=0;iev<10;iev++) {
    ub_BeamHeader bh;
    std::vector<ub_BeamData> bd;
    if (nextBeamEvent(beam, bh,bd)) {
      for (int i=0;i<bh.getNumberOfDevices();i++) {
	std::string varname=bd[i].getDeviceName();
	varname.erase(std::remove(varname.begin(), varname.end(), ':'), varname.end());
	std::pair<std::string,uint> p(varname, bd[i].getData().size());
	vars.insert(p);
      }    
      rwbyt.push_back(bh.getNumberOfBytesInRecord());
    }
  }

  // This does not work ???
  //  for (uint i=rwbyt.size()-1;i>=0;i--) {
  //  rewindBeamFile(beam,rwbyt[i]);
  // }

  fBeamConf[beam].fBeamStream->clear();
  fBeamConf[beam].fBeamStream->seekg(0);
  fBeamConf[beam].fTotalSpillCount=0;
  fBeamConf[beam].fGoodSpillCount=0;

  fBeamConf[beam].fTree->Branch("run",&fRun,"run/I");
  fBeamConf[beam].fTree->Branch("subrun",&fSubRun,"subrun/I");
  fBeamConf[beam].fTree->Branch("timestamp",&fTimestamp,"timestamp/L");
  fBeamConf[beam].fTree->Branch("FOM",&fFOM,"FOM/F");

  mf::LogInfo(__FUNCTION__) <<"Now print the device names";
  std::stringstream ss;
  for (auto& it : vars) {
    // ss<<it.first<<"\t"<<it.second<<std::endl;
    if (it.second==1) {
	fBeamConf[beam].fTreeVar[it.first]=-999;
	ss <<"Creating scalar branch for device "<<it.first.c_str()<<std::endl;
	fBeamConf[beam].fTree->Branch(it.first.c_str(),&fBeamConf[beam].fTreeVar[it.first],
				      (it.first+"/F").c_str());
      } else {
	float* x=new float[it.second];
	fBeamConf[beam].fTreeArr[it.first]=x;
	ss <<"Creating vector branch for device "<<it.first.c_str()<<std::endl;
	fBeamConf[beam].fTree->Branch(it.first.c_str(),&fBeamConf[beam].fTreeArr[it.first],
				      (it.first+"["+std::to_string(it.second)+"]/F").c_str());
      }

  }
  mf::LogInfo(__FUNCTION__)<<ss.str();
  
}

DEFINE_ART_MODULE(BeamData)
