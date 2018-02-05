// tk103.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "tk103.h"


// This is an example of an exported variable
TK103_API int ntk103=0;

// This is an example of an exported function.
TK103_API int fntk103(void)
{
    return 42;
}

// This is the constructor of a class that has been exported.
// see tk103.h for the class definition
Ctk103::Ctk103()
{
    return;
}

gps * Ctk103::detect(unsigned char*, int len)
{
	read();
	process();
	write();
}

void Ctk103::start()
{
	started = true;
	while (started)
	{
		read();
		process();
		write();
	}

	//finalize process stopping here...
}

void Ctk103::stop()
{
	started = false;
}

std::string Ctk103::status()
{
	switch (istatus)
	{
	default:
		return "Unknown status";
	}
}

int Ctk103::read()
{
	return 0;
}

int Ctk103::write()
{
	return 0;
}

int Ctk103::process()
{
	return 0;
}
