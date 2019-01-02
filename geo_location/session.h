# pragma once

#ifndef GLS_IMPORTS
	#define GLS_IMPORTS
#endif // !GLS_IMPORTS

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
//#include "zmq.h"
#include <thread>
#include "../sdk/gps.h"
#include "../sdk/data_downstream.h"
#include <boost/algorithm/string.hpp>
#include "../sdk/NanoLog.hpp"

using boost::asio::ip::tcp;

class session
	: public std::enable_shared_from_this<session>
{
public:
	session(tcp::socket socket, LPGPS _gps)
		: socket_(std::move(socket)),
		gps(_gps)
	{
		memset(this->buff, 0, max_length);
	}

	void start()
	{	
		do_read();
	}

private:
	void do_read()
	{
		auto self(shared_from_this());

		memset(data_, 0, max_length);
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
		[this, self](boost::system::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				size_t remaining_bytes_size = strlen(this->buff),
				    total_new_length = remaining_bytes_size + length;

				unsigned char* new_bytes = new unsigned char[total_new_length];
				memset(new_bytes, 0, total_new_length);				
				memcpy(new_bytes, this->buff, remaining_bytes_size);
				memcpy(new_bytes + remaining_bytes_size, this->data_, length);

				char* _temp = this->buff;
				_temp += strlen(this->buff); 
				int write_index = 0;
				for (unsigned int read_pointer = 0; read_pointer < total_new_length; read_pointer++)
				{					
					write_index = write_index < 1 ? 0 : write_index;

					_temp[write_index] = new_bytes[read_pointer];
					write_index++;

					if (new_bytes[read_pointer] == ')')
					{
						//detect hardware
						//void* device = utils.detectDevice(this->buff, max_length);

						//Process the data
						std::string s = std::string(this->buff);
						boost::trim(s);

						LOG_WARN << "Recieved data from device (" << gps->deviceName() << " ) on device " << gps->deviceId() << " ): " << s;

						std::string output = gps->process((const char*)s.c_str(), max_length);

						if (output.length() > 0) {
							//write out the output tot device
							LOG_WARN << "Writing data to device (" << gps->deviceName() << " ) on device " << gps->deviceId() << " ): " << output;
							boost::asio::async_write(socket_, boost::asio::buffer((unsigned char*)output.c_str(), output.length()),
								[this, self](boost::system::error_code ec, std::size_t /*length*/)
							{
								if (!ec) {}
							});
						}

						//After processing clear buffer
						_temp = buff;
						memset(this->buff, 0, max_length);
						write_index = 0;
					}
				}

				delete new_bytes;
				do_read();
			}
		});
	}

	LPGPS gps = nullptr;
	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length] = { 0 };
    char buff[max_length] = { 0 };
	std::shared_ptr<boost::asio::mutable_buffer> left_over_bytes;
};
