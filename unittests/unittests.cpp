#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\sdk\utils.h"
#include "..\sdk\data_upstream.h"
#include "modelutil.h"
#include <iostream>
#include "..\tk103\tk103.h"

#pragma comment(lib, "tk103.lib")
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
			data_upstream du;
			tk103.process(((unsigned char*)tk103_ascii_gps_data), strlen(tk103_hex_gps_data), &du);
			std::cout << du.control_data;
			std::cout << du.message_no_or_time;
			std::cout << du.payload;
			// TODO: Your test code here
		}
	};
}