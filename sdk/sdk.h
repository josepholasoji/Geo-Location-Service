#pragma once
#include <memory>
#include "zmq.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#ifndef _MSC_VER
	#include <dlfcn.h>
	#include <dirent.h>
	#include <pthread.h>
	#define OTL_ODBC_UNIX
#else

#endif 

#define UNICODE
#define OTL_ODBC
#define OTL_ODBC_SELECT_STM_EXECUTE_BEFORE_DESCRIBE
#define OTL_ODBC_MYSQL
#define OTL_STREAM_POOLING_ON
#define OTL_STL
#define OTL_ODBC_MULTI_MODE

#include "otlv4.h"
#include "NanoLog.hpp"

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

struct __gps__ {

	~__gps__() {
		db.logoff();
	}

	__gps__() {
		//start logger
		// Log will roll to the next file after every 1MB.
		nanolog::initialize(nanolog::GuaranteedLogger(), "logs/", "geolocation_service.log", 10);
		nanolog::set_log_level(nanolog::LogLevel::WARN);
		LOG_INFO << "GEO - LOCATION SERVICE - 1.0";

		//Zero MQ version
		int major, minor, patch;
		zmq_version(&major, &minor, &patch);
		LOG_INFO << "Current 0MQ version is" << major << "." << minor << "." << patch;

		//Start zero mq
		LOG_WARN << "Starting MQ on port 5555";
		context = new zmq::context_t(1);
		publisher = new zmq::socket_t(*context, ZMQ_PUB);
		publisher->setsockopt(ZMQ_LINGER, 0);
		publisher->bind("tcp://*:5555");
		LOG_WARN << "0MQ successfully started on port 5555";

		//start connection to sql db
		otl_connect::otl_initialize(1);
		db.rlogon("DRIVER={MySQL};SERVER=127.0.0.1;PORT=3306;DATABASE=geolocation_service;USER=root;PASSWORD=;");
		LOG_WARN << "Successfully logged on to DB @ " << "127.0.0.1 as " << "root";
	}

	void log_feedback(device_feedback* device_feeback) {

		//TODO:load default feedback store from the db, 
		//remove the static assignment below
		__data_store data_store_selection = __data_store::SQLDB;

		if (data_store_selection == __data_store::SQLDB) {
			boost::property_tree::ptree out;
			out.put("acc_ignition_on", device_feeback->acc_ignition_on);
			out.put("deviceId", device_feeback->deviceId);
			out.put("dlat", device_feeback->dlat);
			out.put("dlon", device_feeback->dlon);
			out.put("dorientation", device_feeback->dorientation);
			out.put("dspeed", device_feeback->dspeed);
			out.put("main_power_switch_on", device_feeback->main_power_switch_on);

			boost::property_tree::ptree out_date_and_time;
			out.put("_dateTime.day", device_feeback->_dateTime->day);
			out.put("_dateTime.hour", device_feeback->_dateTime->hour);
			out.put("_dateTime.minute", device_feeback->_dateTime->minute);
			out.put("_dateTime.month", device_feeback->_dateTime->month);
			out.put("_dateTime.second", device_feeback->_dateTime->second);
			out.put("_dateTime.year", device_feeback->_dateTime->year);

			out.put("message_type", "GPS_FEEDBACK_MEESAGE_SQL_COMMIT");

			std::ostringstream oss;
			boost::property_tree::write_json(oss, out);
			std::string jsonString = oss.str();

			//send a 0MQ message
			this->publisher->send(jsonString.c_str(), jsonString.size());
		}
		else if (data_store_selection == __data_store::MONGODB) {

		}
		else if (data_store_selection == __data_store::REDIS) {

		}
	}

	bool is_device_registered(const char* deviceId) {
		try {
			int is_registered = 0;
			otl_stream o(1, "{call find_device(:device_id<varchar[20],in>, @registered)}", this->db);
			o.set_commit(0);
			o << deviceId;

			//read all the outut...
			otl_stream s(1, "select @registered  :#1<int>", db, otl_implicit_select);
			s >> is_registered;
			return is_registered == 0;
		}
		catch (otl_exception& p) {
			LOG_WARN << (char*)p.msg; // print out error message
			LOG_WARN << p.code; // print out error code
			LOG_WARN << p.var_info; // print out the variable that caused the error
			LOG_WARN << (char*)p.sqlstate; // print out SQLSTATE message
			LOG_WARN << p.stm_text; // print out SQL that caused the error
		}

		return false;
	}

	otl_connect db;
	zmq::socket_t* publisher;
	zmq::context_t* context;
};

typedef struct __gps__ GPS_HANDLERS, *LPGPS_HANDLERS;