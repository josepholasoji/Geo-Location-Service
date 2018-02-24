#include <zmq.h>
#include <zmq_utils.h>
#include <boost/asio.hpp>
#include "server.h"

int main()
{
	//Start zero mq
	void *zmq_context = zmq_ctx_new();
	void *zmq_socket_handle = zmq_socket(zmq_context, ZMQ_PUB);
	zmq_bind(zmq_socket_handle, "tcp://*:5555");
	
	//Start the server...
	boost::asio::io_service io_service;
	server s(io_service, 2018);
	io_service.run();


	//Stop zero mq
	zmq_unbind(zmq_socket_handle, "tcp://*:5555");
	zmq_close(zmq_socket_handle);

	return 0;
}