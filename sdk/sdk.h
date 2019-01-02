#pragma once
#include "stdafx.h"
#include <memory>
#include <exception>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <process.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include "../include/cpprest/filestream.h"
#include "../include/cpprest/http_client.h"

#define OTL_ODBC
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE
#define OTL_ODBC_MYSQL
#define OTL_STREAM_POOLING_ON
#define OTL_STL
#define OTL_ODBC_MULTI_MODE

#include "zmq.hpp"
#include "gps.h"
#include "otlv4.h"


typedef int(*f_funci)();
typedef LPGPS(*f_load)(void *);

using namespace utility;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GLS_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GLS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GLS_EXPORTS
	#if defined(_MSC_VER)
		#define GLS_API __declspec(dllexport)
	#else
		#define GLS_API 
	#endif // defined(WINDOW) && (_MSC_VER)
	#else
	#if defined(_MSC_VER)
		#define GLS_API __declspec(dllimport)
	#else
		#define GLS_API 
	#endif // defined(WINDOW) && (_MSC_VER)
#endif


namespace geolocation_svc {

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
	struct datetime* _dateTime;
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

 class __gps__ {

	public:
		GLS_API ~__gps__();
		GLS_API __gps__();
		virtual void log_feedback(device_feedback* device_feeback);
		virtual void log_feedback(std::string device_feeback_str);
		virtual bool is_device_registered(const char* deviceId);

		GLS_API std::shared_ptr<std::vector<gps*>> search_gps_device_drivers();
		GLS_API void start_device_feedbacks_logs_job();


	private:
		static __gps__* self;
		otl_connect* db;
		const char dir_path[6] = { "./gps" };
		zmq::socket_t* publisher;
		zmq::context_t* context;
		std::shared_ptr<std::vector<gps*>> vgpses;

		std::string document_db_username;
		std::string document_db_userpassword;

		web::http::client::http_client_config client_config_for_proxy();
		std::string get_document_db_username();
		std::string get_document_db_userpassword();
	};

	//Typedefs...
	typedef class __gps__ GPS_HANDLERS, *LPGPS_HANDLERS;
}
