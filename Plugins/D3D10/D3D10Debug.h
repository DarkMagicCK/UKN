#ifndef Project_Unknown_D3D10Debug_h
#define Project_Unknown_D3D10Debug_h

#include <d3d10.h>

namespace ukn {

	/* 
		message info queue debug message support when
		MIST_DEBUG flag is defined
		and
		device is created with D3D10_CREATE_DEVICE_DEBUG flag
	*/
	class D3D10Debug {
	public:
		D3D10Debug(ID3D10Device* device);
		~D3D10Debug();

		/* log all messages avaiable in the info queue to mist::Logger */
		void logAvailableMessages();

		/* check hresult return codes and log, return succeed or not */
		/* custom message is appended to the log message */
		static bool CHECK_RESULT(HRESULT result, const wchar_t* customMessage = 0);

		ID3D10InfoQueue* getInfoQueue() const;

	private:
		ID3D10Device* mDevice;
		ID3D10InfoQueue* mInfoQueue;
	};

}

#endif