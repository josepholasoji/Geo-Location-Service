// tk103.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "tk103.h"
#include "..\sdk\data_payload_from_device.h"
#include "..\sdk\data_downstream.h"


// This is an example of an exported variable
TK103_API int ntk103=0;

// This is an example of an exported function.
TK103_API int fntk103(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see tk103.h for the class definition
Ctk103::Ctk103()
{
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
		{ "BP05" ,{ "BP05","Answer	device	login	response	message 3.2.2","Device status message",_command_message_enum::BP05 } },
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


    return;
}

gps * Ctk103::detect(unsigned char*, int len)
{
	//read();
	//process();
	//write();

	return nullptr;
}

void Ctk103::start()
{
	started = true;
	while (started)
	{
		unsigned char* data = read();
		auto processed_data = process(data);
		unsigned char* out_data = (unsigned char*)processed_data;
		write(out_data);
	}

	//finalize process stopping here...
}

void Ctk103::stop()
{
	started = false;
}

void Ctk103::status()
{
	switch (istatus)
	{
	//default:
		//return "Unknown status";
	}
}

void Ctk103::config()
{
}

unsigned char* Ctk103::read()
{
	return 0;
}

int Ctk103::write(unsigned char* ch)
{
	return 0;
}

data_payload_from_device * Ctk103::process(unsigned char* ch)
{
	data_payload_from_device* out_data = nullptr;
	data_payload_from_device* in_data = (data_payload_from_device*)ch;
	return out_data;
}
