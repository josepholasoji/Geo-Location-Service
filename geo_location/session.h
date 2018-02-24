#pragma once
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <zmq.h>
#include <zmq_utils.h>
#include "../sdk/gps.h"

using boost::asio::ip::tcp;

class session
	: public std::enable_shared_from_this<session>
{
public:
	session(tcp::socket socket, void* zmq_pub_obj)
		: socket_(std::move(socket)),
		zmq_pub_obj(zmq_pub_obj)
	{
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
				//save the remaining parameter for later...				
				unsigned char* c_buf = nullptr;
				if (this->left_over_bytes != nullptr)
				{
					 c_buf = boost::asio::buffer_cast<unsigned char*>(*this->left_over_bytes);
				}

				unsigned char* new_bytes = nullptr;
				int total_new_length = this->remaining_bytes_size + length;
				new_bytes = new unsigned char[total_new_length];
				memset(new_bytes, 0, total_new_length);
				
				if(c_buf != nullptr)
					memcpy(new_bytes, c_buf, this->remaining_bytes_size);
				
				memcpy(new_bytes + this->remaining_bytes_size, this->data_, length);

				char buff[max_length] = { 0 };
				for (this->read_pointer = 0; this->read_pointer < total_new_length; this->read_pointer++)
				{					
					buff[this->read_pointer] = new_bytes[this->read_pointer];

					if (new_bytes[this->read_pointer] == ')')
					{
						//Write the processed data to the zmq publisher
						zmq_send(zmq_pub_obj, buff, this->read_pointer + 1, 0);
						memset(buff, 0, max_length);
						this->read_pointer++;
					}
				}

				this->remaining_bytes_size = total_new_length - this->read_pointer;
				if (this->remaining_bytes_size > 0)
				{
					unsigned char* remaining_bytes = new unsigned char[this->remaining_bytes_size];
					remaining_bytes = { 0 };
					memcpy(remaining_bytes, new_bytes + this->read_pointer, this->remaining_bytes_size);
					this->left_over_bytes = std::make_shared<boost::asio::mutable_buffer>(remaining_bytes, this->remaining_bytes_size);
				}
				else this->left_over_bytes = nullptr;
			}
		});
	}

	void do_write(std::size_t length)
	{
		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				do_read();
			}
		});
	}

	void* zmq_pub_obj;
	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length];
	unsigned int remaining_bytes_size = 0, read_pointer = 0;
	std::shared_ptr<boost::asio::mutable_buffer> left_over_bytes;
};
