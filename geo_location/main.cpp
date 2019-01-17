#if defined(_MSC_VER)
	#include "../sdk/stdafx.h"
#endif
#include "server.h"

#include <boost/asio.hpp>

#ifndef GLS_IMPORTS
	#define GLS_IMPORTS
#endif // !GLS_IMPORTS

#include "../sdk/NanoLog.hpp"
#include "../sdk/sdk.h"
int main()
{
	auto gps_service = std::make_shared<geolocation_svc::GPS_HANDLERS>();

	//search for gps devices...
	auto gpses = gps_service->search_gps_device_drivers();

	//start the device logger jobs...
	gps_service->start_device_feedbacks_logs_job();

	//build the services...
	boost::asio::io_service io_service;
	for (gps* _gps : *gpses) {
		try {
			LOG_WARN << "Starting device services for " << _gps->deviceName() << " @ " << _gps->serverPort();
			new server(io_service, _gps->serverPort(), _gps);
		}
		catch (std::exception ex) {
			LOG_WARN << "Error starting device services for " << _gps->deviceName() << " @ " << _gps->serverPort();
			LOG_WARN << "Error message: " << ex.what();

			const char* ex_message = ex.what();
		}
	}

	//start the service...
	io_service.run();
	return 0;
}
