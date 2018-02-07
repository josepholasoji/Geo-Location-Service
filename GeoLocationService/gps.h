#pragma once
class gps
{
public:
	gps()=0;
	// A method to process incoming data from the modem to start communication. 
	//This is the first methd after the server accepts the connection from the modem.
	virtual gps* detect() = 0;
	// After detecting the gps device, this method starts a service thread to service the gps communications.
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void status() = 0;
	virtual void config() = 0;
};

