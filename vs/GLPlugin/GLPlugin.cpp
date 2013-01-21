// GLPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GLPlugin.h"


// This is an example of an exported variable
GLPLUGIN_API int nGLPlugin=0;

// This is an example of an exported function.
GLPLUGIN_API int fnGLPlugin(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see GLPlugin.h for the class definition
CGLPlugin::CGLPlugin()
{
	return;
}
