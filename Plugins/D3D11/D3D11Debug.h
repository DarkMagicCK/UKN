#ifndef Project_Unknown_D3D11Debug_h
#define Project_Unknown_D3D11Debug_h

#include <D3D11.h>
#include "mist/Platform.h"

namespace ukn {

	/* 
		message info queue debug message support when
		MIST_DEBUG flag is defined
		and
		device is created with D3D11_CREATE_DEVICE_DEBUG flag
	*/
	class D3D11Debug {
	public:
		D3D11Debug(ID3D11Device* device);
		~D3D11Debug();

		/* log all messages avaiable in the info queue to mist::Logger */
		void logAvailableMessages();

		/* check hresult return codes and log, return succeed or not */
		/* custom message is appended to the log message */
		static bool CHECK_RESULT(HRESULT result, const wchar_t* customMessage = 0);

		ID3D11InfoQueue* getInfoQueue() const;

	private:
        mist::int64 mLastLoggesIndex;
		ID3D11Device* mDevice;
		ID3D11InfoQueue* mInfoQueue;
	};

}

#endif