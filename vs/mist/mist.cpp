// mist.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "mist.h"


// This is an example of an exported variable
MIST_API int nmist=0;

// This is an example of an exported function.
MIST_API int fnmist(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see mist.h for the class definition
Cmist::Cmist()
{
	return;
}
