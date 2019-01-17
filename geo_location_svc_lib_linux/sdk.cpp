#pragma once

#include "../sdk/sdk.h"
#include "../sdk/NanoLog.hpp"
#include "yaml-cpp/yaml.h"
#include <boost/locale.hpp>

using namespace std;
using namespace utility;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

namespace geolocation_svc {

	__gps__* __gps__::self = NULL;

	__gps__::~__gps__() {
			if (db != NULL && db->connected)
			{
				 db->logoff();
			}			
		}

	__gps__::__gps__() {

		    __gps__::self = this;
			db = NULL;

			//Load configurations
			try {
				YAML::Node config = YAML::LoadFile("config/application.properties");
				document_db_username = config["db.document.datasource.username"].as<std::string>();
				document_db_userpassword = config["db.document.datasource.password"].as<std::string>();
				document_db_host = config["db.document.datasource.host"].as<std::string>();
				document_db_database_name = config["db.document.datasource.db"].as<std::string>();
				document_db_databse_port = config["db.document.datasource.port"].as<int>();

				db_userpassword = config["db.datasource.password"].as<std::string>();
				db_username = config["db.datasource.username"].as<std::string>();
				db_url = config["db.datasource.url"].as<std::string>();

				message_queue_port = config["message.queue.port"].as<int>();
				message_queue_host = config["message.queue.host"].as<std::string>();
				message_queue_username = config["message.queue.username"].as<std::string>();
				message_queue_password = config["message.queue.password"].as<std::string>();
			}
			catch (YAML::Exception yml_exception) {
				printf(std::string("Startup error, Error loading config file: " +  yml_exception.msg).c_str());
				return;
			}


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
			LOG_WARN << "Starting MQ on port " << message_queue_host << ":" << message_queue_port;
			context = new zmq::context_t(1);
			publisher = new zmq::socket_t(*context, ZMQ_PUB);
			publisher->setsockopt(ZMQ_LINGER, 0);

			std::stringstream bind_fmt;
			bind_fmt << "tcp://" << message_queue_host <<":" << message_queue_port;
			publisher->bind(bind_fmt.str());
			LOG_WARN << "0MQ successfully started on port " << message_queue_host << ":" << message_queue_port;

			try {
				//start connection to sql db
				otl_connect::otl_initialize(1);
				db = new otl_connect();
				db->rlogon(db_url.c_str());
				LOG_WARN << "Successfully logged on to DB @ " << "127.0.0.1 as " << "root";
			}
			catch (otl_exception& p)
			{
				LOG_WARN << (char*)p.msg; // print out error message
				LOG_WARN << p.code; // print out error code
				LOG_WARN << p.var_info; // print out the variable that caused the error
				LOG_WARN << (char*)p.sqlstate; // print out SQLSTATE message
				LOG_WARN << p.stm_text; // print out SQL that caused the error
			}
			catch (std::exception ex) {
				LOG_WARN << ex.what();
			}
		}

	void __gps__::log_feedback(device_feedback* device_feeback) {

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

			//int year =  std::atoi(std::string((device_feeback->_dateTime->year < 10 ? "200" : "20") + std::to_string(device_feeback->_dateTime->year)).c_str()); std::atoi(std::string((device_feeback->_dateTime->year < 10 ? "200" : "20") + std::to_string(device_feeback->_dateTime->year)).c_str());
			std::ostringstream year;
			year << "2" << std::setw(3) << std::setfill('0') << device_feeback->_dateTime->year;
			out.put("_dateTime.year", year.str());

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
	void __gps__::log_feedback(std::string device_feeback_str) {		
		this->publisher->send(device_feeback_str.c_str(), device_feeback_str.size());
	}

	std::shared_ptr<std::vector<gps*>> __gps__::search_gps_device_drivers() {
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
					std::cerr << "Cannot open library: " << dlerror() << '\n';
					LOG_WARN << "Cannot open library: " << dlerror() << '\n';
					return gpses;
				}

				// reset errors
				dlerror();
				LOG_WARN << "Loading GPS file: " << gpsfile;
				f_load load = (f_load)dlsym(handle, "load");
				const char *dlsym_error = dlerror();
				if (dlsym_error) {
					std::cerr << "Cannot load symbol 'load' from " << namelist[i]->d_name << "; " << dlsym_error << '\n';
					LOG_WARN << "Cannot load symbol 'load' from " << namelist[i]->d_name << "; " << dlsym_error << '\n';

					dlclose(handle);
					return gpses;
				}

				gpses->push_back(load(this));
				free(namelist[i]);
			}
		}

		free(namelist);
		return gpses;
	}

	void __gps__::start_device_feedbacks_logs_job() {
		otl_connect* db_addr = this->db;
		std::tuple<zmq::context_t*, otl_connect*> *log_params = new std::tuple<zmq::context_t*, otl_connect*>(context, db_addr);

		boost::thread t{ [](void* params) {

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

				std::string str(ddata, update.size());
				boost::trim(str);

				LOG_WARN << "Recieved data for logging: " << str;

				// Create an HTTP request.
				// Encode the URI query since it could contain special characters like spaces.
				std::stringstream URL_fmt;
				URL_fmt << "http://" << __gps__::self->get_document_db_host() << ":" << __gps__::self->get_document_db_database_port() << "/_db/" << __gps__::self->get_document_db_database_name() << "/_api/document/logs";

				http_client client(utility::conversions::to_string_t(URL_fmt.str()));
				http_request request(methods::POST);
				request.headers().add("Content-type", "application/json");
				request.headers().add("Authorization", __gps__::self->get_basic_auth_data());
				request.set_body(str);

				auto response = client.request(request)
					.then([str](http_response response) {
					if (response.status_code() != status_codes::Accepted && response.status_code() != status_codes::Created) {
						__gps__::self->log_feedback(str);
					}
				});
			}
		}, (void*)log_params };
	}

	bool __gps__::is_device_registered(const char* deviceId) {
		http_client client(web::uri(utility::conversions::to_string_t( std::string(std::string("http://127.0.0.1:8529/_db/geo_location/_api/document/devices/") + std::string(deviceId)) )));
		http_request request(methods::GET);
		request.headers().add("Accept", "application/json");
		request.headers().add("Authorization", this->get_basic_auth_data());
		
		auto response = client.request(request);
		auto status_code = response.get().status_code();
		return status_code == status_codes::OK ? true : false;
	}

	/*
		Can pass proxy information via environment variable http_proxy.
		Example:
		Linux:   export http_proxy=http://192.1.8.1:8080
		*/
	web::http::client::http_client_config __gps__::client_config_for_proxy()
	{
		web::http::client::http_client_config client_config;

		if (const char* env_http_proxy = std::getenv("http_proxy"))
		{
			std::string env_http_proxy_string(env_http_proxy);

			if (env_http_proxy_string == U("auto"))
				client_config.set_proxy(web::web_proxy::use_auto_discovery);
			else
				client_config.set_proxy(web::web_proxy(env_http_proxy_string));
		}

		return client_config;
	}

	std::string __gps__::get_document_db_username() {
		return this->document_db_username;
	}

	std::string __gps__::get_document_db_userpassword() {
		return this->document_db_userpassword;
	}

	std::string __gps__::get_document_db_host() {
		return this->document_db_host;
	}

	std::string __gps__::get_document_db_database_name() {
		return this->document_db_database_name;
	}

	int __gps__::get_document_db_database_port() {
		return this->document_db_databse_port;
	}

	std::string __gps__::get_basic_auth_data() {
		if (this->basic_auth_data.empty()) {
			std::string authData = std::string(this->get_document_db_username() + ":" + this->get_document_db_userpassword());
			this->basic_auth_data = std::string("Basic ") + utility::conversions::to_base64(std::vector<unsigned char>(authData.begin(), authData.end()));
		}

		return this->basic_auth_data;
	}
};