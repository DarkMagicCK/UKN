//
//  TypeInfo.h
//  Project Unknown
//
//  Created by Robert Bu on 12/5/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_TypeInfo_h
#define Project_Unknown_TypeInfo_h

#include "UKN/Platform.h"

#include <map>

namespace ukn {
    
    namespace reflection {
           
        class UKN_API RttiTypeInfo {
        public:
            explicit RttiTypeInfo(const std::type_info& info) : _typeInfo(info) { };
            
            bool operator < (const RttiTypeInfo& rhs) const {
                return _typeInfo.before(rhs._typeInfo) != 0;
            }
            
            const char* name() const {
                return _typeInfo.name();
            }
            
        private:
            const std::type_info& _typeInfo;
        };
        
        
    }
    
    
} // namespace ukn

#endif
