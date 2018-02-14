#pragma once
#include <string>
#include "gps_message_body.h"

struct data_structure
{
	unsigned char* serial_number;
	int serial_number_len;
	unsigned char* time;
	unsigned char* time_len;
	__int32 command;
};

