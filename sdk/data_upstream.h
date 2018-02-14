struct data_upstream
{
	unsigned short version;
	unsigned char* message_no_or_time;
	unsigned int message_no_or_time_len;
	unsigned char* device_version;
	unsigned int device_version_len;
	union _data_upstream_payload payload;
	unsigned int payload_len;
	unsigned char* control_data;
	unsigned int control_data_len;
};

struct _data_upstream_payload_
{

};

union _data_upstream_payload
{
	unsigned char[1024]
};
