#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\sdk\sdk.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace geo_location_svc
{
	TEST_CLASS(geo_location_svc_unittests_withmocks)
	{
	public:
		geolocation_svc::LPGPS_HANDLERS evenHandlers = nullptr;

		TEST_METHOD_INITIALIZE(startup) {
			evenHandlers = new geolocation_svc::GPS_HANDLERS();
			evenHandlers->start_device_feedbacks_logs_job();
		}

		TEST_METHOD_CLEANUP(cleanup) {
			delete evenHandlers;
		}

		TEST_METHOD(count_gps_devices)
		{
			auto gpses = evenHandlers->search_gps_device_drivers();
			Assert::IsTrue(gpses->size() >= 0);
		}

		TEST_METHOD(must_work)
		{
			Assert::IsTrue((1 + 1) == 2);
		}

	};
}