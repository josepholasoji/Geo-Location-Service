#ifndef TK103_EXPORTS
	#define TK103_EXPORTS
#endif // !TK103_EXPORTS


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

//FROM SERVER TO DEVICE
//FROM SERVER TO DEVICE
#define WRITE_HANDSHAKE_SIGNAL_RESPONSE (id) "(" + id + "AP01HSO)"
#define WRITE_HANDSHAKE_DEVICE_LOGIN_RESPONSE (id) "(" + id + "AP05)"
#define WRITE_ISOCHRONIC_CONTINUES_FEEDBACK_CONFIGURATION (id) "(" + id + "AR0000140024)"
#define WRITE_ANSWER_ALARM_MESSAGE (id) "(" + id + "AS012)"
#define WRITE_ONE_TIME_ENQUIRY_MESSAGE (id) "(" + id + "AP00)"
#define WRITE_SETTING_VEHICLE_HIGH_AND_LOW_LIMIT_SPEED (id) "(" + id + "AP12H050L030)"
#define WRITE_OIL_CONTROL_SINGLE (id) "(" + id + "AV010)"
#define WRITE_CIRCUIT_CONTROL_SIGNAL (id) "(" + id + "AV000)"
#define WRITE_CONTROL_THE_RESTARTED_MESSAGE_OF_THE_DEVICE (id) "(" + id + "AT000)"
#define WRITE_SET_ACC_OPEN_SENDING_DATA_INTERVALS (id) "(" + id + "AR050014)"
#define WRITE_SET_ACC_CLOSE_SENDING_DATA_INTERVALS (id) "(" + id + "AR060014)"
#define WRITE_SETTING_GEO_FENCE_MESSAGE(id) "(" + id + "AX051, N,2245.318,2246.452,E,11233.232,11355.175)"

//FROM DEVICE TO SERVER
#define READ_HANDSHAKE_SIGNAL_MESSAGE () "(%12s%4s000013612345678HSO)"
#define READ_CONTINUOUS_ANSWER_SETTING_ISOCHRONOUS_FEEDBACK_MESSAGE () "(%12s%4s00050014)"
#define READ_LOGIN_MESSAGE () "(%12s%4s000013632782450080524A2232.9806N11404.9355E000.1101241323.8700000000L000450AC)"
#define READ_ALARM_MESSAGE () "(%12s%4s9061830A2934.0133 N10627.2544E040.0080331309.6200000000L000770AD)"
#define READ_ANSWER_CALLING_MESSAGE () "(%12s%4s080525A2934.0133N 10627.2544E000.0141830309.6200000000L00000023)"
#define READ_ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE () "(%12s%4s080612A2232.9828N11404.9297E000.0022828000.0000000000L000230AA)"
#define READ_CONTINUES_FEEDBACK_ENDING_MESSAGE () "(%4s[GPSDATA])"
#define READ_ANSWER_THE_RESTARTED_MESSAGE_OF_THE_DEVICE () "(%4s)"
#define READ_SETUP_THE_SPEED_OF_THE_CAR () "(%12s%4sH0501L030)"
#define READ_ANSWER_THE_SETTING_ACC_OPEN_SENDING_DATA_INTERVALS () "(%4s)"
#define READ_CONTROL_CIRCUIT () "(%4s%1i)"
#define READ_CONTROL_OIL () "(%4s%1i)"
#define READ_ANSWER_THE_SETTING_ACC_CLOSE_SENDING_DATA_INTERVALS () "(%4s)"
#define READ_ANSWER_THE_SETTING_GEOFENCE_MESSAGE () "(%4s[0/1])"
#define READ_ANSWER_THE_SETTING_ACC_CLOSE_SENDING_DATA_INTERVALS () "(%4s)"




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
