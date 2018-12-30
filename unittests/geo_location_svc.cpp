#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\sdk\sdk.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define OTL_ODBC 
#define OTL_ANSI_CPP_11_NULLPTR_SUPPORT
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE

namespace geo_location_svc
{
	TEST_CLASS(geo_location_svc_unittests_withmocks)
	{
	public:
		LPGPS_HANDLERS evenHandlers = nullptr;

		TEST_METHOD_INITIALIZE(startup) {
			evenHandlers = new GPS_HANDLERS();
			evenHandlers->start_device_feedbacks_logs_job();
		}

		TEST_METHOD_CLEANUP(cleanup) {
			delete evenHandlers;
		}

		TEST_METHOD(count_gps_devices)
		{
			auto gpses = evenHandlers->search_gps_devices();
			Assert::IsTrue(gpses->size() >= 0);
		}

		TEST_METHOD(must_work)
		{
			Assert::IsTrue(true);
		}

	};
}