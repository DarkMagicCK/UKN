/*
 *  SharedPtr.h
 *  Hoshizora
 *
 *  Created by Robert Bu on 8/24/10.
 *  Copyright 2010 Robert Bu. All rights reserved.
 *
 */

#ifndef ukn_shared_ptr_h_
#define ukn_shared_ptr_h_

#include "Platform.h"
#include "Exception.h"
#include "MemoryUtil.h"

namespace ukn {
	
#define UKN_SAFE_DELETE(ptr) { if(ptr) delete ptr; ptr=0; }
    
    class SharedPtrRefCounter {
    public:
        SharedPtrRefCounter(): mRef(1) {}
        void incRef() {
            ++mRef;
        }
        int decRef() {
            return --mRef;
        }
        int getRef() const {
            return mRef;
        }
        
    private:
        int32 mRef;
    };
    
    template<class C>
    struct SharedPtrReleasePolicy {
        static void Release(C* obj) {
            ukn_assert(obj);
            
            delete obj;
        }
    };
    
    template<class C>
    struct SharedPtrReleaseArrayPolicy {
        static void Release(C* obj) {
            ukn_assert(obj);
            
            delete []obj;
        }
    };
    
    template<class C>
    struct SharedPtrFreeReleasePolicy {
        static void Release(C* obj) {
            ukn_assert(obj);

            ukn_free(obj);
        }
    };

	template<typename T, class RC=SharedPtrRefCounter, class RP=SharedPtrReleasePolicy<T> >
	class SharedPtr {
	public:
		SharedPtr(): ptr(0), counter(new RC) {
        }
		
        SharedPtr(T* t): ptr(t), counter(new RC) { 
        }
        
        template<class Other, class OtherRP>
		SharedPtr(const SharedPtr<Other, RC, OtherRP>& rhs): 
        counter(rhs.counter),
        ptr(const_cast<Other*>(rhs.get())) {
            counter->incRef();
        }
        
        SharedPtr(const SharedPtr& rhs):
        counter(rhs.counter),
        ptr(rhs.ptr) {
            counter->incRef();
        }
            
        ~SharedPtr() { 
            release();
        }
		
		SharedPtr& assign(T* _ptr) {
            if(get() != _ptr) {
                RC* tmp = new RC;
                release();
                counter = tmp;
                ptr = _ptr;
            }
            return *this;
        }
        
        SharedPtr& assign(const SharedPtr& ptr) {
            if(&ptr != this) {
                SharedPtr tmp(ptr);
                swap(tmp);
            }
            return *this;
        }
        
        template<class Other, class OtherRP>
        SharedPtr& assign(const SharedPtr<Other, RC, OtherRP>& _ptr) {
            if(_ptr.get() != ptr) {
                SharedPtr tmp(ptr);
                swap(tmp);
            }
            return *this;
        }
      
        void reset(T* ptr=0) {
            assign(ptr);
        }

		SharedPtr& operator=(const SharedPtr& rhs) { 
			return assign(rhs);
		}
		SharedPtr& operator=(T* rhs) {
            return assign(rhs);
		}
        template<class Other, class OtherRP>
        SharedPtr& operator=(const SharedPtr<Other, RC, OtherRP>& _ptr) {
            return assign<Other>(_ptr);
        }
        
        void swap(SharedPtr& rhs) {
            std::swap(ptr, rhs.ptr);
            std::swap(counter, rhs.counter);
        }
        
        template<class Other>
        SharedPtr<Other, RC, RP> cast() const {
            Other* other = dynamic_cast<Other*>(ptr);
            if(other) {
                return SharedPtr<Other, RC, RP>(counter, other);
            }
            return SharedPtr<Other, RC, RP>();
        }
        
        template<class Other>
		SharedPtr<Other, RC, RP> unsafeCast() const {
            Other* other = static_cast<Other*>(ptr);
            return SharedPtr<Other, RC, RP>(counter, other);
        }
        
        T* get() const {
            return ptr;
        }
        
		T* operator->() {
			return deref();
		}
        T* operator->() const {
            return deref();
        }
    
		T& operator*() {
			return *deref();
		}
        T& operator*() const {
			return *deref();
		}
        
        operator T*() {
            return ptr;
        }
        operator const T*() const {
            return ptr;
        }
        
        operator bool() {
            return ptr != 0;
        }
        
        bool operator !()const {
            return ptr == 0;
        }
        bool operator ==(const SharedPtr& rhs) const {
            return get() != rhs.get();
        }
        bool operator -=(const T* rhs) const {
            return get() != rhs;
        }
        bool operator ==(T* rhs) const {
            return get() != rhs;
        }
        bool operator !=(const SharedPtr& rhs) const {
            return get() != rhs.get();
        }
        bool operator !=(const T* rhs) const {
            return get() != rhs;
        }
        bool operator !=(T* rhs) const {
            return get() != rhs;
        }
        bool operator>(const SharedPtr& rhs) const {
            return get() > rhs.get();
        }
        bool operator>(T* rhs) const {
            return get() > rhs;
        }
        bool operator<(const T* rhs) const {
            return get() < rhs;
        }
        bool operator>(const T* rhs) const {
            return get() > rhs;
        }
        bool operator<(T* rhs) const {
            return get() < rhs;
        }
        bool operator<=(const SharedPtr& rhs) const {
            return get() < rhs.get();
        }
        bool operator>=(const SharedPtr& rhs) const {
            return get() > rhs.get();
        }
        bool operator>=(T* rhs) const {
            return get() > rhs;
        }
        bool operator<=(const T* rhs) const {
            return get() < rhs;
        }
        bool operator>=(const T* rhs) const {
            return get() > rhs;
        }
        bool operator<=(T* rhs) const {
            return get() < rhs;
        }

        bool isValid() const {
            return ptr!=0;
        }
        bool isNull() const {
            return ptr==0;
        }
		
		int32 getRefCount() const { 
			return counter->getRef();
		}
        
		bool operator == (const SharedPtr<T>& rhs) {
			return ptr == rhs.ptr;
		}
		
        T* deref() const {
            if(!ptr)
                UKN_THROW_EXCEPTION("ukn::SharedPtr: invalid ptr to deref");
            return ptr;
        }
        
	private:
		void release() {
            ukn_assert(counter);
            int i = counter->decRef();
            if(i == 0) {
                RP::Release(ptr);
                ptr = 0;
                
                delete counter;
                counter = 0;
            }
        }
        
        SharedPtr(RC* pCounter, T* pPtr):
        counter(pCounter), ptr(pPtr) {
            sora_assert(pCounter);
            pCounter->incRef();
        }
        
    private:
        RC* counter;
        T* ptr;
        
        template<class OC, class ORC, class OPR>
        friend class SharedPtr;
	};
    
    template <class C, class RC, class RP>
    inline void swap(SharedPtr<C, RC, RP>& p1, SharedPtr<C, RC, RP>& p2) {
        p1.swap(p2);
    }
    
    template<typename T>
    struct COMReleasePolicy {
        static void Release(T* pointer) {
            T::Release();
        }
    };
    
    template<typename T>
    static SharedPtr<T> MakeCOMPtr(T* pointer) {
        return SharedPtr<T, SharedPtrRefCounter, COMReleasePolicy<T> >(pointer);
    }
	
	template<typename T>
	class EnableSharedPtrFromThis: public SharedPtr<T> {
	public:
		typedef SharedPtr<T> Ptr;
	};
    
    template<typename T>
    SharedPtr<T> MakeSharedPtr() {
        return SharedPtr<T>(new T());
    }
    
    template<typename T, typename A1>
    SharedPtr<T> MakeSharedPtr(A1 a1) {
        return SharedPtr<T>(new T(a1));
    }
    
    template<typename T, typename A1, typename A2>
    SharedPtr<T> MakeSharedPtr(A1 a1, A2 a2) {
        return SharedPtr<T>(new T(a1, a2));
    }
    
    template<typename T, typename A1, typename A2, typename A3>
    SharedPtr<T> MakeSharedPtr(A1 a1, A2 a2, A3 a3) {
        return SharedPtr<T>(new T(a1, a2, a3));
    }
    
    template<typename T, typename A1, typename A2, typename A3, typename A4>
    SharedPtr<T> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4) {
        return SharedPtr<T>(new T(a1, a2, a3, a4));
    }
    
    template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
    SharedPtr<T> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
        return SharedPtr<T>(new T(a1, a2, a3, a4, a5));
    }
    
    template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    SharedPtr<T> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) {
        return SharedPtr<T>(new T(a1, a2, a3, a4, a5, a6));
    }
    
    template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    SharedPtr<T> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) {
        return SharedPtr<T>(new T(a1, a2, a3, a4, a5, a6, a7));
    }
    
    template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    SharedPtr<T> MakeSharedPtr(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) {
        return SharedPtr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8));
    }
		
} // namespace ukn
#endif