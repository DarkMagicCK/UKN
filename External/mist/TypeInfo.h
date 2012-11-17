//
//  TypeInfo.h
//  Mist
//
//  Created by Robert Bu on 12/5/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTTypeInfo_h
#define MISTTypeInfo_h

#include "mist/Platform.h"

#include <map>

namespace mist {
    
    namespace reflection {
           
        class MIST_API RttiTypeInfo {
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
    
    
} // namespace mist

#endif
