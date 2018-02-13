#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\sdk\utils.h"
#include "modelutil.h"
#include <iostream>


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
	};
}