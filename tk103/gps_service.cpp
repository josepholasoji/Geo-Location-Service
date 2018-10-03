#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include "gps_service.h"
#include "..\sdk\Utils.h"

using namespace std;

gps_service::gps_service()
{
	deviceId = std::string("123456");
	otl_connect::otl_initialize();

	try
	{
		std::string strConn = "DRIVER={MySQL ODBC 8.0 ANSI Driver};SERVER=127.0.0.1;PORT=3306;DATABASE=geolocation_service;USER=root;PASSWORD=;";
		this->db.rlogon(strConn.c_str());
	}
	catch (otl_exception& p)
	{
		cerr << p.msg << endl; // print out error message
		cerr << p.code << endl; // print out error code
		cerr << p.var_info << endl; // print out the variable that caused the error
		cerr << p.sqlstate << endl; // print out SQLSTATE message
		cerr << p.stm_text << endl; // print out SQL that caused the error
	}
}


gps_service::~gps_service()
{
	db.logoff();
}

std::string gps_service::deviceLogin(data_payload_from_device*  deviceData)
{
	std::string id(deviceData->_LOGIN_MESSAGE.id, sizeof(deviceData->_LOGIN_MESSAGE.id));
	std::string deviceId(deviceData->_LOGIN_MESSAGE.device_id, sizeof(deviceData->_LOGIN_MESSAGE.device_id));

	//Get list of configured devices...
	bool isAKnownDevice = this->isDeviceDefined(deviceId);
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

bool gps_service::isDeviceDefined(std::string id) {
	try {
		int is_device_registered = 0;
		otl_stream o(1, "{call find_device(:device_id<char[20],in>, @registered)}", this->db);
		o.set_commit(0);
		o << id.c_str();

		//read all the outut...
		otl_stream s(1, "select @registered  :#1<int>", db, otl_implicit_select);
		s >> is_device_registered;
		return is_device_registered > 0;
	}
	catch (otl_exception& p) {
		cerr << p.msg << endl; // print out error message
		cerr << p.code << endl; // print out error code
		cerr << p.var_info << endl; // print out the variable that caused the error
		cerr << p.sqlstate << endl; // print out SQLSTATE message
		cerr << p.stm_text << endl; // print out SQL that caused the error
	}

	return false;
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
		otl_datetime _dateTime;
		_dateTime.day = std::atoi(GPScharsToString(date.day).c_str());
		_dateTime.month = std::atoi(GPScharsToString(date.month).c_str());

		
		_dateTime.year = std::atoi(std::string("20" + GPScharsToString(date.year)).c_str());
		_dateTime.hour = std::atoi(GPScharsToString(time.hh).c_str());
		_dateTime.minute = std::atoi(GPScharsToString(time.mm).c_str());
		_dateTime.second = std::atoi(GPScharsToString(time.ss).c_str());

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


		//save the location details
		try
		{
			int is_device_registered = 0;
			otl_stream o(1, 
				"{call add_device_location_log(:time<timestamp,in>,:latitude<double,in>,:longitude<double,in>,:device_id<char[20],in>,:orientation<double,in>,:speed<double,in>,:power_switch_is_on<int,in>,:igintion_is_on<int,in>,:miles_data<double,in>)}",
				this->db);

			//o.set_commit(0);

			o << _dateTime
			  << dlat 
			  << dlon 
			  << this->deviceId.c_str() 
			  << dorientation 
		      << dspeed 
			  << (main_power_switch_on ? 1 : 0)
			  << (acc_ignition_on ? 1 : 0)
			  << (double) dmile_data;
		}
		catch (otl_exception& p) {
			cerr << p.msg << endl; // print out error message
			cerr << p.code << endl; // print out error code
			cerr << p.var_info << endl; // print out the variable that caused the error
			cerr << p.sqlstate << endl; // print out SQLSTATE message
			cerr << p.stm_text << endl; // print out SQL that caused the error
		}
	}

	//Send location to geo-location service for processing
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
		otl_datetime _dateTime;
		_dateTime.day = std::atoi(GPScharsToString(date.day).c_str());
		_dateTime.month = std::atoi(GPScharsToString(date.month).c_str());


		_dateTime.year = std::atoi(std::string("20" + GPScharsToString(date.year)).c_str());
		_dateTime.hour = std::atoi(GPScharsToString(time.hh).c_str());
		_dateTime.minute = std::atoi(GPScharsToString(time.mm).c_str());
		_dateTime.second = std::atoi(GPScharsToString(time.ss).c_str());

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


		//save the location details
		try
		{
			int is_device_registered = 0;
			otl_stream o(1,
				"{call add_device_location_log(:time<timestamp,in>,:latitude<double,in>,:longitude<double,in>,:device_id<char[20],in>,:orientation<double,in>,:speed<double,in>,:power_switch_is_on<int,in>,:igintion_is_on<int,in>,:miles_data<double,in>)}",
				this->db);

			//o.set_commit(0);

			o << _dateTime
				<< dlat
				<< dlon
				<< this->deviceId.c_str()
				<< dorientation
				<< dspeed
				<< (main_power_switch_on ? 1 : 0)
				<< (acc_ignition_on ? 1 : 0)
				<< (double)dmile_data;
		}
		catch (otl_exception& p) {
			cerr << p.msg << endl; // print out error message
			cerr << p.code << endl; // print out error code
			cerr << p.var_info << endl; // print out the variable that caused the error
			cerr << p.sqlstate << endl; // print out SQLSTATE message
			cerr << p.stm_text << endl; // print out SQL that caused the error
		}
	}

	//Send location to geo-location service for processing
	std::string output = "";
	return output;
}

std::string gps_service::deviceHandshake(data_payload_from_device*  deviceData)
{
	std::string id(deviceData->_HANDSHAKE_SIGNAL_MESSAGE.id, sizeof(deviceData->_HANDSHAKE_SIGNAL_MESSAGE.id));
	std::string deviceId(deviceData->_HANDSHAKE_SIGNAL_MESSAGE.device_id, sizeof(deviceData->_HANDSHAKE_SIGNAL_MESSAGE.device_id));

	//Get list of configured devices...
	bool isAKnownDevice = this->isDeviceDefined(deviceId);

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

