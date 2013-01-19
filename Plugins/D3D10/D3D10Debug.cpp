#include "D3D10Debug.h"

#include "mist/Logger.h"
#include "mist/StringUtil.h"

#include <D3DX10.h>

namespace ukn {

	using namespace mist;

	D3D10Debug::D3D10Debug(ID3D10Device* device):
	mDevice(device),
	mInfoQueue(0) {
		if(!D3D10Debug::CHECK_RESULT(mDevice->QueryInterface(__uuidof(ID3D10InfoQueue), (void**)&mInfoQueue)))
			mInfoQueue = 0;
	}

	D3D10Debug::~D3D10Debug() {
		if(mInfoQueue)
			mInfoQueue->Release();
	}

	bool D3D10Debug::CHECK_RESULT(HRESULT result, const wchar_t* customMessage) {
		if(SUCCEEDED(result))
			return true;

		if(!customMessage)
			customMessage = L"";

		switch(result) {
		case D3D10_ERROR_FILE_NOT_FOUND:
			log_error(MistString(L"D3D error: file not found ") + customMessage);
			break;
		
		case D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
			log_error(MistString(L"D3D error: too many unique state objects ") + customMessage);
			break;

		case D3DERR_INVALIDCALL:
			log_error(MistString(L"D3D error: invalid call ") + customMessage);
			break;

		case D3DERR_WASSTILLDRAWING:
			log_error(MistString(L"D3D error: was still drawwing ") + customMessage);
			break;

		case E_FAIL:
			log_error(MistString(L"D3D error: fail ") + customMessage);
			break;

		case E_INVALIDARG:
			log_error(MistString(L"D3D error: invalid arg ") + customMessage);
			break;

		case E_OUTOFMEMORY:
			log_error(MistString(L"D3D error: out of memory ") + customMessage);
			break;

		}
		return false;
	}

	void D3D10Debug::logAvailableMessages() {
		if(mInfoQueue &&
			mInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter() > 0) {
				/* log from back to front */
			for(mist::int64 i = mInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter() - 1; i >= 0 ; --i) {
				SIZE_T messageLength = 0;
				if(SUCCEEDED(mInfoQueue->GetMessage(i, NULL, &messageLength))) {
					D3D10_MESSAGE* message = mist_malloc_t(D3D10_MESSAGE, messageLength);
					if(message &&
						SUCCEEDED(mInfoQueue->GetMessage(i, message, &messageLength))) {
							switch(message->Severity) {
							case D3D10_MESSAGE_SEVERITY_CORRUPTION:
							case D3D10_MESSAGE_SEVERITY_ERROR:
								log_error(std::string(message->pDescription, message->pDescription + message->DescriptionByteLength));
								break;
							case D3D10_MESSAGE_SEVERITY_INFO:
								log_info(std::string(message->pDescription, message->pDescription + message->DescriptionByteLength));
								break;
							case D3D10_MESSAGE_SEVERITY_WARNING:
								log_warning(std::string(message->pDescription, message->pDescription + message->DescriptionByteLength));
								break;
							}
					}
				}
			}
		}
	}

	ID3D10InfoQueue* D3D10Debug::getInfoQueue() const {
		return mInfoQueue;
	}

}