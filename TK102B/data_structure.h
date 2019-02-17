#pragma once
#include <string>
#include "gps_message_body.h"
#include <cinttypes>

struct data_structure
{
	unsigned char* serial_number;
	int serial_number_len;
	unsigned char* time;
	unsigned char* time_len;
	std::int32_t command;
};

