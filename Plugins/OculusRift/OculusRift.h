#ifdef OCULUSRIFT_EXPORTS
#define OCULUSRIFT_API __declspec(dllexport)
#else
#define OCULUSRIFT_API __declspec(dllimport)
#endif