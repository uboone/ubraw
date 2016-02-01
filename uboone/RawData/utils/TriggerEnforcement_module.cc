////////////////////////////////////////////////////////////////////////
// Class:       TriggerEnforcement
// Module Type: filter
// File:        TriggerEnforcement_module.cc
//
// Generated at Wed Oct  7 12:33:08 2015 by Kazuhiro Terao using artmod
// from cetpkgsupport v1_08_06.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "Utilities/TimeService.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <memory>
#include "RawData/TriggerData.h"
#include "uboone/RawData/utils/ubdaqSoftwareTriggerData.h"
#include "uboone/TriggerSim/UBTriggerTypes.h"
#include <string>

class TriggerEnforcement;

class TriggerEnforcement : public art::EDFilter {
public:
  explicit TriggerEnforcement(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  TriggerEnforcement(TriggerEnforcement const &) = delete;
  TriggerEnforcement(TriggerEnforcement &&) = delete;
  TriggerEnforcement & operator = (TriggerEnforcement const &) = delete;
  TriggerEnforcement & operator = (TriggerEnforcement &&) = delete;

  // Required functions.
  bool filter(art::Event & e) override;

private:

  // Declare member data here.
  const size_t _bit_v_size;
  std::vector<bool> _exclude_bit_v;
  std::vector<bool> _include_bit_v;
  std::vector<std::string> _include_software_trig_v;
  std::vector<std::string> _exclude_software_trig_v;
  std::string _hardware_trigger_producer;
  std::string _software_trigger_producer;

  bool _verbose;

};


TriggerEnforcement::TriggerEnforcement(fhicl::ParameterSet const & p)
: _bit_v_size(32)
{

  _hardware_trigger_producer = p.get<std::string>("HardwareTriggerProducer");
  _software_trigger_producer = p.get<std::string>("SoftwareTriggerProducer");
  _verbose = p.get<bool>("Verbose");
  _exclude_bit_v.clear();
  _exclude_bit_v.resize(_bit_v_size,false);
  for(auto const& name : p.get<std::vector<std::string> >("ExcludeBits")) {

    if(_verbose) std::cout<<"\033[93m["<<__FUNCTION__<<"]\033[00m excluding: "<<name.c_str()<<std::endl;
    
    if      ( name == "PMTTriggerBeam"   ) _exclude_bit_v.at(trigger::kPMTTriggerBeam   ) = true;
    else if ( name == "PMTTriggerCosmic" ) _exclude_bit_v.at(trigger::kPMTTriggerCosmic ) = true;
    else if ( name == "PMTTrigger"       ) _exclude_bit_v.at(trigger::kPMTTrigger       ) = true;
    else if ( name == "TriggerPC"        ) _exclude_bit_v.at(trigger::kTriggerPC        ) = true;
    else if ( name == "TriggerEXT"       ) _exclude_bit_v.at(trigger::kTriggerEXT       ) = true;
    else if ( name == "TriggerBNB"       ) _exclude_bit_v.at(trigger::kTriggerBNB       ) = true;
    else if ( name == "TriggerNuMI"      ) _exclude_bit_v.at(trigger::kTriggerNuMI      ) = true;
    else if ( name == "Veto"             ) _exclude_bit_v.at(trigger::kVeto             ) = true;
    else if ( name == "TriggerCalib"     ) _exclude_bit_v.at(trigger::kTriggerCalib     ) = true;
    else if ( name == "FakeGate"         ) _exclude_bit_v.at(trigger::kFakeGate         ) = true;
    else if ( name == "FakeBeam"         ) _exclude_bit_v.at(trigger::kFakeBeam         ) = true;
    else if ( name == "Spare"            ) _exclude_bit_v.at(trigger::kSpare            ) = true;
    else {
      std::cerr<<"\033[93mInvalid TriggerBit Name: "<<name.c_str()<<std::endl;
      throw std::exception();
    }
    
  }
  _include_bit_v.clear();
  _include_bit_v.resize(_bit_v_size,false);
  for(auto const& name : p.get<std::vector<std::string> >("IncludeBits")) {

    if(_verbose) std::cout<<"\033[93m["<<__FUNCTION__<<"]\033[00m including: "<<name.c_str()<<std::endl;
    
    if      ( name == "PMTTriggerBeam"   ) _include_bit_v.at(trigger::kPMTTriggerBeam   ) = true;
    else if ( name == "PMTTriggerCosmic" ) _include_bit_v.at(trigger::kPMTTriggerCosmic ) = true;
    else if ( name == "PMTTrigger"       ) _include_bit_v.at(trigger::kPMTTrigger       ) = true;
    else if ( name == "TriggerPC"        ) _include_bit_v.at(trigger::kTriggerPC        ) = true;
    else if ( name == "TriggerEXT"       ) _include_bit_v.at(trigger::kTriggerEXT       ) = true;
    else if ( name == "TriggerBNB"       ) _include_bit_v.at(trigger::kTriggerBNB       ) = true;
    else if ( name == "TriggerNuMI"      ) _include_bit_v.at(trigger::kTriggerNuMI      ) = true;
    else if ( name == "Veto"             ) _include_bit_v.at(trigger::kVeto             ) = true;
    else if ( name == "TriggerCalib"     ) _include_bit_v.at(trigger::kTriggerCalib     ) = true;
    else if ( name == "FakeGate"         ) _include_bit_v.at(trigger::kFakeGate         ) = true;
    else if ( name == "FakeBeam"         ) _include_bit_v.at(trigger::kFakeBeam         ) = true;
    else if ( name == "Spare"            ) _include_bit_v.at(trigger::kSpare            ) = true;
    else {
      std::cerr<<"\033[93mInvalid TriggerBit Name: "<<name.c_str()<<std::endl;
      throw std::exception();
    }
    
    _include_software_trig_v = p.get<std::vector<std::string> >("includeSoftwareTriggers");

    _exclude_software_trig_v = p.get<std::vector<std::string> >("excludeSoftwareTriggers");

  }
  if(_verbose) {
    std::cout<<"\033[93m["<<__FUNCTION__<<"]\033[00m bits to be excluded: ";
    for(size_t i=0; i<_exclude_bit_v.size(); ++i)
      if(_exclude_bit_v[i]) std::cout<< i << " ";
    std::cout<<std::endl;
    std::cout<<"\033[93m["<<__FUNCTION__<<"]\033[00m bits to be included after exclusion applied: ";
    for(size_t i=0; i<_include_bit_v.size(); ++i)
      if(_include_bit_v[i]) std::cout<< i << " ";
    std::cout<<std::endl;
      
    for (auto const& name : _exclude_software_trig_v) {
      std::cout<<"\033[93m["<<__FUNCTION__<<"]\033[00m excluding: "<<name.c_str()<<std::endl;
    }
    for (auto const& name : _include_software_trig_v) {
      std::cout<<"\033[93m["<<__FUNCTION__<<"]\033[00m including after exclusion: "<<name.c_str()<<std::endl;
    }
  }
}

bool TriggerEnforcement::filter(art::Event & e)
{
  ::art::ServiceHandle< util::TimeService > ts;

  bool hardware_decision=false;
  bool software_decision=false;

  ts->preProcessEvent(e);

  if(_hardware_trigger_producer.empty()) hardware_decision=true;
  if(_software_trigger_producer.empty()) software_decision=true;

  if (hardware_decision && software_decision) return true;

  art::Handle<std::vector<raw::Trigger> > hardware_trigger_handle;
  e.getByLabel(_hardware_trigger_producer,hardware_trigger_handle);
  art::Handle<raw::ubdaqSoftwareTriggerData> software_trigger_handle;
  e.getByLabel(_software_trigger_producer,software_trigger_handle);

  if(!hardware_trigger_handle.isValid()) {
    std::cerr<<"\033[93mInvalid Producer Label: \033[00m" <<_hardware_trigger_producer.c_str()<<std::endl;
    throw std::exception();
  }

  if(!software_trigger_handle.isValid()) {
    std::cerr<<"\033[93mInvalid Producer Label: \033[00m" <<_software_trigger_producer.c_str()<<std::endl;
    throw std::exception();
  }

  for(auto const& t : *hardware_trigger_handle) {

    for(size_t i=0; i<_bit_v_size; ++i) {

      unsigned char bit_index = (unsigned char)i;
      if(t.Triggered(bit_index) && _exclude_bit_v[i]) {
	if(_verbose) std::cout<<"Excluding by the bit: "<< i <<std::endl;
	return false;
      }

      if(hardware_decision) continue;
      if(t.Triggered(bit_index) && _include_bit_v[i]) {
	if(_verbose) std::cout<<"Including by the bit: "<< i <<std::endl;
	hardware_decision = true;
      }

    }
  }

  if(hardware_decision) {
    if( software_trigger_handle->vetoAlgos( _exclude_software_trig_v) )
      return false;

    if( software_trigger_handle->passedAlgos( _include_software_trig_v) )
      software_decision=true;
  }
    
  return (software_decision && hardware_decision) ;
}

DEFINE_ART_MODULE(TriggerEnforcement)
