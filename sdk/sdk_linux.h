#pragma once
#include <memory>
#include "zmq.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>



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

	auto search_gps_devices() {
		auto  gpses = std::make_shared<std::vector<gps*>>();

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

				gpses->push_back(load(this));
				free(namelist[i]);
			}
		}

		free(namelist);
		return gpses;
	}

	void start_device_feedbacks_logs_job() {
		pthread_t td;
		
		otl_connect* db_addr = &this->db;
		std::tuple<zmq::context_t*, otl_connect*> *log_params = new std::tuple<zmq::context_t*, otl_connect*>(context, db_addr);

		int err = pthread_create(&td, NULL, [](void* params)->void* {

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

				strcpy(device_feeback->deviceId, root.get<std::string>("deviceId").c_str());

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

			return NULL;
		}, (void*)log_params);
		if (err != 0)
			printf("\ncan't create thread :[%s]", strerror(err));
		else
			printf("\n Thread created successfully\n");
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

	private:
		static __gps__* self;
		otl_connect* db;
		const char dir_path[6] = { "./gps" };
		zmq::socket_t* publisher;
		zmq::context_t* context;
		std::shared_ptr<std::vector<gps*>> vgpses;

		std::string document_db_username;
		std::string document_db_userpassword;
		std::string document_db_host;
		std::string document_db_database_name;
		int document_db_databse_port;

		std::string db_userpassword;
		std::string db_username;
		std::string db_url;

		int message_queue_port;
		std::string message_queue_host;
		std::string message_queue_username;
		std::string message_queue_password;



		web::http::client::http_client_config client_config_for_proxy();

		//Accessors
		std::string get_document_db_username();
		std::string get_document_db_userpassword();
		std::string get_document_db_host();
		std::string get_document_db_database_name();
		int get_document_db_database_port();
};

//Typedefs...
typedef class __gps__ GPS_HANDLERS, *LPGPS_HANDLERS;