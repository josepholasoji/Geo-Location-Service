struct data_upstream
{
	unsigned short version;
	unsigned char* message_no_or_time;
	unsigned int message_no_or_time_len;
	unsigned char* device_version;
	unsigned int device_version_len;
	unsigned char* payload;
	unsigned int payload_len;
	unsigned char* control_data;
	unsigned int control_data_len;
};