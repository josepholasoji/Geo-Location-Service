#pragma once

#include<string>
#include <sstream>

#include "..\sdk\data_payload_from_device.h"
#include "..\sdk\data_downstream.h"
#include "..\sdk\sdk.h"


class gps_service
{
private:
	LPGPS_HANDLERS handlers;

public:
	gps_service();
	~gps_service();

	std::string deviceId;

	//Device => Server specific messages
	std::string deviceLogin(data_payload_from_device*  deviceData);
	std::string deviceFeedback(data_payload_from_device*  deviceData);
	std::string deviceFeedbackEnding(data_payload_from_device*  deviceData);
	std::string deviceHandshake(data_payload_from_device*  deviceData);
	void set_handlers(LPGPS_HANDLERS);
};

