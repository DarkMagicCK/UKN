// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MIST_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MIST_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MIST_EXPORTS
#define MIST_API __declspec(dllexport)
#else
#define MIST_API __declspec(dllimport)
#endif

// This class is exported from the mist.dll
class MIST_API Cmist {
public:
	Cmist(void);
	// TODO: add your methods here.
};

extern MIST_API int nmist;

MIST_API int fnmist(void);

#include "mist/Platform.h"

