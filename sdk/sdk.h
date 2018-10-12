#pragma once
#include <memory>

enum __data_store {
	SQLDB,
	MONGODB,
	REDIS
};

struct datetime {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	unsigned long fraction;
	int frac_precision;
};


struct device_feedback
{
	struct datetime _dateTime;
	double dlat;
	double dlon;
	char deviceId[15];
	double dorientation;
	double dspeed;
	bool main_power_switch_on;
	bool acc_ignition_on;
	double dmile_data;
};

struct device_login
{
	char deviceId[15];
};

struct __gps__ {
	void  (*log_feedback)(device_feedback*, __data_store);
	bool (*is_device_registered)(char *);
};

typedef struct __gps__ GPS_HANDLERS;