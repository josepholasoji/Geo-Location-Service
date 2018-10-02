#pragma once

#include<string>
#include <sstream>
#define OTL_ODBC 
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE

#include "..\sdk\otlv4.h"
#include "..\sdk\data_payload_from_device.h"
#include "..\sdk\data_downstream.h"


class gps_service
{
private:
	otl_connect db;

public:
	gps_service();
	~gps_service();

	std::string deviceId;

	//Device => Server specific messages
	std::string deviceLogin(data_payload_from_device*  deviceData);
	std::string deviceFeedback(data_payload_from_device*  deviceData);
	std::string deviceFeedbackEnding(data_payload_from_device*  deviceData);
	std::string deviceHandshake(data_payload_from_device*  deviceData);
	bool isDeviceDefined(std::string id);
};

