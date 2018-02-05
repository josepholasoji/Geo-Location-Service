// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TK103_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TK103_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TK103_EXPORTS
#define TK103_API __declspec(dllexport)
#else
#define TK103_API __declspec(dllimport)
#endif

#include <string>
#include "../sdk/gps.h"
#include "data_structure.h"
#include <boost\asio.hpp>


//defintions
#define HEAD_LENGTH 1
#define SERIAL_NUMBER_TIME_LENGTH 12
#define COMMAND_LENGTH 4
#define MESSAGE_LENGTH 1024
#define TRAIL_LENGTH 1


// This class is exported from the tk103.dll
class TK103_API Ctk103: gps {
public:
	int istatus;
	bool started = false;
	Ctk103(void);
	// TODO: add your methods here.
	// A method to process incoming data from the modem to start communication. 
	//This is the first methd after the server accepts the connection from the modem.
	gps* detect(unsigned char*, int len);
	// After detecting the gps device, this method starts a service thread to service the gps communications.
	void start();
	void stop();
	std::string status();

	//
	int read();
	int write(data_structure*);
	int process(data_structure*);
};

extern TK103_API int ntk103;

TK103_API int fntk103(void);
