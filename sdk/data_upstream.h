struct data_upstream
{
	unsigned word version;
	char* device_id;
	unsigned int device_id_len;
	char* device_version;
	unsigned int device_version_len;
	char* payload;
	unsigned int payload_len;
	char* control_data;
	unsigned int control_data_len;
};