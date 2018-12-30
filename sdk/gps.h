#pragma once

class gps
{
public:
	//
	virtual short int serverPort() = 0;
	virtual const char* deviceName() = 0;
	virtual const char* deviceId() = 0;

	// A method to process incoming data from the modem to start communication. 
	//This is the first methd after the server accepts the connection from the modem.
	virtual gps* detect(char*, int) = 0;

	// After detecting the gps device, this method starts a service thread to service the gps communications.
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void status() = 0;
	virtual void config() = 0;
	virtual const char* process(const char*, int) = 0;
};

typedef class gps GPS, *LPGPS;
