
#ifndef SNCLIENT_H_02A93454
#define SNCLIENT_H_02A93454


#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "sn-data-util.h"
#include "datatypes/ub_EventRecord.h"
#include "datatypes/raw_data_access.h"


class SnClient {
public:
  SnClient(boost::asio::io_service& io_service, int crate, const std::string& host, const std::string& port);
  ~SnClient();

  void connect();
  void start_request(sn_data_request request, std::shared_ptr<gov::fnal::uboone::datatypes::ub_EventRecord> event);  

  void callback_1(const boost::system::error_code& error, size_t nbytes);
  void callback_2(const boost::system::error_code& error, size_t nbytes);
  void insert_into_record();

  int         _crate;
  std::string _host;
  std::string _port;
  sn_data_request _request;
  
  boost::asio::ip::tcp::socket _socket;
   
  // reply
  size_t _size; // Number of bytes in reply
  gov::fnal::uboone::datatypes::raw_fragment_t _frag;     
  
  // result
  std::shared_ptr<ub_EventRecord> _event;
};

#endif /* end of include guard: SNCLIENT_H_02A93454 */
