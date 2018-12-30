#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\sdk\utils.h"
#include "../sdk/gps.h"
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

#include "../sdk/otlv4.h"
#include <iostream>

namespace tk103
{
	TEST_CLASS(tk103_unittests_withmocks)
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