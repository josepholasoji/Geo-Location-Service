#pragma once
class geo_location_svc
{
public:
	geo_location_svc();
	~geo_location_svc();
	int start();
	int stop();
	int status();
};

