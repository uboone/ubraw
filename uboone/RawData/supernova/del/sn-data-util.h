#ifndef SN_DATA_UTIL_H_A1FB1BE2
#define SN_DATA_UTIL_H_A1FB1BE2

#include <boost/array.hpp>
#include <boost/asio.hpp>

struct sn_data_request
{
  int32_t run;
  int32_t subrun;
  int64_t frame;
  // -1 frame means send me the first frame in the file
  sn_data_request():run(-1), subrun(-1), frame(-1) {};
};


template<typename T>
void send_obj( boost::asio::ip::tcp::socket& socket, T& obj )
{
  boost::system::error_code ignored_error;
  size_t size = sizeof(obj);
  boost::asio::write(socket, boost::asio::buffer(&size,sizeof(size_t)),
          boost::asio::transfer_all(), ignored_error);

  boost::asio::write(socket, boost::asio::buffer(&obj,size),
          boost::asio::transfer_all(), ignored_error);
  
}

template<typename T>
void recieve_obj( boost::asio::ip::tcp::socket& socket, T& obj )
{
  size_t size = 0;;
  boost::asio::read(socket, boost::asio::buffer(&size,sizeof(size_t)));
  assert(size == sizeof(obj));
  boost::asio::read(socket, boost::asio::buffer(&obj,size));
   
}

typedef std::vector<unsigned char> myBuffer_t;

template<typename T>
void send_stuff( boost::asio::ip::tcp::socket& socket, T& buffer )
{
  boost::system::error_code ignored_error;
  size_t size = buffer.size()*sizeof(typename T::value_type);
  boost::asio::write(socket, boost::asio::buffer(&size,sizeof(size_t)),
          boost::asio::transfer_all(), ignored_error);

  boost::asio::write(socket, boost::asio::buffer(buffer.begin(),size),
          boost::asio::transfer_all(), ignored_error);
}


template<typename T>
void recieve_stuff( boost::asio::ip::tcp::socket& socket, T& buffer )
{
  size_t size = 0;;
  boost::asio::read(socket, boost::asio::buffer(&size,sizeof(size_t)));
  buffer.resize(size/sizeof(typename T::value_type));
  boost::asio::read(socket, boost::asio::buffer(buffer.begin(),size));
  
}

#endif /* end of include guard: SN_DATA_UTIL_H_A1FB1BE2 */

