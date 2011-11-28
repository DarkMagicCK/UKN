//
//  DllLoader.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/DllLoader.h"

#ifndef UKN_OS_WINDOWS
#include <dlfcn.h>
#endif

namespace ukn {
    
#ifdef UKN_OS_WINDOWS
    
    class DllLoaderImpl {
    public:
        bool open(const char* path) {
            module = LoadLibraryA(path);
            if(module == NULL) {
                return false;
            }
            return true;
        }
        void close() {
            if(module) {
                FreeLibrary(module);
            }
            module = NULL;
        }
        void* getProc(const char* proc) {
            if(module) {
                return GetProcAddress(module, proc);
            }
            return NULL;
        }
        
    private:
        HMODULE module;
    };
    
#elif defined(UKN_OS_OSX) || defined(UKN_OS_LINUX) || defined(UKN_OS_IOS) 
    
    class DllLoaderImpl {
    public:
        bool open(const char* path) {
            dyn = dlopen(path, RTLD_LOCAL);
            if(!dyn) {
                return false;
            }
            return true;
        }
        void close() {
            if(dyn)
                dlclose(dyn);
            dyn = NULL;
        }
        void* getProc(const char* proc) {
            if(dyn) {
                return dlsym(dyn, proc);
            }
            return NULL;
        }
        
    private:
        void* dyn;
    };
    
#else
    
    // null impl
    
    class DllLoaderImpl {
    public:
        bool open(const char* path) { return false; }
        void close() {}
        void* getProc(const char* proc) { return NULL; }
    };
    
#endif
    
    DllLoader::DllLoader(): 
    mImpl(new DllLoaderImpl) {
        
    }
    
    DllLoader::~DllLoader() {
        if(mImpl) {
            mImpl->close();
            delete mImpl;
        }
    }
    
    bool DllLoader::open(const char* path) {
        return mImpl->open(path);
    }
    
    void DllLoader::close() {
        mImpl->close();
    }
    
    void* DllLoader::getProc(const char* name) {
        return mImpl->getProc(name);
    }
    
} // namespace ukn  
