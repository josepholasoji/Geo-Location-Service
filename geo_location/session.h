#pragma once
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <zmq.h>
#include <zmq_utils.h>
#include <thread>
#include "../sdk/gps.h"
#include "Utils.h"
#include "../sdk/data_downstream.h"
#include <boost/algorithm/string.hpp>

using boost::asio::ip::tcp;

Utils utils;
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
				for (int write_index = 0, read_pointer = 0; read_pointer < total_new_length; read_pointer++, write_index++)
				{					
					_temp[write_index] = new_bytes[read_pointer];
					if (new_bytes[read_pointer] == ')')
					{
						//detect hardware
						//void* device = utils.detectDevice(this->buff, max_length);

						//Process the data
						std::string s = std::string(this->buff);
						boost::trim(s);
						auto output = gps->process((char*)s.c_str(), max_length);

						//write out the output tot device
						boost::asio::async_write(socket_, boost::asio::buffer((unsigned char*)output.c_str(), output.length()),
							[this, self](boost::system::error_code ec, std::size_t /*length*/)
						{
							if (!ec){}
						});


						//After processing clear buffer
						memset(this->buff, 0, max_length);

						_temp = buff;
						write_index = -1;
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
	char data_[max_length];
    char buff[max_length] = { 0 };
	std::shared_ptr<boost::asio::mutable_buffer> left_over_bytes;
};
