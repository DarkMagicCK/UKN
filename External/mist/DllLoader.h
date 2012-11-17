//
//  DllLoader.h
//  Mist
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTDllLoader_h
#define MISTDllLoader_h

#include "mist/Platform.h"

namespace mist {
    
    class MIST_API DllLoader {
    public:
        DllLoader();
        ~DllLoader();
        
        bool open(const char* path);
        void close();
        
        void* getProc(const char* name);

    private:
        class DllLoaderImpl;

        DllLoaderImpl* mImpl;
    };
    
} // namespace mist


#endif
