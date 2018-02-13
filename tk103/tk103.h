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
#include <boost/asio.hpp>


//defintions
#define HEAD_LENGTH 1
#define SERIAL_NUMBER_TIME_LENGTH 12
#define COMMAND_LENGTH 4
#define MESSAGE_LENGTH 1024
#define TRAIL_LENGTH 1

struct _command_message
{
	const char *message_type_subtype_number;
	const char *message_description;
	const char *remark;
	enum _command_message_enum type;
};

enum _command_message_enum
{
	AP00,
	AP01,
	AP03,
	AP04,
	AP05,
	AP07,
	AP11,
	AP12,
	AP15,
	AP17,


	AQ00,
	AQ01,
	AQ02,
	AQ03,
	AQ04,


	AR00,
	AR01,
	AR05,
	AR06,


	AS01,
	AS07,


	AT00,



	AV00,
	AV01,
	AV02,
	AV03,


	AX00,
	AX01,
	AX02,
	AX03,
	AX04,
	AX05,
	
	BO01,
	BP00,
	BP02,
	BP03,
	BP04,
	BP05,
	BP12,
	BP07,


	BR00,
	BR01,
	BR02,
	BR05,
	BR06,


	BS04,
	BS05,
	BS06,
	BS08,
	BS09,
	BS20,
	BS21,
	BS23,


	BT00,
	BU00,
	BV00,
	BV01,
	BV02
};

// This class is exported from the tk103.dll
class TK103_API Ctk103: gps {

	std::map<std::string,struct _command_message > device_command_message;

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
	void status();
	void config();

	//
	int read(unsigned char* ch);
	int write(unsigned char* ch);
	int process(unsigned char* ch, int ch_len, struct data_upstream *du);
};

extern TK103_API int ntk103;

TK103_API int fntk103(void);
