//
//  Common.h
//  Project Unknown
//
//  Created by Robert Bu on 11/21/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Common_h
#define Project_Unknown_Common_h

#include "Platform.h"
#include "Exception.h"

#ifdef UKN_OS_WINDOWS

#include <windows.h>

#else


#endif // UKN_OS_WINDOWS

namespace ukn {

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
    
    static ukn_string format_string(const char* format, ...) {
        va_list	arg;
		char message[512] = {0};
		va_start(arg, format);
		vsprintf(message, format, arg);
		va_end(arg);
		return ukn_string(message);
    }
    
    template<typename T, typename Base>
    static T checked_cast(Base* pointer) {
        T derived = dynamic_cast<T>(pointer);
        if(!derived)
            UKN_THROW_EXCEPTION("checked_cast: failed to cast from parent to child");
        return derived;
    }
    
    inline ukn_string get_lib_name(const ukn_string& lib) {
#if defined(UKN_OS_OSX)
        return lib + ".dylib";
#elif defined(UKN_OS_WINDOWS)
        return lib + ".dll";
#elif defined(UKN_OS_LINUX)
        return lib + ".so";
#endif
    }
    
#define UKN_DEF_SET_GET(type, var, name) \
    void set##name(type value) { \
        var = value; \
    } \
    type get##name() const { \
        return var; \
    }
    
} // namespace ukn


#endif
