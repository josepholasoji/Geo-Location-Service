#pragma once
class gps
{
public:
	// A method to process incoming data from the modem to start communication. 
	//This is the first methd after the server accepts the connection from the modem.
	virtual gps* detect(unsigned char*, int) = 0;
	// After detecting the gps device, this method starts a service thread to service the gps communications.
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual std::string status() = 0;
};

