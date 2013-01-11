//
//  Uncopyable.h
//  Mist
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTUncopyable_h
#define MISTUncopyable_h

#include "Platform.h"

namespace mist {
    
    class MIST_API Uncopyable {
    public:
        Uncopyable() { }
		virtual ~Uncopyable() { }
        
    protected:
		Uncopyable(const Uncopyable&) {}
		Uncopyable& operator=(const Uncopyable&) { return *this; }
    };
    
} // namespace mist

#endif
