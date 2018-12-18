#pragma once
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
	#if defined(_MSC_VER)
		#define TK103_API __declspec(dllexport)
	#else
		#define TK103_API 
	#endif // defined(WINDOW) && (_MSC_VER)
#else
	#if defined(_MSC_VER)
		#define TK103_API __declspec(dllimport)
	#else
		#define TK103_API 
	#endif // defined(WINDOW) && (_MSC_VER)
#endif

#include <string>
#include "../sdk/gps.h"
#include "../sdk/data_payload_from_device.h"
#include "../sdk/dms.h"
#include "../sdk/Utils.h"
#include "data_structure.h"
#include <boost/asio.hpp>
#include <tuple>
#include "gps_service.h"
#include "../sdk/sdk.h"


//defintions
#define HEAD_LENGTH 1
#define SERIAL_NUMBER_TIME_LENGTH 12
#define COMMAND_LENGTH 4
#define MESSAGE_LENGTH 1024
#define TRAIL_LENGTH 1

//FROM SERVER TO DEVICE
#define WRITE_HANDSHAKE_SIGNAL_RESPONSE (id) "(" + id + "AP01HSO)"
#define WRITE_HANDSHAKE_DEVICE_LOGIN_RESPONSE (id) "(" + id + "AP05)"

// XXXX：Interval for each message of continues feedback. hex。Unit：
// Second, 4 characters in all, H_STRING. The max is 0xFFFF
// seconds 。 When XXXX＝0,the device stops continues feedback. YYZZ：The total time for feedback, 16 advance system. Unit：YY：
// Hour 、 ZZ：Minute. 4 characters in all，H_STRING，The max is 0xFFFF，ie:255 hours 255 minutes. When YYZZ＝0, according to
// the time intervals, continues feedback.
// When both XXXX and YYZZ are not 0，it figure that feedback according to the time intervals, when it up to the total time, it
// automaticly stop to feedback

// This message is available to ecolomic device and navigation device. In the mode of SMS to continues feedback, if set time interval is less than the Min time interval (Set by the device manufacturer),it will continues feedback according to the Min time interval, otherwise continues feedback according to the set time. The data mode is the
// same as the SMS mode.
#define WRITE_ISOCHRONIC_CONTINUES_FEEDBACK_CONFIGURATION (id, xxxx, yy, zz) "(" + id + "AR00"+ xxxx + yy + zz + ")"

// X：The type of alarm for BO01X up alarm message.1character, 16 advance system, ASCII character
// 0：Cut off vehicle oil	
// 1： Happen accident	
// 2：Vehicle rob (SOS help)
// 3：Vehicle anti-theft alarm	
// 4：Vehicle low speed alarm 
// 5：Vehicle over speed alarm		
// 6. Alarm out of Geo-fence
#define WRITE_ANSWER_ALARM_MESSAGE (id, x) "(" + id + "AS01" + x + ")"
#define WRITE_ONE_TIME_ENQUIRY_MESSAGE (id) "(" + id + "AP00)"

//figures the sending message time is 2008 - 8 - 30 - 14 :18 : 30.Setting the up limit speed is50km / h, low limit is 30km / h.When up limit is 000, 
//it figures cancel alarm up limit, and When down limit is 000, it figures cancel alarm down limit.Less 3
//digits of the speed, full 0 on left.Alarm refer to 3.2.4
#define WRITE_SETTING_VEHICLE_HIGH_AND_LOW_LIMIT_SPEED (id, HHH, LLL) "(" + id + "AP12" + "H" + HHH + "L" + LLL + ")"

//“1”or“0”,“1”figures opening oil，“0”figures closing oil。
#define WRITE_OIL_CONTROL_SINGLE (id, x) "(" + id + "AV01" + x + ")"

//“1”or“0”, “1”figures opening circuit, “0”figures closing circuit.
#define WRITE_CIRCUIT_CONTROL_SIGNAL (id, x) "(" + id + "AV00" + x + ")"

//Meaning the device restart.Response: BT00
#define WRITE_CONTROL_THE_RESTARTED_MESSAGE_OF_THE_DEVICE (id) "(" + id + "AT00)"

//XXXX：The time for sending data intervals for the ACC Open, hex.
//Unit：Second
//Meaning it sends back intervals, //when the ACC is opening.
//Response: BR05
#define WRITE_SET_ACC_OPEN_SENDING_DATA_INTERVALS (id, xxxx) "(" + id + "AR05" + xxxx + ")"

//XXXX：The time for sending data intervals for the ACC Open, hex.
//Unit：Second
//Meaning it sends back intervals, //when the ACC is closing.
//Response: BR06
#define WRITE_SET_ACC_CLOSE_SENDING_DATA_INTERVALS (id, xxxx) "(" + id + "AR06" + xxxx + ")"

//Format: AX05	N, D, Minlatitude, Maxlatitude, G, Minlongitude, Maxlongitude
//AX05：Fixed Keywords
//N ： ”0” or “1” ， ”0”, figures cancel Geo - fence, “1”figures sets Geo - fence.
//If for cancelling the Geo - fence, the back data cannot be sent out.
//D：Standard for latitude, N, north latitude; S: south latitude.
//Minlatitude : lower limit for latitude, Format : DDFF.FFF, DD ： latitude’s degree(00 ~90), FF.FFF：latitude’s cent(00.0000 ~59.999) ， reserve three digit decimal fraction.
//Maxlatitude： upper limit for latitude, Format : DDFF.FFF, DD ： latitude’s degree(00 ~90), FF.FFF：latitude’s cent(00.0000 ~59.999) ， reserve three digit decimal fraction.
//G：Standard for longitude, E, east longitude; S: south longitude.
//W : west longitude
//Minlogintude : lower limit for longitude, Format : DDDFF.FFF，DDD： Longitude’s degree(000 ~180), FF.FFF：longitude’s cent(00.0000 ~59.999), reserve three digit decimal
//fraction.
//Minlongitude：upper limit for longitude, Format : DDDFF.FFF，DDD： Longitude’s degree(000 ~180), FF.FFF：longitude’s cent(00.0000 ~59.999), reserve three digit decimal
//fraction.
//Response:	BU00
//Example: 
//(080830141830AX051, N, 2245.318, 2246.452, E, 11233.232, 11355.175)
//Figures the sending message time is 2008 - 8 - 30 - 14 :18 : 30.Set Geo - fence., lower limit for latitude is 22	degree	45．318 cent，upper limit for latitude is 22 degree
//46.452 cent; lower limit for longitude is 112	degree	33.232 cent，upper limit for
//longitude is 113 degree 55.175 cent．

#define WRITE_SETTING_GEO_FENCE_MESSAGE(id, N, latitude_N_or_S, Minlatitude, Maxlatitude, longitude_E_or_S_or_W, Minlogintude, Maxlongitude)  "(" + id + "AX05" + N + "," + latitude_N_or_S + "," + Minlatitude + "," + Maxlatitude + "," + longitude_E_or_S_or_W + "," + Minlogintude + "," + Maxlongitude + ")"

//FROM DEVICE TO SERVER

//Centre service response AP01
#define READ_HANDSHAKE_SIGNAL_MESSAGE (source, id, command, device_id, body) sprintf(source, "(%12s%4s%15s%3s)", id, command, device_id, body)

//Format: BS08XXXXYYZZ
//BS08：Fix key words
//XXXX：interval of time every each return news。Unit：second， total of 4 bytes，H_STRING，up to 65535 seconds 。 XXXX＝0, stop to return message。
//YYZZ：total return time，Unit：YY：Hour、ZZ：Minute。Total of 4 bytes，hexadecimal，up to FFFF，means 255 hours and 255
//minutes。When YYZZ＝0, then ceaselessly return according to the interval of time。
//When XXXX and YYZZ unequal to，then means ceaselessly
//return by time interval，stop return until reach the total time。
//No need to response
#define READ_CONTINUOUS_ANSWER_SETTING_ISOCHRONOUS_FEEDBACK_MESSAGE (source, id, command, device_id, xxxxx, yy, zz) sprintf(source, "(%12s%4s%15s%4s%2s%2s)", id, command, device_id, xxxxx, yy, zz)

//Centre service response AP05
#define READ_LOGIN_MESSAGE (source, id, command, device_id, body) sprintf(source, "(%12s%4s%15s%60s)", id, command, device_id, body)

//BO01X + GPS data
//BO01：Fixed keywords
//X：Specific alarm information code，1 byte，Hexadecimal。
//Alarm information：
//0：Vehicle power off	
//1：Accident	
//2：Vehicle robbery（SOS help）
//3：Vehicle anti - theft and alarming	
//4：Lowerspeed Alert
//5：Overspeed Alert	6:Alarm when out of Geo - fence
//Centre response: AS01
#define READ_ALARM_MESSAGE (source, id, command, device_id, body) sprintf(source, "(%12s%4s%15s%61s)", id, command, device_id, body)

//
//BP04＋GPS data
//BP04：fix Command Word。
//Eg.:（080525141830BP04080525A2934.0133N 10627.2544E000.0141830309.6200000000L00000023）
//Showing the time for send message at 22:18 : 30, on May 25.Upterminal news
//（center response by one roll call），
//GPS data acquisition time is May25, 2008， 
//Universal time is 14 : 18 : 30，”A” shows the data available，
//29 degrees, 34.0133 minutes north latitude，106 degrees 27.2544 minutes east longitude，
//speed is 0km / h，the angle is 309.62 degrees, from due north.。
//No response
#define READ_ANSWER_CALLING_MESSAGE (source, id, command, body) sprintf(source, "(%12s%4s%61s)", id, command, body)

//Response	No
#define READ_ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE (source, id, command, device_id, body) sprintf(source, "(%12s%4s%15%60s)", id, command, device_id, body)

//Response	No
#define READ_CONTINUES_FEEDBACK_ENDING_MESSAGE (source, id, command,  body) sprintf(source, "(%12s%4s%60s)", id, command, body)

//
#define READ_ANSWER_THE_RESTARTED_MESSAGE_OF_THE_DEVICE (source, id, command,  body) sprintf(source, "(%12s%4s%60s)", id, command, body)
#define READ_SETUP_THE_SPEED_OF_THE_CAR (source, id, command,  body) sprintf(source, "(%12s%4s%60s)", id, command, body)

//Response	No
#define READ_ANSWER_THE_SETTING_ACC_OPEN_SENDING_DATA_INTERVALS (source, id, command) sprintf(source, "(%12s%4s)", id, command)

//Response	No
#define READ_CONTROL_CIRCUIT (source, id, command,  body) sprintf(source, "(%12s%4s%1s)", id, command, body)

//Response	No
#define READ_CONTROL_OIL (source, id, command,  body) sprintf(source, "(%12s%4s%1s)", id, command, body)

//Response	No
#define READ_ANSWER_THE_SETTING_ACC_OPEN_SENDING_DATA_INTERVALS (source, id, command) sprintf(source, "(%12s%4s)", id, command)

//Format: BU00N
//BU00 ：Command
//N：0 or 1, “0”figures answer the cancelling Geo - fence.“1” figures
//answer setting Geo - fence.
//Response	No
#define READ_ANSWER_THE_SETTING_GEOFENCE_MESSAGE (source, id, command, body) sprintf(source, "(%12s%4s%1s)", id, command, body)

//Response	No
#define READ_ANSWER_THE_SETTING_ACC_CLOSE_SENDING_DATA_INTERVALS (source, id, command) sprintf(source, "(%12s%4s)", id, command)

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

struct _command_message
{
	const char *message_type_subtype_number;
	const char *message_description;
	const char *remark;
	enum _command_message_enum type;
};

extern "C"
{
	TK103_API  gps* load(LPGPS_HANDLERS);
}

// This class is exported from the tk103.dll
class Ctk103: public gps {
	int istatus;
	bool started = false;
	std::map<std::string, struct _command_message > device_command_message;
	std::string _deviceId;
	gps_service _gps_service;

	LPGPS_HANDLERS handlers;

	void *zmq_context;
	void *zmq_in_socket_handle, *zmq_out_socket_handle;

public:

	Ctk103(LPGPS_HANDLERS);
	~ Ctk103();

	//
	unsigned char* read();
	int write(unsigned char* ch, int size);
	TK103_API std::tuple<data_payload_from_device*, struct _command_message> parseDeviceRequest(const char* ch);
	TK103_API std::map<std::string, struct _command_message > deviceCommandMessage();

	// Inherited via gps
	virtual void start() override;
	virtual void stop() override;
	virtual void status() override;
	virtual void config() override;

	// Inherited via gps
	virtual gps * detect(char *, int) override;

	virtual const char* process(const char *data, int size);

	// Inherited via gps
	virtual short int serverPort() override;
	virtual const char* deviceId() override;
	virtual const char* deviceName() override;
};

extern TK103_API int ntk103;

