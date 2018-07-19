#pragma once

#define GPScharsToString(x) std::string(x, sizeof(x))

class Utils
{
public:
	Utils();
	~Utils();
	void* Utils::detectDevice(char*, int);
	static std::string Utils::formDeviceResponse(const char *id, char* command, char* body);
};



Utils::Utils()
{
}


Utils::~Utils()
{
}

void* Utils::detectDevice(char* data, int dataSize)
{
	return nullptr;
}

std::string Utils::formDeviceResponse(const char *id, char* command, char* body)
{
	return std::string("(" + std::string(id) + std::string(command) + (body != nullptr ? std::string(body) : "") + ")");
}

//std::string Utils::makeDateTimeFromGPSData(_gps_data_date date, _gps_data_time time)
//{
//	return "";
//}