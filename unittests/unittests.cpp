#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\sdk\utils.h"
#include "..\sdk\data_payload_from_device.h"
#include "modelutil.h"
#include <iostream>
#include "..\tk103\tk103.h"
#include <tuple>

#pragma comment(lib, " ")
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittests
{		
	TEST_CLASS(unittests)
	{
	public:
		
		TEST_METHOD(scanAndLoadAllLibraries)
		{
			// TODO: Your test code here
		}

		TEST_METHOD(convertHexValuesToAscii)
		{
			std::string asciiValue;
			utils::hex2ascii(tk103_hex_gps_data, asciiValue);
			std::cout << asciiValue;
			// TODO: Your test code here
		}

		TEST_METHOD(testTK103DataProcessor)
		{
			Ctk103 tk103;
			data_payload_from_device* _data_payload_from_device = {0};
			auto _tuple = utils::strip_start_and_end_tags(tk103_ascii_gps_data);
			
			auto proccessd_data = tk103.process(std::get<0>(_tuple));
			_data_payload_from_device = (data_payload_from_device*)std::get<0>(proccessd_data);
			int length = (int)std::get<1>(proccessd_data);
			Assert::IsNotNull(_data_payload_from_device);

			std::string command_string = std::string(_data_payload_from_device->_MESSAGE_AND_ID_ONLY.command, sizeof(_data_payload_from_device->_MESSAGE_AND_ID_ONLY.command));
			auto message_entry = tk103.device_command_message.find(command_string);
			Assert::IsFalse(message_entry == tk103.device_command_message.end());

			struct _command_message message = message_entry->second;
			Assert::IsNotNull(message.message_description);
		}
	};
}