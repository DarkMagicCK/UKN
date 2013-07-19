// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the UKN_DLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// UKN_DLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef UKN_DLL_EXPORTS
#define UKN_DLL_API __declspec(dllexport)
#else
#define UKN_DLL_API __declspec(dllimport)
#endif

// This class is exported from the ukn_dll.dll
class UKN_DLL_API Cukn_dll {
public:
	Cukn_dll(void);
	// TODO: add your methods here.
};

extern UKN_DLL_API int nukn_dll;

UKN_DLL_API int fnukn_dll(void);

#pragma comment(lib, "mist.lib")

