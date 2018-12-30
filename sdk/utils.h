#pragma once

#define GPScharsToString(x) std::string(x, sizeof(x))
//Miscs
//#define GPScharsToString(x) std::string((char*)&x, sizeof(x))

class Utils
{
public:
	Utils() {};
	~Utils() {};
	void* detectDevice(char*, int) {
		return nullptr;
	};

	static const std::string formDeviceResponse(const char *id, const char* command, char* body) {
		return std::string("(" + std::string(id) + std::string(command) + (body != nullptr ? std::string(body) : "") + ")");
	};
};
//std::string Utils::makeDateTimeFromGPSData(_gps_data_date date, _gps_data_time time)
//{
//	return "";
//}