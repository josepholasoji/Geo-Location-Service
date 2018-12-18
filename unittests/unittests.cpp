#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\sdk\utils.h"
#include "..\sdk\gps.h"
#include "..\sdk\data_payload_from_device.h"
#include "../tk103/tk103.h"
#include "..\sdk\sdk.h"
#include "modelutil.h"
#include <memory>
#include <tuple>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define OTL_ODBC 
#define OTL_ANSI_CPP_11_NULLPTR_SUPPORT
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE

#include "../geo_location/otlv4.h"
#include <iostream>

namespace unittests
{
	TEST_CLASS(unittests_withmocks)
	{
	public:
		LPGPS_HANDLERS evenHandlers = nullptr;

		TEST_METHOD_INITIALIZE(startup) {
			evenHandlers = new GPS_HANDLERS();
			evenHandlers->is_device_registered = [](const char *) {return true; };
			evenHandlers->log_feedback = [](device_feedback* device_feeback)->void {
				try
				{
					otl_connect db;
					otl_connect::otl_initialize();
					db.rlogon("DRIVER={MySQL ODBC 8.0 ANSI Driver};SERVER=127.0.0.1;PORT=3306;DATABASE=geolocation_service;USER=root;PASSWORD=;");

					//
					otl_datetime _dateTime;
					_dateTime.day = device_feeback->_dateTime->day;
					_dateTime.month = device_feeback->_dateTime->month;
					_dateTime.year = std::atoi(std::string((device_feeback->_dateTime->year < 10 ? "200" : "20") + std::to_string(device_feeback->_dateTime->year)).c_str());
					_dateTime.hour = device_feeback->_dateTime->hour;
					_dateTime.minute = device_feeback->_dateTime->minute;
					_dateTime.second = device_feeback->_dateTime->second;

					//
					otl_stream o(1,
						"{call add_device_location_log(:time<timestamp,in>,:latitude<double,in>,:longitude<double,in>,:device_id<char[20],in>,:orientation<double,in>,:speed<double,in>,:power_switch_is_on<int,in>,:igintion_is_on<int,in>,:miles_data<double,in>)}",
						db);

					o.set_commit(0);

					o << _dateTime
						<< device_feeback->dlat
						<< device_feeback->dlon
						<< device_feeback->deviceId
						<< device_feeback->dorientation
						<< device_feeback->dspeed
						<< (device_feeback->main_power_switch_on ? 1 : 0)
						<< (device_feeback->acc_ignition_on ? 1 : 0)
						<< (double)device_feeback->dmile_data;
				}
				catch (otl_exception& p) {

				}
			};
		}

		TEST_METHOD_CLEANUP(cleanup) {
			delete evenHandlers;
		}

		TEST_METHOD(convertHexValuesToAscii)
		{
			std::string asciiValue;
			//utils::hex2ascii(tk103_hex_gps_data, asciiValue);
			std::cout << asciiValue;
			// TODO: Your test code here
		}

		TEST_METHOD(testTK103DataProcessor)
		{
			Ctk103* _gps = (Ctk103*)load(evenHandlers);

			data_payload_from_device* _data_payload_from_device = { 0 };

			_data_payload_from_device = (data_payload_from_device*)(tk103_ascii_isochronous_continuos_feedback_message);
			Assert::IsNotNull(_data_payload_from_device);

			std::string command_string = std::string(_data_payload_from_device->_MESSAGE_AND_ID_ONLY.command, sizeof(_data_payload_from_device->_MESSAGE_AND_ID_ONLY.command));
			auto message_entry = _gps->deviceCommandMessage().find(command_string);
			Assert::IsFalse(message_entry == _gps->deviceCommandMessage().end());

			struct _command_message message = message_entry->second;
			Assert::IsNotNull(message.message_description);
		}

		TEST_METHOD(testTK103DeviceLogin)
		{
			Ctk103* _gps = (Ctk103*)load(evenHandlers);

			data_payload_from_device* _data_payload_from_device = { 0 };

			_data_payload_from_device = (data_payload_from_device*)(tk103_ascii_device_login_request);
			Assert::IsNotNull(_data_payload_from_device);

			//DB setup should be setup for this test to pass: device id => 000013632782450
			std::string output = _gps->process((char*)tk103_ascii_device_login_request, 0);
			Assert::AreEqual(tk103_ascii_device_login_response, output.c_str());
		}

		TEST_METHOD(testTK103DeviceHandshake)
		{
			Ctk103* _gps = (Ctk103*)load(evenHandlers);

			data_payload_from_device* _data_payload_from_device = { 0 };

			_data_payload_from_device = (data_payload_from_device*)(tk103_ascii_device_handshake_request);
			Assert::IsNotNull(_data_payload_from_device);

			std::string output = _gps->process((char*)tk103_ascii_device_handshake_request, 0);
			Assert::AreEqual(tk103_ascii_device_handshake_response, output.c_str());
		}

		TEST_METHOD(testTK103DeviceFeedbackMessage)
		{
			Ctk103* _gps = (Ctk103*)load(evenHandlers);

			data_payload_from_device* _data_payload_from_device = { 0 };

			_data_payload_from_device = (data_payload_from_device*)(tk103_ascii_device_feedback_request);
			Assert::IsNotNull(_data_payload_from_device);

			std::string output = _gps->process((char*)tk103_ascii_device_feedback_request, 0);
			Assert::AreEqual(tk103_ascii_device_feedback_response, output.c_str());
		}

		TEST_METHOD(testTK103DeviceFeedbacEndingMessage)
		{
			Ctk103* _gps = (Ctk103*)load(evenHandlers);

			data_payload_from_device* _data_payload_from_device = { 0 };

			_data_payload_from_device = (data_payload_from_device*)(tk103_ascii_device_feedback_ending_request);
			Assert::IsNotNull(_data_payload_from_device);

			std::string output = _gps->process((char*)tk103_ascii_device_feedback_ending_request, 0);
			Assert::AreEqual(tk103_ascii_device_feedback_response, output.c_str());
		}
	};
}