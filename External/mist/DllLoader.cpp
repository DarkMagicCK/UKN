//
//  DllLoader.cpp
//  Mist
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/DllLoader.h"

#ifndef MIST_OS_WINDOWS
#include <dlfcn.h>
#else
#include <windows.h>
#endif

namespace mist {
    
#ifdef MIST_OS_WINDOWS
    
    class DllLoader::DllLoaderImpl {
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
    
#elif defined(MIST_OS_OSX) || defined(MIST_OS_LINUX) || defined(MIST_OS_IOS) 
    
    class DllLoader::DllLoaderImpl {
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
    
    class DllLoader::DllLoaderImpl {
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
    
} // namespace mist  
