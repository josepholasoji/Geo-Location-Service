#include "pch.h"
#include "../sdk/sdk.h"
#include "../tk103/tk103.h"
#include "../sdk/utils.h"
#include "../sdk/gps.h"
#include "../sdk/data_payload_from_device.h"
#include "../sdk/sdk.h"
#include "modelutil.h"
#include <thread>
#include <chrono>

geolocation_svc::LPGPS_HANDLERS sdkHandle = NULL;
Ctk103* _gps = NULL;

class TK103FunctionalityTests: public testing::Test {

public:


	TK103FunctionalityTests() {}

	~TK103FunctionalityTests() {}

	virtual void SetUp() {}

	virtual void TearDown() {}

    static void SetUpTestCase() {
		sdkHandle = new geolocation_svc::GPS_HANDLERS();
		sdkHandle->start_device_feedbacks_logs_job();
		_gps = (Ctk103*)load(sdkHandle);
	}
	
	static void TearDownTestCase(){
		delete sdkHandle;	
	}
};

TEST_F(TK103FunctionalityTests, convertHexValuesToAscii)
{
	std::string asciiValue;
	//utils::hex2ascii(tk103_hex_gps_data, asciiValue);
	std::cout << asciiValue;
	// TODO: Your test code here
}

TEST_F(TK103FunctionalityTests, testTK103DataProcessor)
{
	data_payload_from_device* _data_payload_from_device = { 0 };

	_data_payload_from_device = (data_payload_from_device*)(tk103_ascii_isochronous_continuos_feedback_message);
	EXPECT_TRUE(NULL != _data_payload_from_device);

	std::string command_string = std::string(_data_payload_from_device->_MESSAGE_AND_ID_ONLY.command, sizeof(_data_payload_from_device->_MESSAGE_AND_ID_ONLY.command));
	auto commandMap = _gps->deviceCommandMessage();
	auto message_entry = commandMap.find(command_string);
	EXPECT_FALSE(message_entry == commandMap.end());

	struct _command_message message = message_entry->second;
	EXPECT_TRUE(NULL != message.message_description);
}

TEST_F(TK103FunctionalityTests, testTK103DeviceLogin)
{
	data_payload_from_device* _data_payload_from_device = { 0 };

	_data_payload_from_device = (data_payload_from_device*)(tk103_ascii_device_login_request);
	EXPECT_TRUE(NULL != _data_payload_from_device);

	//DB setup should be setup for this test to pass: device id => 000013632782450
	std::string output = _gps->process((char*)tk103_ascii_device_login_request, 0);
	EXPECT_TRUE(output.compare(tk103_ascii_device_login_response) == 0);
}

TEST_F(TK103FunctionalityTests, testTK103DeviceHandshake)
{
	data_payload_from_device* _data_payload_from_device = { 0 };

	_data_payload_from_device = (data_payload_from_device*)(tk103_ascii_device_handshake_request);
	EXPECT_TRUE(NULL != _data_payload_from_device);

	std::string output = _gps->process((char*)tk103_ascii_device_handshake_request, 0);
	EXPECT_TRUE(output.compare(tk103_ascii_device_handshake_response) == 0);
}

TEST_F(TK103FunctionalityTests, testTK103DeviceFeedbackMessage)
{
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(3s);

		data_payload_from_device* _data_payload_from_device = { 0 };

		_data_payload_from_device = (data_payload_from_device*)(tk103_ascii_device_feedback_request);
		EXPECT_TRUE(NULL != _data_payload_from_device);

		std::string output = _gps->process((char*)tk103_ascii_device_feedback_request, 0);
		EXPECT_TRUE(output.compare(tk103_ascii_device_feedback_response) == 0);

	std::this_thread::sleep_for(3s);
}

TEST_F(TK103FunctionalityTests, testTK103DeviceFeedbacEndingMessage)
{
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(3s);

		data_payload_from_device* _data_payload_from_device = { 0 };

		_data_payload_from_device = (data_payload_from_device*)(tk103_ascii_device_feedback_ending_request);
		EXPECT_TRUE(_data_payload_from_device != NULL);

		std::string output = _gps->process((char*)tk103_ascii_device_feedback_ending_request, 0);
		EXPECT_TRUE(output.compare(tk103_ascii_device_feedback_response) == 0);

	std::this_thread::sleep_for(3s);
}
