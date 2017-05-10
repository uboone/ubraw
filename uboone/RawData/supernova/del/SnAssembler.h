#ifndef SNASSEMBLER_H_E6B87598
#define SNASSEMBLER_H_E6B87598

#include "fhiclcpp/ParameterSet.h"
#include "SnClient.h"
// #include "dispatcher/KvpSet.h"

class SnAssembler
{
public:
  SnAssembler(fhicl::ParameterSet const &config);
  
  void connect();
  std::shared_ptr<gov::fnal::uboone::datatypes::ub_EventRecord> assemble(sn_data_request request);

  fhicl::ParameterSet _config;
  std::vector<std::shared_ptr<SnClient> > _clients;
  
  boost::asio::io_service _io_service;
};


#endif /* end of include guard: SNASSEMBLER_H_E6B87598 */
