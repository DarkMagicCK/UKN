//
//  Resource.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Resource_h
#define Project_Unknown_Resource_h

#include "Platform.h"

namespace ukn {
    
    class UKN_API Resource {
    public:
        Resource() {
            // todo uniqueid
        }
        
        Resource(ukn_wstring& name):
        mName(name) {
            
        }
        
        virtual ~Resource() { }
        
        const ukn_wstring& getName() const {
            return mName;
        }
        
        uint32 getUniqueId() const {
            return mUniqueId;
        }
        
        virtual uint32 getTypeId() const = 0;
        
    private:
        ukn_wstring mName;
        
        // unique id for the resource
        uint32 mUniqueId;
    };
    
    
} // namespace ukn

#endif
