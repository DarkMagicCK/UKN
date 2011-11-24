//
//  ScopedPtr.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_ScopedPtr_h
#define Project_Unknown_ScopedPtr_h

#include "Platform.h"

namespace ukn {
    
    template<class T>
    class ScopedPtr: SoraUncopyable {
    public:
        explicit ScopedPtr(T* _ptr):
        ptr(_ptr) {
        }
        
        ScopedPtr():
        ptr(0) {
            
        }
        
        ~ScopedPtr() {
            if(this->ptr) {
                delete this->ptr;
                this->ptr = 0;
            }
        }
        
        void reset(T* p = 0) {
            this->ptr = p;
        }
        
        T& operator*() const {
            ukn_assert(this->ptr != 0);
            return *this->ptr;
        }
        T* operator->() const {
            return this->ptr;
        }
        
        T* get() const {
            return this->ptr;
        }
        
        void swap(ScopedPtr& b);
        
    private:
        T* ptr;
    };
    
    template<class T>
    void swap(ScopedPtr<T>& a, ScopedPtr<T>& b) {
        std::swap(a.ptr, b.ptr);
    }
    
} // namespace ukn

#endif
