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
#include "../sdk/data_downstream.h"

using boost::asio::ip::tcp;

class session
	: public std::enable_shared_from_this<session>
{
public:
	session(tcp::socket socket, void* zmq_pub_obj)
		: socket_(std::move(socket)),
		zmq_pub_obj(zmq_pub_obj)
	{
		memset(this->buff, 0, max_length);
	}

	void start()
	{
		do_read();
		setup_writeprocess();
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
				int remaining_bytes_size = strlen(this->buff),
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
						//Write the processed data to the zmq publisher
						zmq_msg_t msg;
						int rc = zmq_msg_init_size(&msg, write_index + 1);
						assert(rc == 0);
						memcpy(zmq_msg_data(&msg), this->buff, write_index + 1);
						rc = zmq_sendmsg(zmq_pub_obj, &msg, 0);
						assert(rc == 0);
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

	void setup_writeprocess()
	{
		auto self(shared_from_this());
		std::thread t = std::thread([this, self]
		{
			while (true)
			{
				zmq_msg_t msg;
				int rc = zmq_msg_init(&msg);

				assert(rc == 0);
				rc = zmq_recvmsg(zmq_pub_obj, &msg, 0);

			    std::shared_ptr<data_downstream> ds();
				memcpy(&ds, zmq_msg_data(&msg), zmq_msg_size(&msg));

				assert(rc == 0);
				zmq_msg_close(&msg);

				//read from the output buffer and send to the device...
				boost::asio::async_write(socket_, boost::asio::buffer((unsigned char*)&ds, sizeof(data_downstream)),
					[this, self](boost::system::error_code ec, std::size_t /*length*/)
				{
					if (!ec)
					{

					}
				});
			}
		}, nullptr);
	}

	void* zmq_pub_obj;
	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length];
	char buff[max_length] = { 0 };
	std::shared_ptr<boost::asio::mutable_buffer> left_over_bytes;
};
