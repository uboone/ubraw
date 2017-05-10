#include "SnAssembler.h"


SnAssembler::SnAssembler(fhicl::ParameterSet const &config)
  : _config(config)
{  
}

void
SnAssembler::connect()
{  
  _clients.resize(11);
  for(int crate=1;crate<=10;crate++) {
    _clients[crate].reset( new SnClient(_io_service,
                                  crate,
                                  _config.get<std::string>(std::string("host_")+std::to_string(crate), "localhost"),
                                  _config.get<std::string>("port","43081")
                                  ) );
    _clients[crate]->connect(); // may throw
  }
}


std::shared_ptr<gov::fnal::uboone::datatypes::ub_EventRecord> 
SnAssembler::assemble(sn_data_request request)
{
  std::shared_ptr<gov::fnal::uboone::datatypes::ub_EventRecord> event(new gov::fnal::uboone::datatypes::ub_EventRecord);
  for(int crate=1;crate<=10;crate++) {
    _clients[crate]->start_request(request, event);
  }

  std::cout << "Running io_service" << std::endl;
  _io_service.run();
  std::cout << "Running io_service finished" << std::endl;
  return event;
}
