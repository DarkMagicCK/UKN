//
//  RefCounted.h
//  Mist
//
//  Created by Robert Bu on 12/17/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTRefCounted_h
#define MISTRefCounted_h

#include "mist/Platform.h"
#include "mist/Uncopyable.h"

namespace mist {
    
    class MIST_API RefCounted: Uncopyable {
    public:
        RefCounted(): mCounter(1) { }
        
        void duplicate();
        void release();
        int32 getRefCount() const;
        
    protected:
        virtual ~RefCounted() { }
        
    private:
        mutable int32 mCounter;
    };
    
    inline int32 RefCounted::getRefCount() const {
        return mCounter;
    }
    
    inline void RefCounted::duplicate() {
        ++mCounter;
    }
    
    inline void RefCounted::release() {
        if(--mCounter == 0)
            delete this;
    }
    
} // namespace mist


#endif
