#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include "gps_service.h"
#include "..\sdk\Utils.h"

using namespace std;

gps_service::gps_service()
{
	deviceId = std::string("123456");
}


gps_service::~gps_service()
{
 
}

std::string gps_service::deviceLogin(data_payload_from_device*  deviceData)
{
	std::string id(deviceData->_LOGIN_MESSAGE.id, sizeof(deviceData->_LOGIN_MESSAGE.id));
	std::string deviceId(deviceData->_LOGIN_MESSAGE.device_id, sizeof(deviceData->_LOGIN_MESSAGE.device_id));

	//Get list of configured devices...
	bool isAKnownDevice = this->handlers->is_device_registered(deviceId.c_str());
	if (isAKnownDevice)
	{
		this->deviceId = deviceId;
		std::string output = std::move(Utils::formDeviceResponse(id.c_str(), "AP05", nullptr));
		return output;
	}
	else
	{
		return "*";
	}

}

std::string gps_service::deviceFeedback(data_payload_from_device*  deviceData)
{
	std::string dataAvailable = GPScharsToString(deviceData->_ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE.gps_data.dataAvailable);
	if (dataAvailable._Equal("A")) {
		_latitude lat = deviceData->_ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE.gps_data.latitude;
		_longitude lon = deviceData->_ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE.gps_data.longitude;
		_gps_data_time time = deviceData->_ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE.gps_data.time;
		_gps_data_date date = deviceData->_ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE.gps_data.date;

		double dlat = std::atof(GPScharsToString(lat.degree).c_str()) + (std::atof(GPScharsToString(lat.minutes).c_str()) / 60);
		if (GPScharsToString(lat.direction)._Equal("S"))
			dlat = -dlat;

		double dlon = std::atof(GPScharsToString(lon.degree).c_str()) + (std::atof(GPScharsToString(lon.minutes).c_str()) / 60);
		if (GPScharsToString(lon.direction)._Equal("W"))
			dlon = -dlon;

		//We sql server timestamp format - YYYY-MM-DD HH:MI:SS
		std::string timeStamp = "20" + GPScharsToString(date.year) + "-" + GPScharsToString(date.month) + "-" + GPScharsToString(date.day) + " " + GPScharsToString(time.hh) + ":" + GPScharsToString(time.mm) + ":" + GPScharsToString(time.ss);

		//std::string datetime = Utils::makeDateTimeFromGPSData(date, time);
		std::string id = GPScharsToString(deviceData->_ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE.id);

		std::string speed = GPScharsToString(deviceData->_ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE.gps_data.speed); //The unit is km/h
		double dspeed = std::stod(speed);

		std::string orientation = GPScharsToString(deviceData->_ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE.gps_data.orientation);
		double dorientation = std::stod(orientation);
		
		std::string IOState = GPScharsToString(deviceData->_ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE.gps_data.IOState);
		bool main_power_switch_on = IOState[0] == '0' ? true : false;
		bool acc_ignition_on = IOState[1] == '1' ? true : false;

		std::string MilePost = GPScharsToString(deviceData->_ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE.gps_data.MilePost);
		std::string sMilepost = "";
		if (MilePost._Equal("L")) {
			sMilepost = "Mileage";
		}

		std::string MileData = GPScharsToString(deviceData->_ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE.gps_data.MileData);
		std::string _s("0x");
		_s.append(MileData);
		unsigned long dmile_data = 0;
		std::istringstream iss(_s);
		iss >> std::hex >> dmile_data;

		device_feedback *feedback = new device_feedback();
		feedback->acc_ignition_on = acc_ignition_on;
		strcpy_s(feedback->deviceId, this->deviceId.c_str());
		feedback->dlat = dlat;
		feedback->dlon = dlon;
		feedback->dmile_data = dmile_data;
		feedback->dorientation = dorientation;
		feedback->dspeed = dspeed;
		feedback->main_power_switch_on = main_power_switch_on;

		datetime __datetime;
		__datetime.day = std::atoi(GPScharsToString(date.day).c_str());
		__datetime.month = std::atoi(GPScharsToString(date.month).c_str());
		__datetime.year = std::atoi(GPScharsToString(date.year).c_str());

		__datetime.hour = std::atoi(GPScharsToString(time.hh).c_str());
		__datetime.minute = std::atoi(GPScharsToString(time.mm).c_str());
		__datetime.second = std::atoi(GPScharsToString(time.ss).c_str());

		feedback->_dateTime = __datetime;

		this->handlers->log_feedback(feedback);
		delete feedback;
	}

	std::string output = "";
	return output;
}

std::string gps_service::deviceFeedbackEnding(data_payload_from_device*  deviceData)
{
	std::string dataAvailable = GPScharsToString(deviceData->_CONTINUES_FEEDBACK_ENDING_MESSAGE.gps_data.dataAvailable);
	if (dataAvailable._Equal("A")) {
		_latitude lat = deviceData->_CONTINUES_FEEDBACK_ENDING_MESSAGE.gps_data.latitude;
		_longitude lon = deviceData->_CONTINUES_FEEDBACK_ENDING_MESSAGE.gps_data.longitude;
		_gps_data_time time = deviceData->_CONTINUES_FEEDBACK_ENDING_MESSAGE.gps_data.time;
		_gps_data_date date = deviceData->_CONTINUES_FEEDBACK_ENDING_MESSAGE.gps_data.date;

		double dlat = std::atof(GPScharsToString(lat.degree).c_str()) + (std::atof(GPScharsToString(lat.minutes).c_str()) / 60);
		if (GPScharsToString(lat.direction)._Equal("S"))
			dlat = -dlat;

		double dlon = std::atof(GPScharsToString(lon.degree).c_str()) + (std::atof(GPScharsToString(lon.minutes).c_str()) / 60);
		if (GPScharsToString(lon.direction)._Equal("W"))
			dlon = -dlon;

		//We sql server timestamp format - YYYY-MM-DD HH:MI:SS
		std::string timeStamp = "20" + GPScharsToString(date.year) + "-" + GPScharsToString(date.month) + "-" + GPScharsToString(date.day) + " " + GPScharsToString(time.hh) + ":" + GPScharsToString(time.mm) + ":" + GPScharsToString(time.ss);

		//std::string datetime = Utils::makeDateTimeFromGPSData(date, time);
		std::string id = GPScharsToString(deviceData->_CONTINUES_FEEDBACK_ENDING_MESSAGE.id);

		std::string speed = GPScharsToString(deviceData->_CONTINUES_FEEDBACK_ENDING_MESSAGE.gps_data.speed); //The unit is km/h
		double dspeed = std::stod(speed);

		std::string orientation = GPScharsToString(deviceData->_CONTINUES_FEEDBACK_ENDING_MESSAGE.gps_data.orientation);
		double dorientation = std::stod(orientation);

		std::string IOState = GPScharsToString(deviceData->_CONTINUES_FEEDBACK_ENDING_MESSAGE.gps_data.IOState);
		bool main_power_switch_on = IOState[0] == '0' ? true : false;
		bool acc_ignition_on = IOState[1] == '1' ? true : false;

		std::string MilePost = GPScharsToString(deviceData->_CONTINUES_FEEDBACK_ENDING_MESSAGE.gps_data.MilePost);
		std::string sMilepost = "";
		if (MilePost._Equal("L")) {
			sMilepost = "Mileage";
		}

		std::string MileData = GPScharsToString(deviceData->_CONTINUES_FEEDBACK_ENDING_MESSAGE.gps_data.MileData);
		std::string _s("0x");
		_s.append(MileData);
		unsigned long dmile_data = 0;
		std::istringstream iss(_s);
		iss >> std::hex >> dmile_data;

		device_feedback *feedback = new device_feedback();
		feedback->acc_ignition_on = acc_ignition_on;
		strcpy_s(feedback->deviceId, this->deviceId.c_str());
		feedback->dlat = dlat;
		feedback->dlon = dlon;
		feedback->dmile_data = dmile_data;
		feedback->dorientation = dorientation;
		feedback->dspeed = dspeed;
		feedback->main_power_switch_on = main_power_switch_on;

		datetime __datetime;
		__datetime.day = std::atoi(GPScharsToString(date.day).c_str());
		__datetime.month = std::atoi(GPScharsToString(date.month).c_str());
		__datetime.year = std::atoi(GPScharsToString(date.year).c_str());

		__datetime.hour = std::atoi(GPScharsToString(time.hh).c_str());
		__datetime.minute = std::atoi(GPScharsToString(time.mm).c_str());
		__datetime.second = std::atoi(GPScharsToString(time.ss).c_str());

		feedback->_dateTime = __datetime;

		this->handlers->log_feedback(feedback);
		delete feedback;
	}

	std::string output = "";
	return output;
}

std::string gps_service::deviceHandshake(data_payload_from_device*  deviceData)
{
	std::string id(deviceData->_HANDSHAKE_SIGNAL_MESSAGE.id, sizeof(deviceData->_HANDSHAKE_SIGNAL_MESSAGE.id));
	std::string deviceId(deviceData->_HANDSHAKE_SIGNAL_MESSAGE.device_id, sizeof(deviceData->_HANDSHAKE_SIGNAL_MESSAGE.device_id));

	//Get list of configured devices...
	bool isAKnownDevice = this->handlers->is_device_registered(deviceId.c_str());

	//
	if (isAKnownDevice)
	{
		std::string output = std::move(Utils::formDeviceResponse(id.c_str(), "AP01", "HSO"));
		return output;
	}
	else
	{
		return "";
	}
}

void gps_service::set_handlers(LPGPS_HANDLERS handlers)
{
	this->handlers = handlers;
}

