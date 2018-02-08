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
<<<<<<< HEAD:sdk/gps.h
	virtual std::string status() = 0;
=======
	virtual void status() = 0;
	virtual void config() = 0;
>>>>>>> 618c27df71bd8213b524a82d6c928dbeea1b74a5:GeoLocationService/gps.h
};

