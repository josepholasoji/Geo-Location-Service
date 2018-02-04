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
