# pragma once

#ifndef GLS_IMPORTS
	#define GLS_IMPORTS
#endif // !GLS_IMPORTS

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "session.h"
#include "../sdk/NanoLog.hpp"
//#include <zmq.h>
//#include <zmq_utils.h>

using boost::asio::ip::tcp;

class server
{
public:
	server(boost::asio::io_service& io_service, short port, LPGPS gps)
		: acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		socket_(io_service),
		gps(gps)
	{
		do_accept();
	}

	~server()
	{
		//Stop zero mq
		LOG_WARN << "Stopping device services for " << gps->deviceName() << " @ " << gps->serverPort();
	}

private:
	void do_accept()
	{
		acceptor_.async_accept(socket_,
			[this](boost::system::error_code ec)
		{
			if (!ec)
			{
				std::make_shared<session>(std::move(socket_), this->gps )->start();
			}

			do_accept();
		});
	}

	void* zmq_pub_obj;
	tcp::acceptor acceptor_;
	tcp::socket socket_;
	void *zmq_socket_handle = nullptr;
	LPGPS gps;
};
