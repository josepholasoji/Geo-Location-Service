// tk103.cpp : Defines the exported functions for the DLL application.
//
#if defined(_MSC_VER)
	#include "../sdk/stdafx.h"
#endif

#include "tk102B.h"
#include "../sdk/data_payload_from_device.h"
#include "../sdk/data_downstream.h"
#include <thread>
#include <memory>
#include <boost/chrono.hpp>
#include "../sdk/Utils.h"


// This is an example of an exported function.
TK102B_API gps* load(geolocation_svc::LPGPS_HANDLERS handlers)
{
	gps* _gps = new Ctk102B(handlers);
	return _gps;
}


// This is the constructor of a class that has been exported.
// see tk103.h for the class definition
Ctk102B::Ctk102B(geolocation_svc::LPGPS_HANDLERS _handlers)
{
	this->handlers = _handlers;
	this->port = 2772;

	device_command_message = {
		{ "AP00" ,{ "AP00","One time calling message 3.1.5","Device parameter message",_command_message_enum::AP00 } },
		{ "AP01" ,{ "AP01","Response handshake signal message 3.1.1","Device parameter message",_command_message_enum::AP01 } },
		{ "AP03" ,{ "AP03","Read device parameter configuring message","Device parameter message",_command_message_enum::AP03 } },
		{ "AP04" ,{ "AP04","Read device operated status message","Device parameter message",_command_message_enum::AP04 } },
		{ "AP05" ,{ "AP05","Device login response message 3.1.2","Device parameter message",_command_message_enum::AP05 } },
		{ "AP07" ,{ "AP07","Center No. configuring message","Device parameter message",_command_message_enum::AP07 } },
		{ "AP11" ,{ "AP11","Cell phone NO. configuring message","Device parameter message",_command_message_enum::AP11 } },
		{ "AP12" ,{ "AP12","Setting vehicle high and low limit speed 3.1.8","Device parameter message",_command_message_enum::AP12 } },
		{ "AP15" ,{ "AP15","Monitor Command","Device parameter message",_command_message_enum::AP15 } },
		{ "AP17" ,{ "AP17","Read device cell phone configuring","Device parameter message",_command_message_enum::AP17 } },
		{ "AQ00" ,{ "AQ00","Common Message","General communication message",_command_message_enum::AQ00 } },
		{ "AQ01" ,{ "AQ01","Attempt Message","General communication message",_command_message_enum::AQ01 } },
		{ "AQ02" ,{ "AQ02","Answer of calling message(Taxi)","General communication message",_command_message_enum::AQ02 } },
		{ "AQ03" ,{ "AQ03","Calling Message(Taxi)","General communication message",_command_message_enum::AQ03 } },
		{ "AQ04" ,{ "AQ04","Navigation Message","General communication message",_command_message_enum::AQ04 } },
		{ "AR00" ,{ "AR00","Isochronous	for	continues	feedback configuring 3.1.3","Vehicle positioning Answer message",_command_message_enum::AR00 } },
		{ "AR01" ,{ "AR01","Isometry for continues feedback configuring","Vehicle positioning Answer message",_command_message_enum::AR01 } },
		{ "AR05" ,{ "AR05","Set	ACC	open	sending	data	transmiting intervals 3.1.12","Vehicle positioning Answer message",_command_message_enum::AR05 } },
		{ "AR06" ,{ "AR06","Set	ACC	open	sending	data	transmiting intervals 3.1.13","Vehicle positioning Answer message",_command_message_enum::AR06 } },
		{ "AS01" ,{ "AS01","Answer Alarm Message 3.1.4","Answer signal",_command_message_enum::AS01 } },
		{ "AS07" ,{ "AS07","Answer	Message	for	getting	customer successfully (Taxi)","Answer signal",_command_message_enum::AS07 } },
		{ "AT00" ,{ "AT00","Control the restarted message of the device 3.1.11","Control signal",_command_message_enum::AT00 } },
		{ "AV00" ,{ "AV00","Circuit control signal 3.1.9","Expanding message",_command_message_enum::AV00 } },
		{ "AV01" ,{ "AV01","Oil control signal 3.1.10","Expanding message",_command_message_enum::AV01 } },
		{ "AV02" ,{ "AV02","One key configuring command","Expanding message",_command_message_enum::AV02 } },
		{ "AV03" ,{ "AV03","Read one key configuring","Expanding message",_command_message_enum::AV03 } },
		{ "AX00" ,{ "AX00","Answer currency up explaining result message","Expanding message",_command_message_enum::AX00 } },
		{ "AX01" ,{ "AX01","Alarm configuring message","Expanding message",_command_message_enum::AX01 } },
		{ "AX02" ,{ "AX02","Device Function configuring command","Expanding message",_command_message_enum::AX02 } },
		{ "AX03" ,{ "AX03","Device mode configured command","Expanding message",_command_message_enum::AX03 } },
		{ "AX04" ,{ "AX04","Intialized device command","Expanding message",_command_message_enum::AX04 } },
		{ "AX05" ,{ "AX05","Setting Geo-fence Message 3.1.14","Expanding message",_command_message_enum::AX05 } },
		{ "BO01" ,{ "BO01","Alarm message 3.2.4","Alarm message",_command_message_enum::BO01 } },
		{ "BP00" ,{ "BP00","Handshake signal message 3.2.1","Device status message",_command_message_enum::BP00 } },
		{ "BP02" ,{ "BP02","Answer	device	parameter	configured message","Device status message",_command_message_enum::BP02 } },
		{ "BP03" ,{ "BP03","Answer device operated status message","Device status message",_command_message_enum::BP03 } },
		{ "BP04" ,{ "BP04","Answer calling message 3.2.5","Device status message",_command_message_enum::BP04 } },
		{ "BP05" ,{ "BP05","Device	login 	message 3.2.2","Device status message",_command_message_enum::BP05 } },
		{ "BP12" ,{ "BP12","Answer vehicle high and low speed limit 3.2.8","Device status message",_command_message_enum::BP12 } },
		{ "BP07" ,{ "BP07","Message for getting customer successfully","Device status message",_command_message_enum::BP07 } },
		{ "BR00" ,{ "BR00","Isochronous feedback message 3.2.6","Vehicle Positioning message",_command_message_enum::BR00 } },
		{ "BR01" ,{ "BR01","Isometry continuous feedback message","Vehicle Positioning message",_command_message_enum::BR01 } },
		{ "BR02" ,{ "BR02","Continues feedback ending messsage3.2.7","Vehicle Positioning message",_command_message_enum::BR02 } },
		{ "BR05" ,{ "BR05","Answer the Setting ACC open sending data transmitting intervals 3.2.12","Vehicle Positioning message",_command_message_enum::BR05 } },
		{ "BR06" ,{ "BR06","Answer the Setting	ACC open sending data transmitting intervals 3.2.13","Vehicle Positioning message",_command_message_enum::BR06 } },
		{ "BS04" ,{ "BS04","Answer attempered Message","Answer message",_command_message_enum::BS04 } },
		{ "BS05" ,{ "BS05","Answer reading called configuring number","Answer message",_command_message_enum::BS05 } },
		{ "BS06" ,{ "BS06","Answer caller configuring number","Answer message",_command_message_enum::BS06 } },
		{ "BS08" ,{ "BS08","Answer	setting	isochronous	feedback message 3.2.3","Answer message",_command_message_enum::BS08 } },
		{ "BS09" ,{ "BS09","Answer setting Isometry feedback message","Answer message",_command_message_enum::BS09 } },
		{ "BS20" ,{ "BS20","Answer response calling message (Taxi)","Answer message",_command_message_enum::BS20 } },
		{ "BS21" ,{ "BS21","Answer calling message(Taxi)","Answer message",_command_message_enum::BS21 } },
		{ "BS23" ,{ "BS23","Answer navigation message","Answer message",_command_message_enum::BS23 } },
		{ "BT00" ,{ "BT00","Answer the restarted message of the device 3.2.11","",_command_message_enum::BT00 } },
		{ "BU00" ,{ "BU00","Answer  the	Setting Geo-fence Message 3.2.14","",_command_message_enum::BU00 } },
		{ "BV00" ,{ "BV00","Answer circuit control 3.2.9","Answer control sign",_command_message_enum::BV00 } },
		{ "BV01" ,{ "BV01","Answer oil control 3.2.10","Answer control sign",_command_message_enum::BV01 } },
		{ "BV02" ,{ "BV02","Answer enquiring of one key setting","Answer control sign",_command_message_enum::BV02 } }
	};

	//Subscribe to the 
	//zmq_context = zmq_ctx_new();
	//zmq_in_socket_handle = zmq_socket(zmq_context, ZMQ_SUB);
	//zmq_bind(zmq_in_socket_handle, "tcp://*:5555");

	_gps_service.set_handlers(this->handlers);
	return;
}

const char* Ctk102B::process(const char *data, int size)
{
	data_payload_from_device* deviceData = nullptr;

	auto parsedData = this->parseDeviceRequest(data);
	deviceData = std::get<0>(parsedData);
	switch (std::get<1>(parsedData).type)
	{
		case _command_message_enum::AP00:
			break;
		case _command_message_enum::AP01:
			break;
		case _command_message_enum::AP03:
			break;
		case _command_message_enum::AP04:
			break;
		case _command_message_enum::AP05:
			break;
		case _command_message_enum::AP07:
			break;
		case _command_message_enum::AP11:
			break;
		case _command_message_enum::AP12:
			break;
		case _command_message_enum::AP15:
			break;
		case _command_message_enum::AP17:
			break;
		case _command_message_enum::AQ00:
			break;
		case _command_message_enum::AQ01:
			break;
		case _command_message_enum::AQ02:
			break;
		case _command_message_enum::AQ03:
			break;
		case _command_message_enum::AQ04:
			break;
		case _command_message_enum::AR00:
			break;
		case _command_message_enum::AR01:
			break;
		case _command_message_enum::AR05:
			break;
		case _command_message_enum::AR06:
			break;
		case _command_message_enum::AS01:
			break;
		case _command_message_enum::AS07:
			break;
		case _command_message_enum::AT00:
			break;
		case _command_message_enum::AV00:
			break;
		case _command_message_enum::AV01:
			break;
		case _command_message_enum::AV02:
			break;
		case _command_message_enum::AV03:
			break;
		case _command_message_enum::AX00:
			break;
		case _command_message_enum::AX01:
			break;
		case _command_message_enum::AX02:
			break;
		case _command_message_enum::AX03:
			break;
		case _command_message_enum::AX04:
			break;
		case _command_message_enum::AX05:
			break;
		case _command_message_enum::BO01:
			break;
		case _command_message_enum::BP00: //Device handshake...
		{
			return _gps_service.deviceHandshake(deviceData);
		}
		break;
		case _command_message_enum::BP02:
			break;
		case _command_message_enum::BP03:
			break;
		case _command_message_enum::BP04:
			break;
		case _command_message_enum::BP05: //Device login message
		{
			return _gps_service.deviceLogin(deviceData);
		}
		break;
		case _command_message_enum::BP12:
			break;
		case _command_message_enum::BP07:
			break;
		case _command_message_enum::BR00://Device feedbac message
		{
			return _gps_service.deviceFeedback(deviceData);
		}
		break;
		case _command_message_enum::BR01:
			break;
		case _command_message_enum::BR02: {
			return _gps_service.deviceFeedbackEnding(deviceData);
		}
										  break;
		case _command_message_enum::BR05:
			break;
		case _command_message_enum::BR06:
			break;
		case _command_message_enum::BS04:
			break;
		case _command_message_enum::BS05:
			break;
		case _command_message_enum::BS06:
			break;
		case _command_message_enum::BS08:
			break;
		case _command_message_enum::BS09:
			break;
		case _command_message_enum::BS20:
			break;
		case _command_message_enum::BS21:
			break;
		case _command_message_enum::BS23:
			break;
		case _command_message_enum::BT00:
			break;
		case _command_message_enum::BU00:
			break;
		case _command_message_enum::BV00:
			break;
		case _command_message_enum::BV01:
			break;
		case _command_message_enum::BV02:
			break;
		default: {

		}
	}

	return "";
}

Ctk102B::~Ctk102B()
{
}

void Ctk102B::config(const char* key, const char* value)
{
	this->config(key, value);
}

gps * Ctk102B::detect(char *, int)
{
	return nullptr;
}

short int Ctk102B::serverPort()
{
	return this->port;
}

const char* Ctk102B::deviceName()
{
	return "TK102B";
}

const char* Ctk102B::deviceId()
{
	return this->_gps_service.deviceId.c_str();
}

TK102B_API std::map<std::string, struct _command_message > Ctk102B::deviceCommandMessage() {
	return this->device_command_message;
}

void Ctk102B::start()
{
	started = true;

	std::thread t([this]
	{
		while (started)
		{

		}
	});
}


void Ctk102B::stop()
{
	started = false;
}

void Ctk102B::status()
{

}

unsigned char* Ctk102B::read()
{
	return 0;
}

int Ctk102B::write(unsigned char* ch, int size)
{
	return 0;
}

TK102B_API  std::tuple<data_payload_from_device*, struct _command_message> Ctk102B::parseDeviceRequest(const char* ch)
{
	data_payload_from_device* in_data = (data_payload_from_device *)(ch);
	struct _command_message msg = this->device_command_message[std::string(in_data->_LOGIN_MESSAGE.command, sizeof(in_data->_LOGIN_MESSAGE.command))];

	return std::tuple<data_payload_from_device*, struct _command_message>(in_data, msg);
}

