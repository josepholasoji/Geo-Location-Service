#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

class geo_location_svc
{
public:
	geo_location_svc();
	~geo_location_svc();
	int start();
	int stop();
	int status();
};

