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
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "RawData/BeamInfo.h"
#include "RawData/DAQHeader.h"
#include "RawData/TriggerData.h"
#include "datatypes/raw_data_access.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <memory>

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
  void endSubRun(art::SubRun & sr) override;
  void endJob() override;
  void produce(art::Event & e) override;
 
  bool nextBeamEvent(std::string beamline, ub_BeamHeader &bh, std::vector<ub_BeamData> &bd);
  int compareTime(ub_BeamHeader& bh, art::Event& e, float dt);
  float getFOM(ub_BeamHeader& bh, ub_BeamData& bd);

private:

  // Declare member data here.
  struct BeamConf_t {
    int fBeamID;
    std::ifstream* fBeamStream;
    std::string fFileName;
    std::map<std::string, float> fSums;
    uint32_t fTotalEventCount;
    uint32_t fMergedEventCount;
    float fDt;
  };

  std::vector<std::string> fBeams;
  std::map<std::string, BeamConf_t>  fBeamConf;
  
};


BeamData::BeamData(fhicl::ParameterSet const & p)
// :
// Initialize member data here.
{
  // Call appropriate produces<>() functions here.

  produces< raw::BeamInfo >();  

  fBeams=p.get<std::vector<std::string> >("beams");
  for (uint i=0;i<fBeams.size();i++) {
    fhicl::ParameterSet pbeam=p.get<fhicl::ParameterSet>(fBeams[i]);
    BeamConf_t bconf;
    bconf.fBeamID=i;
    std::vector<std::string> sum_devices=pbeam.get<std::vector<std::string> >("sum_devices");
    for (auto it=sum_devices.begin();it!=sum_devices.end();it++) {
      bconf.fSums[*it]=0;
    }
    bconf.fDt=pbeam.get<float>("merge_time_tolerance");
    std::pair<std::string, BeamConf_t> p(fBeams[i],bconf);
    fBeamConf.insert(p);
  }
}

void BeamData::beginSubRun(art::SubRun & sr)
{
  //load beam file


  mf::LogInfo(__FUNCTION__)<<"Open beam files for run "<<sr.run()
			   <<" subrun "<<sr.subRun();
  
  for (uint ibeam=0;ibeam<fBeams.size();ibeam++) {
    std::stringstream fname;
    fname<<"beam_"<<fBeams[ibeam]<<"_"
	 <<std::setfill('0') << std::setw(7) << sr.run()<<"_"
	 <<std::setfill('0') << std::setw(4) << sr.subRun()<<".dat";
    mf::LogInfo(__FUNCTION__)<< "Opening beam file " << fname.str();
    ifstream *fin=new ifstream(fname.str(), std::ios::binary);
   
    if ( !fin->is_open() ) {
      mf::LogError(__FUNCTION__) <<"Can't open beam file "<<fname.str();
      fBeamConf.erase(fBeams[ibeam]);
      delete fin;
    } else {      
      fBeamConf[fBeams[ibeam]].fFileName=fname.str();
      fBeamConf[fBeams[ibeam]].fTotalEventCount=0;
      fBeamConf[fBeams[ibeam]].fMergedEventCount=0;
      fBeamConf[fBeams[ibeam]].fBeamStream=fin;
    }
  }
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
      mf::LogInfo(__FUNCTION__)<<bh.debugInfo();
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
  for (int i=0;i<100;i++) ss<<"=";
  ss<<std::endl;  
  ss<<std::setw(10)<<"Beam"
    <<std::setw(50)<<"File"
    <<std::setw(20)<<"Merged Events"
    <<std::setw(20)<<"Total Events"
    <<std::endl;
  for (int i=0;i<100;i++) ss<<"-";
  ss<<std::endl;
  for (auto& it : fBeamConf ) {
    ss<<std::setw(10)<<fBeams[it.second.fBeamID]
      <<std::setw(50)<<it.second.fFileName
      <<std::setw(20)<<it.second.fMergedEventCount
      <<std::setw(20)<<it.second.fTotalEventCount
      <<std::endl;
  }
  for (int i=0;i<100;i++) ss<<"=";
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
    for (auto& itdev : it.second.fSums) {
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
      mf::LogWarning(__FUNCTION__) <<"Found BNB event, but bnb not selected in the fcl file.";
      return;
    }
  } else if (trigInfo[0]->TriggerBits() & 0x1000) {
    mf::LogDebug(__FUNCTION__) <<"Looking for NuMI event";
    if (fBeamConf.find("numi")!=fBeamConf.end() )
      beam_name="numi";
    else {
      mf::LogWarning(__FUNCTION__)<< "Found NuMI event, but numi not selected in the fcl file.";  
      return;
    }
  }

  bool ready_for_next_det_event=false;
  while ( !ready_for_next_det_event ) {
    ub_BeamHeader bh;
    std::vector<ub_BeamData> bd;
    ready_for_next_det_event=true;
    if (nextBeamEvent(beam_name,bh,bd)) {
      int comp=compareTime(bh,e,fBeamConf[beam_name].fDt);
      switch (comp) {
      case -1:
	mf::LogDebug(__FUNCTION__)<<"Beam event before";
	ready_for_next_det_event=false;
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
	}
	break;
      case 1:
	mf::LogDebug(__FUNCTION__)<<"Beam event after";
	//rewind beam file
	fBeamConf[beam_name].fBeamStream->seekg(
				    fBeamConf[beam_name].fBeamStream->tellg()
				    -std::streampos(bh.getNumberOfBytesInRecord()));
	fBeamConf[beam_name].fTotalEventCount-=1;
	break;
      }
    }
  }
  
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
      for (auto it=fBeamConf[beamline].fSums.begin();
	   it!=fBeamConf[beamline].fSums.end();it++) {
	if (bdata.getDeviceName().find(it->first)!=std::string::npos) {
	  fBeamConf[beamline].fSums[it->first]+=bdata.getData()[0];
	}
      }
      bd.push_back(bdata);
    }
    std::streampos endpos=file_in->tellg();
    bh.setNumberOfBytesInRecord(endpos-begpos);
  } catch ( ... ) {
    mf::LogInfo()<<"Reached end of beam file";
    result=false;
  }
  fBeamConf[beamline].fTotalEventCount+=1;
  return result;
}

int BeamData::compareTime(ub_BeamHeader& bh, art::Event& e, float dt)
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
  time_t beamtime = time_t(bh.getSeconds())*1000 + time_t(bh.getMilliSeconds())-2390;
  mf::LogInfo(__FUNCTION__)<<detsec<<"\t"<<detmsec<<"\t"
			   <<beamsec<<"\t"<<beammsec;
  mf::LogInfo(__FUNCTION__)<<dettime<<" - "<<beamtime<<" = "
			   <<dettime-beamtime;
  int comp=1;
  if ( dettime>=beamtime && dettime - beamtime < dt ) {comp = 0;}
  else if ( dettime<beamtime && beamtime - dettime < dt ) {comp = 0;}
  else if (dettime < beamtime ) comp =1;
  else comp = -1;

  return comp;
}

float BeamData::getFOM(ub_BeamHeader& bh, ub_BeamData& bd)
{
  return 1.0;
}

DEFINE_ART_MODULE(BeamData)
