#include <zmq.h>
#include <zmq_utils.h>
#include <boost/asio.hpp>
#include <memory>

#include "server.h"
#include "../sdk/sdk.h"

#define OTL_ODBC 
#define OTL_ANSI_CPP_11_NULLPTR_SUPPORT
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE

#include "otlv4.h"

using namespace std;



typedef int(__stdcall *f_funci)();
typedef gps*(__stdcall *f_load)();


otl_connect db;
char dir_path[] = "./gps";
GPS_HANDLERS *handlers = nullptr;

void log_feedback(device_feedback* device_feeback) {

}

void log_feedback_sql(device_feedback* device_feeback) {
	//save the location details
	try
	{
		int is_device_registered = 0;
		otl_stream o(1,
			"{call add_device_location_log(:time<timestamp,in>,:latitude<double,in>,:longitude<double,in>,:device_id<char[20],in>,:orientation<double,in>,:speed<double,in>,:power_switch_is_on<int,in>,:igintion_is_on<int,in>,:miles_data<double,in>)}",
			db);

		o.set_commit(0);

		otl_datetime _dateTime;
		_dateTime.day = device_feeback->_dateTime.day;
		_dateTime.month = device_feeback->_dateTime.month;		
		_dateTime.year = std::atoi(std::string("20" +  std::to_string(device_feeback->_dateTime.year)).c_str());
		_dateTime.hour = device_feeback->_dateTime.hour;
		_dateTime.minute = device_feeback->_dateTime.minute;
		_dateTime.second = device_feeback->_dateTime.second;

		o << _dateTime
			<< device_feeback->dlat
			<< device_feeback->dlon
			<< device_feeback ->deviceId
			<< device_feeback ->dorientation
			<< device_feeback ->dspeed
			<< (device_feeback ->main_power_switch_on ? 1 : 0)
			<< (device_feeback ->acc_ignition_on ? 1 : 0)
			<< (double)device_feeback ->dmile_data;
	}
	catch (otl_exception& p) {
		cerr << p.msg << endl; // print out error message
		cerr << p.code << endl; // print out error code
		cerr << p.var_info << endl; // print out the variable that caused the error
		cerr << p.sqlstate << endl; // print out SQLSTATE message
		cerr << p.stm_text << endl; // print out SQL that caused the error
	}
}

bool is_device_registered(char* deviceId) {
	try {
		int is_device_registered = 0;
		otl_stream o(1, "{call find_device(:device_id<char[20],in>, @registered)}", db);
		o.set_commit(0);
		o << deviceId;

		//read all the outut...
		otl_stream s(1, "select @registered  :#1<int>", db, otl_implicit_select);
		s >> is_device_registered;
		return is_device_registered > 0;
	}
	catch (otl_exception& p) {
		cerr << p.msg << endl; // print out error message
		cerr << p.code << endl; // print out error code
		cerr << p.var_info << endl; // print out the variable that caused the error
		cerr << p.sqlstate << endl; // print out SQLSTATE message
		cerr << p.stm_text << endl; // print out SQL that caused the error
	}

	return false;
}

int main()
{
	otl_connect::otl_initialize();

	try
	{
		std::string strConn = "DRIVER={MySQL ODBC 8.0 ANSI Driver};SERVER=127.0.0.1;PORT=3306;DATABASE=geolocation_service;USER=root;PASSWORD=;";
		db.rlogon(strConn.c_str());
	}
	catch (otl_exception& p)
	{
		cerr << p.msg << endl; // print out error message
		cerr << p.code << endl; // print out error code
		cerr << p.var_info << endl; // print out the variable that caused the error
		cerr << p.sqlstate << endl; // print out SQLSTATE message
		cerr << p.stm_text << endl; // print out SQL that caused the error
	}

	handlers = (GPS_HANDLERS*)malloc(sizeof(GPS_HANDLERS));
	handlers->log_feedback = log_feedback;
	handlers->is_device_registered = is_device_registered;

	auto  gpses = std::make_shared<std::vector<gps*>>();

	//Start zero mq
	void *zmq_context = zmq_ctx_new();

	//Search the plugins directory for service plugins
	WIN32_FIND_DATA file = { 0 };
	char path[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, (LPWSTR)path);

	HANDLE search_handle = FindFirstFile(L"services\\*.gps", &file);
	if (search_handle)
	{
		do
		{
			//load each dynamically...
			HINSTANCE hGetProcIDDLL = LoadLibrary((LPCWSTR)std::wstring(L"services\\").append(file.cFileName).c_str());

			if (!hGetProcIDDLL) {
				std::cout << "could not load the dynamic library" << std::endl;
				return EXIT_FAILURE;
			}

			// resolve function address here
			f_load load = (f_load)GetProcAddress(hGetProcIDDLL, "load");
			if (!load) {
				std::cout << "could not locate the function" << std::endl;
				return EXIT_FAILURE;
			}

			gpses->push_back(load());
		} while (FindNextFile(search_handle, &file));
		FindClose(search_handle);
	}
			
	boost::asio::io_service io_service;
	std::vector<server> servers;
	//Start the servers...
	for (gps* _gps : *gpses)
	{
		new server(io_service, _gps->serverPort(), _gps);
	}

	io_service.run();

	auto on_exit = []{
		db.logoff();
		free((void*)handlers);
	};

	atexit(on_exit);
	
	return 0;
}
