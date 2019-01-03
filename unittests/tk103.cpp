#include "stdafx.h"
#include "CppUnitTest.h"

#include <memory>
#include <tuple>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define OTL_ODBC 
#define OTL_ANSI_CPP_11_NULLPTR_SUPPORT
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE

#include "../sdk/otlv4.h"
#include <iostream>

namespace tk103
{
	TEST_CLASS(tk103_unittests_withmocks)
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

		

		TEST_METHOD(

		TEST_METHOD(

		TEST_METHOD(

		TEST_METHOD(

		TEST_METHOD(
	};
}