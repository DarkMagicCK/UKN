//
//  Common.h
//  Mist
//
//  Created by Robert Bu on 11/21/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTCommon_h
#define MISTCommon_h

#include "mist/Platform.h"

#include <cstdarg>
#include <cstdio>

namespace mist {

    // for_each(STL_CONT.begin(), STL_CONT.end(), DeleteSTLPtr());
	// for_each(STL_MAP.begin(), STL_MAP.end(), DeleteSTLMapPtr());

    struct DeleteSTLPtr {
		template<typename T>
		void operator() (const T* ptr) const {
			if(ptr) {
				delete ptr;
				ptr = 0;
			}
		}
	};

	struct DeleteSTLPairPtr {
		template<typename Ty1, typename Ty2>
		void operator() (const std::pair<Ty1, Ty2>& ptr) const {
			if(ptr.second) {
				delete ptr.second;
			}
		}
	};

    static std::string format_string(const char* format, ...) {
        va_list	arg;
		char message[1024] = {0};
		va_start(arg, format);
		vsprintf(message, format, arg);
		va_end(arg);
		return std::string(message);
    }

    inline MistString get_lib_name(const MistString& lib) {
        if(lib.find(L".") == MistString::npos) {
#if defined(MIST_OS_OSX)
        return lib + L".dylib";
#elif defined(MIST_OS_WINDOWS)
        return lib + L".dll";
#elif defined(MIST_OS_LINUX)
        return lib + L".so";
#endif
        }
        return lib;
    }


} // namespace mist


#endif
