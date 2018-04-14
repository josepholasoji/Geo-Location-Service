#pragma once

//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "session.h"
#include <zmq.h>
#include <zmq_utils.h>

using boost::asio::ip::tcp;

class server
{
public:
	server(boost::asio::io_service& io_service, short port, gps* gps)
		: acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		socket_(io_service),
		_gps(gps)
	{
		do_accept();
	}

	~server()
	{
		//Stop zero mq
	}

private:
	void do_accept()
	{
		acceptor_.async_accept(socket_,
			[this](boost::system::error_code ec)
		{
			if (!ec)
			{
				std::make_shared<session>(std::move(socket_), this->_gps )->start();
			}

			do_accept();
		});
	}

	void* zmq_pub_obj;
	tcp::acceptor acceptor_;
	tcp::socket socket_;
	void *zmq_socket_handle = nullptr;
	gps* _gps;
};
