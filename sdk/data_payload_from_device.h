#pragma once

struct _gps_data_date
{
	char year[2],
		month[2],
		day[2];
};

struct _gps_data_time
{
  char	hh[2],
		mm[2],
		ss[2];
};

struct _latitude
{
	char degree[2],
		minutes[7],
		direction[1];//N or S	
};

struct _longitude
{
	char degree[3],
		minutes[7],
		direction[1];//N or S	
};

struct _gps_data
{
	struct _gps_data_date date;
	char	dataAvailable[1];//A: Availbale, V: Invalidation
	struct _latitude latitude;
	struct _longitude longitude;
	char	speed[5];
	struct _gps_data_time time;
	char	orientation[6],
			IOState[8],
			MilePost[1],
			MileData[8];
};

struct MESSAGE_AND_ID_ONLY { char start[1], id[12], command[4], end[1]; };
struct HANDSHAKE_SIGNAL_MESSAGE { char start[1], id[12], command[4], device_id[15], body[3], end[1]; };
struct CONTINUOUS_ANSWER_SETTING_ISOCHRONOUS_FEEDBACK_MESSAGE { char start[1], id[12], command[4], news_interval[4], return_time_hr[2], return_time_min[2], end[1]; };
struct ISOCHRONOUS_CONTINUES_FEEDBACK_CONFIGURATION { char start[1], id[12], command[4], message_interval[4], total_time_hr[2], total_time_min[2], end[1]; };
struct LOGIN_MESSAGE { char start[1], id[12], command[4], device_id[15]; _gps_data gps_data; char end[1]; };
struct ALARM_MESSAGE { char start[1], id[12], command[4], alarm_information_code[1]; _gps_data gps_data; char end[1]; };
struct ANSWER_CALLING_MESSAGE { char start[1], id[12], command[4]; _gps_data gps_data; char end[1]; };
struct ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE { char start[1], id[12], command[4]; _gps_data gps_data; char end[1]; };
struct CONTINUES_FEEDBACK_ENDING_MESSAGE { char start[1], id[12], command[4]; _gps_data gps_data; char end[1]; };
struct ANSWER_THE_RESTARTED_MESSAGE_OF_THE_DEVICE { char start[1], id[12], command[4], body[60], end[1]; };
struct SETUP_THE_SPEED_OF_THE_CAR { char start[1], id[12], command[4], body[60], end[1]; };
struct ANSWER_THE_SETTING_ACC_OPEN_SENDING_DATA_INTERVALS { char start[1], id[12], command[4], end[1]; };
struct CONTROL_CIRCUIT { char start[1], id[12], command[4], body[1], end[1]; };
struct CONTROL_OIL { char start[1], id[12], command[4], body[1], end[1]; };
struct ANSWER_THE_SETTING_GEOFENCE_MESSAGE { char start[1], id[12], command[4], body[1], end[1]; };
struct ANSWER_THE_SETTING_ACC_CLOSE_SENDING_DATA_INTERVALS { char start[1], id[12], command[4], end[1]; };

union data_payload_from_device
{
	struct MESSAGE_AND_ID_ONLY _MESSAGE_AND_ID_ONLY;
	struct HANDSHAKE_SIGNAL_MESSAGE _HANDSHAKE_SIGNAL_MESSAGE;
	struct CONTINUOUS_ANSWER_SETTING_ISOCHRONOUS_FEEDBACK_MESSAGE _CONTINUOUS_ANSWER_SETTING_ISOCHRONOUS_FEEDBACK_MESSAGE;
	struct LOGIN_MESSAGE _LOGIN_MESSAGE;
	struct ALARM_MESSAGE _ALARM_MESSAGE;
	struct ANSWER_CALLING_MESSAGE _ANSWER_CALLING_MESSAGE;
	struct ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE _ISOCHRONOUS_FOR_CONTINUES_FEEDBACK_MESSAGE;
	struct CONTINUES_FEEDBACK_ENDING_MESSAGE _CONTINUES_FEEDBACK_ENDING_MESSAGE;
	struct ANSWER_THE_RESTARTED_MESSAGE_OF_THE_DEVICE _ANSWER_THE_RESTARTED_MESSAGE_OF_THE_DEVICE;
	struct SETUP_THE_SPEED_OF_THE_CAR _SETUP_THE_SPEED_OF_THE_CAR;
	struct ANSWER_THE_SETTING_ACC_OPEN_SENDING_DATA_INTERVALS _ANSWER_THE_SETTING_ACC_OPEN_SENDING_DATA_INTERVALS;
	struct CONTROL_CIRCUIT _CONTROL_CIRCUIT;
	struct CONTROL_OIL _CONTROL_OIL;
	struct ANSWER_THE_SETTING_GEOFENCE_MESSAGE _ANSWER_THE_SETTING_GEOFENCE_MESSAGE;
	struct ANSWER_THE_SETTING_ACC_CLOSE_SENDING_DATA_INTERVALS _ANSWER_THE_SETTING_ACC_CLOSE_SENDING_DATA_INTERVALS;
	unsigned char raw_data[1024];
};


