#pragma once
#include "sdk.h"

using namespace utility;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

namespace geolocation_svc {

	__gps__* __gps__::self = NULL;

	__gps__::~__gps__() {
			db.logoff();
		}

	__gps__::__gps__() {

		    __gps__::self = this;

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

			try {
				//start connection to sql db
				otl_connect::otl_initialize(1);
				db.rlogon("DRIVER={MySQL};SERVER=127.0.0.1;PORT=3306;DATABASE=geolocation_service;USER=root;PASSWORD=;");
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
				//LOG_WARN << "Found GPS file @ " << gpsfile.c_str();

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

				gpses->push_back(load(this));
			} while (FindNextFile(search_handle, &file));
			FindClose(search_handle);
		}

		return gpses;
	}

	void __gps__::start_device_feedbacks_logs_job() {
		otl_connect* db_addr = &this->db;
		std::tuple<zmq::context_t*, otl_connect*> *log_params = new std::tuple<zmq::context_t*, otl_connect*>(context, db_addr);

		_beginthread([](void* params)->void {

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
				http_client client(U("http://127.0.0.1:8529/_db/geo_location/_api/document/logs/"));
				http_request request(methods::POST);
				request.headers().add(L"Content-type", L"application/json");
				request.headers().add(L"Autorization", utility::conversions::to_base64(__gps__::self->get_document_db_username + ":" +__gps__::self->get_document_db_userpassword));
				request.set_body(web::json::value(str.c_str()));

				auto response = client.request(request)
					.then([str](http_response response) {
					if (response.status_code != status_codes::Accepted &&
						response.status_code != status_codes::Created) {

						boost::property_tree::ptree root;
						std::istringstream is(str);
						read_json(is, root);

						device_feedback *device_feeback = (device_feedback *)malloc(sizeof(device_feedback));
						device_feeback->acc_ignition_on = root.get<double>("acc_ignition_on", 0);

						strcpy_s(device_feeback->deviceId, sizeof(device_feeback->deviceId), root.get<std::string>("deviceId").c_str());
											   
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

						__gps__::self->log_feedback(device_feeback);
					}
				});
			}
		}, 1024, (void*)log_params);
	}

	bool __gps__::is_device_registered(const char* deviceId) {
		http_client client(web::uri(utility::conversions::to_string_t( std::string(std::string("http://127.0.0.1:8529/_db/geo_location/_api/document/devices/") + std::string(deviceId)) )));
		http_request request(methods::GET);
		request.headers().add(L"Accept", L"application/json");
		request.headers().add(L"Autorization", utility::conversions::to_base64(__gps__::self->get_document_db_username + ":" + __gps__::self->get_document_db_userpassword));
		auto response = client.request(request);
		return response.get().status_code == status_codes::NotFound ? false : true;
	}

	/*
		Can pass proxy information via environment variable http_proxy.
		Example:
		Linux:   export http_proxy=http://192.1.8.1:8080
		*/
	web::http::client::http_client_config __gps__::client_config_for_proxy()
	{
		web::http::client::http_client_config client_config;
#ifdef _WIN32
		wchar_t* pValue = nullptr;
		std::unique_ptr<wchar_t, void(*)(wchar_t*)> holder(nullptr, [](wchar_t* p) { free(p); });
		size_t len = 0;
		auto err = _wdupenv_s(&pValue, &len, L"http_proxy");
		if (pValue) holder.reset(pValue);
		if (!err && pValue && len)
		{
			std::wstring env_http_proxy_string(pValue, len - 1);
#else
		if (const char* env_http_proxy = std::getenv("http_proxy"))
		{
			std::string env_http_proxy_string(env_http_proxy);
#endif
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
};