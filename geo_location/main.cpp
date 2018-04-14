#include <zmq.h>
#include <zmq_utils.h>
#include <boost/asio.hpp>
#include "server.h"

typedef int(__stdcall *f_funci)();
typedef gps*(__stdcall *f_load)();

int main()
{
	//
	char dir_path[] = "./gps";

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
	return 0;
}
