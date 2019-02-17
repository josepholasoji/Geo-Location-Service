#pragma once
#include "stdafx.h"
#include "sdk.h"
#include "NanoLog.hpp"
#include "yaml-cpp\yaml.h"
#include <boost\locale.hpp>

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

		//Search the plugins directory for service plugins
		WIN32_FIND_DATA file = { 0 };
		char path[MAX_PATH] = { 0 };
		GetCurrentDirectory(MAX_PATH, (LPWSTR)path);

		LOG_WARN << "Searching for GPS files...";

		HANDLE search_handle = FindFirstFile(L"services\\*.gps", &file);
		if (search_handle) {
			do {
				//load each dynamically...
				auto gpsfile = std::wstring(L"services\\").append(file.cFileName);
				LOG_WARN << "Found GPS file @ " << boost::locale::conv::utf_to_utf<char>(gpsfile);

				HINSTANCE hGetProcIDDLL = LoadLibrary((LPCWSTR)gpsfile.c_str());

				if (!hGetProcIDDLL) {
					std::cout << "could not load the GPS service file" << std::endl;
					LOG_WARN << "could not load the GPS service file";
					return gpses;
				}

				// resolve function address here
				//LOG_WARN << "Loading GPS file: " << gpsfile;
				f_load load = (f_load)GetProcAddress(hGetProcIDDLL, "load");
				if (!load) {
					std::cout << "could not locate the function" << std::endl;
					LOG_WARN << "could not locate the function";
					return gpses;
				}

				std::stringstream gps_config_file;
				gps_config_file << "config/application." << file.cFileName << ".properties";
				if (Utils::file_exists(gps_config_file.str())) {
					
					auto gps = load(this);

					YAML::Node config = YAML::LoadFile(gps_config_file.str());

					std::stringstream gps_config;
					gps_config << "service.configure.device.port";
					if (config[gps_config.str()].IsDefined) {
						gps->config(gps_config.str().c_str(), config[gps_config.str()].as<std::string>().c_str());
					}
					else {
						LOG_WARN << "Error! no service port defined for " << gps->deviceName() << "! device will be skipped from startup.";
						break;
					}

					std::stringstream gps_config;
					gps_config << "service.configure.admin.port";
					if (config[gps_config.str()].IsDefined) {
						gps->config(gps_config.str().c_str(), config[gps_config.str()].as<std::string>().c_str());
					}

					gps_config.clear();
					gps_config << "service.configure.device.messageIdIsDeviceId";
					if (config[gps_config.str()].IsDefined) {
						gps->config(gps_config.str().c_str(), config[gps_config.str()].as<std::string>().c_str());
					}

					gps_config.clear();
					gps_config << "service.configure.device.feedback.isochronousMessageInterval";
					if (config[gps_config.str()].IsDefined) {
						gps->config(gps_config.str().c_str(), config[gps_config.str()].as<std::string>().c_str());
					}

					gps_config.clear();
					gps_config << "service.configure.device.feedback.isometryMessageInterval";
					if (config[gps_config.str()].IsDefined) {
						gps->config(gps_config.str().c_str(), config[gps_config.str()].as<std::string>().c_str());
					}

					gps_config.clear();
					gps_config << "service.configure.device.tracking.upperSpeedLimit";
					if (config[gps_config.str()].IsDefined) {
						gps->config(gps_config.str().c_str(), config[gps_config.str()].as<std::string>().c_str());
					}

					gps_config.clear();
					gps_config << "service.configure.device.tracking.lowerSpeedLimit";
					if (config[gps_config.str()].IsDefined) {
						gps->config(gps_config.str().c_str(), config[gps_config.str()].as<std::string>().c_str());
					}

					gps_config.clear();
					gps_config << "service.configure.device.apn";
					if (config[gps_config.str()].IsDefined) {
						gps->config(gps_config.str().c_str(), config[gps_config.str()].as<std::string>().c_str());
					}

					gps_config.clear();
					gps_config << "service.configure.device.onekey";
					if (config[gps_config.str()].IsDefined) {
						gps->config(gps_config.str().c_str(), config[gps_config.str()].as<std::string>().c_str());
					}

					gps_config.clear();
					gps_config << "service.configure.device.pushAlarmtopic";
					if (config[gps_config.str()].IsDefined) {
						gps->config(gps_config.str().c_str(), config[gps_config.str()].as<std::string>().c_str());
					}

					gpses->push_back(gps);
				}
				else {
					LOG_WARN << "Error! no config file found for " << gps->deviceName() << "! device will be skipped from startup.";
				}

			} while (FindNextFile(search_handle, &file));
			FindClose(search_handle);
		}

		return gpses;
	}

	void __gps__::start_device_feedbacks_logs_job() {
		otl_connect* db_addr = this->db;
		std::tuple<zmq::context_t*, otl_connect*> *log_params = new std::tuple<zmq::context_t*, otl_connect*>(context, db_addr);

		_beginthread([](void* params)->void {

			LOG_WARN << "Starting the device feedback logger...";
			std::tuple<zmq::context_t*, otl_connect*>* tp = (std::tuple<zmq::context_t*, otl_connect*>*)params;
			zmq::context_t* zmq_context = std::get<0>(*tp);
			otl_connect* db = (otl_connect*)std::get<1>(*tp);

			LOG_WARN << "Starting the device feedback logger Queue...";


			//connect the subscriber
			std::stringstream bind_fmt;
			bind_fmt << "tcp://" << __gps__::self->message_queue_host << ":" << __gps__::self->message_queue_port;
			zmq::socket_t* subscriber = new zmq::socket_t(*zmq_context, ZMQ_SUB);
			subscriber->setsockopt(ZMQ_LINGER, 0);
			subscriber->connect(bind_fmt.str());

			//add subscribtion filter for feeback messages only
			const char *filter = "";
			subscriber->setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
			LOG_WARN << "Device feedback logger queue started on" << __gps__::self->message_queue_host << ":" << __gps__::self->message_queue_port;

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
				URL_fmt << "http://" <<  __gps__::self->get_document_db_host() << ":" << __gps__::self->get_document_db_database_port() << "/_db/" << __gps__::self->get_document_db_database_name() <<"/_api/document/logs";

				http_client client(utility::conversions::to_string_t(URL_fmt.str()));
				http_request request(methods::POST);
				request.headers().add(L"Content-type", L"application/json");
				request.headers().add(L"Authorization", __gps__::self->get_basic_auth_data());
				request.set_body(str);

				auto response = client.request(request)
					.then([str](http_response response) {
					if (response.status_code() != status_codes::Accepted && response.status_code() != status_codes::Created) {
						__gps__::self->log_feedback(str);
					}
				});
			}
		}, 1024, (void*)log_params);
	}

	const char* __gps__::is_device_registered(const char* deviceId) {
		http_client client(web::uri(utility::conversions::to_string_t( std::string(std::string("http://127.0.0.1:8529/_db/geo_location/_api/document/devices/") + std::string(deviceId)) )));
		http_request request(methods::GET);
		request.headers().add(L"Accept", L"application/json");
		request.headers().add(L"Authorization", this->get_basic_auth_data());
		
		auto response = client.request(request).get();
		if (response.status_code() == status_codes::OK) {
			std::string device_configuration_details = response.extract_utf8string().get();
			return device_configuration_details.c_str();//Return the device configuration details
		}

		return NULL;
	}

	/*
		Can pass proxy information via environment variable http_proxy.
		Example:
		Linux:   export http_proxy=http://192.1.8.1:8080
		*/
	web::http::client::http_client_config __gps__::client_config_for_proxy()
	{
		web::http::client::http_client_config client_config;

		wchar_t* pValue = nullptr;
		std::unique_ptr<wchar_t, void(*)(wchar_t*)> holder(nullptr, [](wchar_t* p) { free(p); });
		size_t len = 0;
		auto err = _wdupenv_s(&pValue, &len, L"http_proxy");
		if (pValue) holder.reset(pValue);
		if (!err && pValue && len)
		{
			std::wstring env_http_proxy_string(pValue, len - 1);
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

	std::wstring __gps__::get_basic_auth_data() {
		if (this->basic_auth_data.empty()) {
			std::string authData = std::string(this->get_document_db_username() + ":" + this->get_document_db_userpassword());
			this->basic_auth_data = std::wstring(L"Basic ") + utility::conversions::to_base64(std::vector<unsigned char>(authData.begin(), authData.end()));
		}

		return this->basic_auth_data;
	}
};