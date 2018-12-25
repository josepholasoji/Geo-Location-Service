#ifndef _CRT_SECURE_NO_WARNINGS
	#define  _CRT_SECURE_NO_WARNINGS
#endif

#include <boost/asio.hpp>
#include <memory>

#include "server.h"
#include "../sdk/sdk.h"
#include "../sdk/zmq.hpp"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <iostream>
#include <sstream>
#include <thread>
#include <algorithm>


using namespace std;
using boost::property_tree::ptree;

const char dir_path[] = "./gps";
LPGPS_HANDLERS handlers = nullptr;

typedef int ( *f_funci)();
typedef LPGPS ( *f_load)(LPGPS_HANDLERS);


void 
#ifdef _MSC_VER 
__cdecl 
#endif 
start_feedbacklog_sql_job(void* params);
 

int main()
{
	try
	{
		handlers = (LPGPS_HANDLERS) new GPS_HANDLERS();
		otl_connect* db_addr = &handlers->db;
		std::tuple<zmq::context_t*, otl_connect*> *log_params = new std::tuple<zmq::context_t*, otl_connect*>(handlers->context, db_addr);

#ifdef _MSC_VER
		_beginthread(start_feedbacklog_sql_job, 1024, (void*)log_params);
#else
		pthread_t td;
		int err = pthread_create(&td, NULL, [](void* _log_params)->void* {
			start_feedbacklog_sql_job(_log_params);
			return NULL;
		}, (void*)log_params);
		if (err != 0)
			printf("\ncan't create thread :[%s]", strerror(err));
		else
			printf("\n Thread created successfully\n");
#endif 
	}
	catch (otl_exception& p)
	{
		LOG_WARN << (char*)p.msg; // print out error message
		LOG_WARN << p.code; // print out error code
		LOG_WARN << p.var_info; // print out the variable that caused the error
		LOG_WARN << (char*)p.sqlstate; // print out SQLSTATE message
		LOG_WARN << p.stm_text; // print out SQL that caused the error
	}
	catch (exception ex) {
		LOG_WARN << ex.what();
	}

	//
	auto  gpses = std::make_shared<std::vector<gps*>>();
	LOG_WARN << "Searching for GPS files...";
	#ifdef _MSC_VER

		//Search the plugins directory for service plugins
		WIN32_FIND_DATA file = { 0 };
		char path[MAX_PATH] = { 0 };
		GetCurrentDirectory(MAX_PATH, (LPWSTR)path);

		HANDLE search_handle = FindFirstFile(L"services\\*.gps", &file);
		if (search_handle) {
			do {
				//load each dynamically...
				auto gpsfile = std::wstring(L"services\\").append(file.cFileName);
				//LOG_WARN << "Found GPS file @ " << gpsfile.c_str();

				HINSTANCE hGetProcIDDLL = LoadLibrary((LPCWSTR)gpsfile.c_str());

				if (!hGetProcIDDLL) {
					std::cout << "could not load the GPS service file" << std::endl;
					LOG_WARN << "could not load the GPS service file";
					return EXIT_FAILURE;
				}

				// resolve function address here
				//LOG_WARN << "Loading GPS file: " << gpsfile;
				f_load load = (f_load)GetProcAddress(hGetProcIDDLL, "load");
				if (!load) {
					std::cout << "could not locate the function" << std::endl;
					LOG_WARN << "could not locate the function";
					return EXIT_FAILURE;
				}

				gpses->push_back(load(handlers));
			} while (FindNextFile(search_handle, &file));
			FindClose(search_handle);
		}

	#else

		struct dirent **namelist;
		int n = scandir("services", &namelist, [](const struct dirent *entry)->int {
			if (std::string(entry->d_name).find(".gps") != std::string::npos) {
				return true;
			}
			return false;
		}, alphasort);
		if (n < 0)
			perror("scandir");
		else {
			for (int i = 0; i < n; i++) {
				auto gpsfile = std::string("services/").append(namelist[i]->d_name);
				LOG_WARN << "Found GPS file @ " << gpsfile;

				void* handle = dlopen(gpsfile.c_str(), RTLD_LAZY);
				if (!handle) {
					cerr << "Cannot open library: " << dlerror() << '\n';
					LOG_WARN << "Cannot open library: " << dlerror() << '\n';
					return 1;
				}

				// reset errors
				dlerror();
				LOG_WARN << "Loading GPS file: " << gpsfile;
				f_load load = (f_load)dlsym(handle, "load");
				const char *dlsym_error = dlerror();
				if (dlsym_error) {
					cerr << "Cannot load symbol 'load' from " << namelist[i]->d_name << "; " << dlsym_error << '\n';
					LOG_WARN << "Cannot load symbol 'load' from " << namelist[i]->d_name << "; " << dlsym_error << '\n';

					dlclose(handle);
					return 1;
				}

				gpses->push_back(load(handlers));
				free(namelist[i]);
			}
		}

		free(namelist);
	#endif 

	boost::asio::io_service io_service;
	std::vector<server> servers;

	//Start the servers...
	for (gps* _gps : *gpses)
	{
		LOG_WARN << "Starting device services for " << _gps->deviceName() << " @ " << _gps->serverPort();
		new server(io_service, _gps->serverPort(), _gps);
	}

	io_service.run();

	//prepare of exit
	auto on_exit = [] {		
		delete handlers;
	};

	atexit(on_exit);
	return 0;
}

void 
#ifdef _MSC_VER 
	__cdecl 
#endif 
	start_feedbacklog_sql_job(void* params){

	LOG_WARN << "Starting the device feedback logger...";
	std::tuple<zmq::context_t*, otl_connect*>* tp = (std::tuple<zmq::context_t*, otl_connect*>*)params;
	zmq::context_t* zmq_context = std::get<0>(*tp);
	otl_connect* db = (otl_connect*)std::get<1>(*tp);

	LOG_WARN << "Starting the device feedback logger Queue...";
	

	//connect the subscriber
	zmq::socket_t* subscriber = new zmq::socket_t(*zmq_context, ZMQ_SUB);
	subscriber->setsockopt(ZMQ_LINGER, 0);
	subscriber->connect("tcp://localhost:5555");

	//add subscribtion filter for feeback messages only
	const char *filter = "";
	subscriber->setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
	LOG_WARN << "Device feedback logger queue started on port: 5555";

	while (true)
	{
		//recieve the 0MQ message
		zmq::message_t update;
		subscriber->recv(&update);
		auto ddata = static_cast<const char*>(update.data());
		//
		ptree root;
		std::string str(ddata, update.size());
		boost::trim(str);
		std::istringstream is(str);
		read_json(is, root);

		LOG_WARN << "Recieved data for logging: " << str;

		device_feedback *device_feeback = (device_feedback *)malloc(sizeof(device_feedback));
		device_feeback->acc_ignition_on = root.get<double>("acc_ignition_on", 0);

#ifdef _MSC_VER
		strcpy_s(device_feeback->deviceId, sizeof(device_feeback->deviceId), root.get<std::string>("deviceId").c_str());
#else
		strcpy(device_feeback->deviceId, root.get<std::string>("deviceId").c_str());
#endif 

		device_feeback->dlat = root.get<double>("dlat", 0);
		device_feeback->dlon = root.get<double>("dlon", 0);
		device_feeback->dmile_data = root.get<double>("dmile_data", 0);
		device_feeback->dorientation = root.get<double>("dorientation", 0);
		device_feeback->dspeed = root.get<double>("dspeed", 0);
		device_feeback->main_power_switch_on = root.get<int>("main_power_switch_on", 0);

		datetime* _datetime = (datetime *)malloc(sizeof(datetime));
		_datetime->hour = root.get<int>("_dateTime.hour", 0);
		_datetime->day = root.get<int>("_dateTime.day", 0);
		_datetime->minute = root.get<int>("_dateTime.minute", 0);
		_datetime->month = root.get<int>("_dateTime.month", 0);
		_datetime->second = root.get<int>("_dateTime.second", 0);
		_datetime->year = root.get<int>("_dateTime.year", 0);
		device_feeback->_dateTime = _datetime;

		//save the location details
		otl_datetime _dateTime;
		_dateTime.day = device_feeback->_dateTime->day;
		_dateTime.month = device_feeback->_dateTime->month;
		_dateTime.year = std::atoi(std::string((device_feeback->_dateTime->year < 10 ? "200" : "20") + std::to_string(device_feeback->_dateTime->year)).c_str());
		_dateTime.hour = device_feeback->_dateTime->hour;
		_dateTime.minute = device_feeback->_dateTime->minute;
		_dateTime.second = device_feeback->_dateTime->second;

		try
		{
			//
			otl_stream o(1024,
				"{call add_device_location_log(:time<timestamp,in>,:latitude<double,in>,:longitude<double,in>,:device_id<char[20],in>,:orientation<double,in>,:speed<double,in>,:power_switch_is_on<int,in>,:ignition_is_on<int,in>,:miles_data<double,in>)}",
				*db);

			o.set_commit(0);

			o << _dateTime
				<< device_feeback->dlat
				<< device_feeback->dlon
				<< device_feeback->deviceId
				<< device_feeback->dorientation
				<< device_feeback->dspeed
				<< (device_feeback->main_power_switch_on ? 1 : 0)
				<< (device_feeback->acc_ignition_on ? 1 : 0)
				<< (double)device_feeback->dmile_data;

			LOG_WARN << "Device feedback logged successfully!";
		}
		catch (otl_exception& p) {
			LOG_WARN << (char*)p.msg; // print out error message
			LOG_WARN << p.code; // print out error code
			LOG_WARN << p.var_info; // print out the variable that caused the error
			LOG_WARN << (char*)p.sqlstate; // print out SQLSTATE message
			LOG_WARN << p.stm_text; // print out SQL that caused the error
		}
	}
}



