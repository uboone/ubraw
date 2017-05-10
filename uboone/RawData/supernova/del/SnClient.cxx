

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <boost/program_options.hpp>

#include "sn-data-util.h"
#include "datatypes/ub_EventRecord.h"
#include "datatypes/raw_data_access.h"

#include "SnClient.h"

using boost::asio::ip::tcp;
using gov::fnal::uboone::datatypes::ub_EventRecord;

SnClient::SnClient(boost::asio::io_service& io_service, int crate, const std::string& host, const std::string& port)
  : _crate(crate)
  , _host(host)
  , _port(port)
  , _socket(io_service)
{
  
}
  
SnClient::~SnClient()
{}

void SnClient::connect()
{
  tcp::resolver resolver(_socket.get_io_service());
  tcp::resolver::query query(_host, _port);
  tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
  tcp::resolver::iterator end;

  boost::system::error_code error = boost::asio::error::host_not_found;
  while (error && endpoint_iterator != end)
  {
    _socket.close();
    _socket.connect(*endpoint_iterator++, error);
  }
  if (error)
    throw boost::system::system_error(error);
}


void SnClient::start_request(sn_data_request req, std::shared_ptr<ub_EventRecord> event)
{
  _event = event;
  _frag.clear();
  _size = 0;
  _request = req;
  send_obj(_socket,_request);

  boost::asio::async_read(_socket, 
                          boost::asio::buffer(&_size,sizeof(size_t)), 
                          boost::bind(&SnClient::callback_1, this, _1, _2) 
  );
}

void SnClient::callback_1(const boost::system::error_code& error, size_t nbytes)
{
  if(nbytes != sizeof(size_t)) { throw std::runtime_error("Didn't get object size."); }
  
  _frag.resize(_size/sizeof(gov::fnal::uboone::datatypes::raw_fragment_t::value_type));  
  boost::asio::async_read(_socket, 
                          boost::asio::buffer(_frag.begin(),_size),
                          boost::bind(&SnClient::callback_2, this, _1, _2) 
                          );
}

void SnClient::callback_2(const boost::system::error_code& error, size_t nbytes)
{
  if(nbytes != _size) { throw std::runtime_error("Didn't get complete fragment.");};

  std::cout  << "Recieved a fragment with " << _frag.size() << " words." << std::endl;
  if(_frag.size())
    std::cout <<  quick_cast<gov::fnal::uboone::datatypes::ub_TPC_CardHeader_v6>(_frag.begin()+2).debugInfo() << std::endl;
  
  insert_into_record();
}

void SnClient::insert_into_record()
{
  if(_frag.size() < 10) throw std::runtime_error("No result returned for crate " + std::to_string(_crate));

  artdaq_fragment_header afh;
  crate_header_t crate_header;
  afh.word_count = _frag.size() + artdaq_fragment_header::num_words()+  sizeof(crate_header_t)/sizeof(raw_data_type);
  afh.metadata_word_count = sizeof(crate_header_t)/sizeof(raw_data_type);

  crate_header.data_transmission_header.raw_fragment_wordcount =  _frag.size();
  if(_crate == 10)   crate_header.crate_type = SystemDesignator::PMT_SYSTEM;
  else               crate_header.crate_type = SystemDesignator::TPC_SN_SYSTEM;
  crate_header.crate_number = _crate;
  crate_header.event_number = _request.frame;
  crate_header.frame_number = _request.frame;
  crate_header.local_host_time. seb_time_sec = 1471893000+_request.frame/625;

  // insert the crate header, then the artdaq header before that
  _frag.insert(_frag.begin(),(uint16_t*)(&crate_header),(uint16_t*)(&crate_header)+sizeof(crate_header_t)/sizeof(raw_data_type));
  _frag.insert(_frag.begin(),(uint16_t*)(&afh)         ,(uint16_t*)(&afh)+artdaq_fragment_header::num_words());



  _event->addFragment_SN(_frag);
  _event->getGlobalHeader().setLocalHostTime(ub_LocalHostTime(1471893000+_request.frame/625,_request.frame*1000000/625));
  _event->getGlobalHeader().setRunNumber(_request.run);
  _event->getGlobalHeader().setSubrunNumber(_request.subrun);
  _event->getGlobalHeader().setEventNumber(_request.frame);
}

