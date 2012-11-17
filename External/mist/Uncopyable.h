//
//  Uncopyable.h
//  Mist
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTUncopyable_h
#define MISTUncopyable_h

namespace mist {
    
    class Uncopyable {
    public:
        Uncopyable() { }
        
    protected:
        Uncopyable(const Uncopyable&);
        Uncopyable& operator=(const Uncopyable&);
    };
    
} // namespace mist

#endif
