// ukn_dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ukn_dll.h"


// This is an example of an exported variable
UKN_DLL_API int nukn_dll=0;

// This is an example of an exported function.
UKN_DLL_API int fnukn_dll(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see ukn_dll.h for the class definition
Cukn_dll::Cukn_dll()
{
	return;
}
