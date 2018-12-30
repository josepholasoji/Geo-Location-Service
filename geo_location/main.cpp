
#include "../sdk/stdafx.h"
#include "server.h"
#include "../sdk/sdk.h"
#include <boost/asio.hpp>

int main()
{
	//
	std::vector<server> servers;
	boost::asio::io_service io_service;
	auto gps_service = std::make_shared<geolocation_svc::GPS_HANDLERS>();

	//search for gps devices...
	auto gpses = gps_service->search_gps_device_drivers();

	//start the device logger jobs...
	gps_service->start_device_feedbacks_logs_job();

	//build the services...
	for (gps* _gps : *gpses) {
		LOG_WARN << "Starting device services for " << _gps->deviceName() << " @ " << _gps->serverPort();
		new server(io_service, _gps->serverPort(), _gps);
	}

	//start the service...
	io_service.run();
	return 0;
}
