#pragma once

//The base requiement is are as follows:
//1. ZeroMQ
#include <zmq.h>
#include <zmq_utils.h>


class gps
{
public:
	//
	virtual int serverPort() = 0;

	// A method to process incoming data from the modem to start communication. 
	//This is the first methd after the server accepts the connection from the modem.
	virtual gps* detect(char*, int) = 0;

	// After detecting the gps device, this method starts a service thread to service the gps communications.
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void status() = 0;
	virtual void config() = 0;
	virtual std::tuple<unsigned char*, int> process(char*, int) = 0;
};

