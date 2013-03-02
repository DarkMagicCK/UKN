/*
*  Ptr.h
*  Hoshizora
*
*  Created by Robert Bu on 8/24/10.
*  Copyright 2010 Robert Bu. All rights reserved.
*
*/

#ifndef MIST_ptr_h_
#define MIST_ptr_h_

#include "mist/Platform.h"
#include "mist/Exception.h"
#include "mist/MemoryUtil.h"
#include "mist/Uncopyable.h"
#include "mist/safe_bool.h"

namespace mist {

    /**
    * Some ptr object implementation
    * including SharedPtr, WeakPtr and ScopedPtr
    **/

    template<typename T, class RP>
    class SharedPtr;

    template<typename T>
    class ScopedPtr;

    template<typename T>
    class WeakPtr;

    template<typename T, typename Base>
    static T checked_cast(Base* pointer) {
        T derived = dynamic_cast<T>(pointer);
        if(!derived)
            MIST_THROW_EXCEPTION(L"checked_cast: failed to cast from parent to child");
        return derived;
    }

    class MIST_API SharedPtrRefCounter {
    public:
        SharedPtrRefCounter(): mRef(1), mWeakRef(1) {}

        void incRef() {
            ++this->mRef;
        }

        int decRef() {
            return --this->mRef;
        }

        int getRef() const {
            return this->mRef;
        }

        void incWeakRef() {
            ++this->mWeakRef;
        }

        int decWeakRef() {
            return --this->mWeakRef;
        }

        int getWeakRef() const {
            return this->mWeakRef;
        }

        void destruct() {
            delete this;
        }

        void release() {
            int32 newref = --this->mRef;
            if(newref != 0)
                return;

            this->weakRelease();
        }

        void weakRelease() {
            int32 newweakref = --this->mWeakRef;
            if(newweakref == 0)
                this->destruct();
        }

    private:
        int32 mRef;
        int32 mWeakRef;
    };

    template<class C>
    struct SharedPtrReleasePolicy {
        static C* Alloc() {
            return new C();
        }

        static void Release(C* obj) {
            if(obj)
                delete obj;
        }
    };

    template<class C>
    struct SharedPtrReleaseArrayPolicy {
        static void Release(C* obj) {
            if(obj)
                delete []obj;
        }
    };

    template<class C>
    struct SharedPtrFreeReleasePolicy {
        static C* Alloc() {
            return mist_malloc_t(C, 1);
        }

        static void Release(C* obj) {
            if(obj)
                mist_free(obj);
        }
    };

    template<typename T>
    class WeakPtr;

    template<typename T, class RP=SharedPtrReleasePolicy<T> >
    class SharedPtr: public safe_bool<SharedPtr<T, RP> > {
    public:
        typedef T element_type;

        SharedPtr(): 
            mPtr(0), 
            mCounter(new SharedPtrRefCounter) {
        }

        explicit SharedPtr(T* t): 
            mPtr(t), 
            mCounter(new SharedPtrRefCounter){ 
        }

        explicit SharedPtr(const WeakPtr<T>& rhs):
            mCounter(rhs.mCounter) {
                mPtr = rhs.mPtr;
        }

        template<class Other, class OtherRP>
        SharedPtr(const SharedPtr<Other, OtherRP>& rhs): 
            mCounter(rhs.mCounter),
            mPtr(const_cast<Other*>(rhs.get())) {
                this->mCounter->incRef();
        }

        SharedPtr(const SharedPtr& rhs):
            mCounter(rhs.mCounter),
            mPtr(rhs.mPtr) {
                this->mCounter->incRef();
        }

        ~SharedPtr() { 
            this->release();
        }

        SharedPtr& assign(T* _ptr) {
            if(get() != _ptr) {
                SharedPtrRefCounter* tmp = new SharedPtrRefCounter;
                release();
                this->mCounter = tmp;
                this->mPtr = _ptr;
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
        SharedPtr& assign(const SharedPtr<Other, OtherRP>& _ptr) {
            if(_ptr.get() != this->mPtr) {
                SharedPtr tmp(_ptr);
                swap(tmp);
            }
            return *this;
        }

        void reset(T* ptr=0) {
            this->assign(ptr);
        }

        SharedPtr& operator=(const SharedPtr& rhs) { 
            return this->assign(rhs);
        }
        SharedPtr& operator=(T* rhs) {
            return this->assign(rhs);
        }

        template<class Other, class OtherRP>
        SharedPtr& operator=(const SharedPtr<Other, OtherRP>& _ptr) {
            return this->assign<Other>(_ptr);
        }

        void swap(SharedPtr& rhs) {
            std::swap(this->mPtr, rhs.mPtr);
            std::swap(this->mCounter, rhs.mCounter);
        }

        template<class Other, class OtherRP>
        SharedPtr<Other, OtherRP> cast() const {
            Other* other = dynamic_cast<Other*>(this->mPtr);
            if(other) {
                return SharedPtr<Other, OtherRP>(this->mCounter, other);
            }
            return SharedPtr<Other, OtherRP>();
        }

        template<class Other, class OtherRP>
        SharedPtr<Other, OtherRP> unsafeCast() const {
            Other* other = static_cast<Other*>(this->mPtr);
            return SharedPtr<Other, OtherRP>(this->mCounter, other);
        }

        T* get() const {
            return this->mPtr;
        }

        T* operator->() const {
            return this->deref();
        }

        T& operator*() {
            return *this->deref();
        }
        T& operator*() const {
            return *this->deref();
        }

        bool boolean_test() const {
            return this->mPtr != 0;
        }

        bool operator !()const {
            return this->mPtr == 0;
        }
        bool operator ==(const SharedPtr& rhs) const {
            return this->get() != rhs.get();
        }
        bool operator ==(T* rhs) const {
            return this->get() != rhs;
        }
        bool operator !=(const SharedPtr& rhs) const {
            return this->get() != rhs.get();
        }
        bool operator !=(const T* rhs) const {
            return this->get() != rhs;
        }
       
        bool isValid() const {
            return this->mPtr!=0;
        }
        bool isNull() const {
            return this->mPtr==0;
        }

        int32 use_count() const { 
            return this->mCounter->getRef();
        }

        inline T* deref() const {
            if(!this->mPtr)
                MIST_THROW_EXCEPTION(L"mist::SharedPtr: invalid ptr to deref");
            return this->mPtr;
        }

    private:
        void release() {
            mist_assert(this->mCounter);
            int i = this->mCounter->decRef();
            if(i == 0) {
                if(this->mPtr) {
                    RP::Release(this->mPtr);
                    this->mPtr = 0;
                }
                this->mCounter->release();
            }
        }

        SharedPtr(SharedPtrRefCounter* pCounter, T* pPtr):
            mCounter(pCounter), mPtr(pPtr) {
                mist_assert(pCounter);
                pCounter->incRef();
        }

    private:
        T* mPtr;
        SharedPtrRefCounter* mCounter;

        template<typename OT>
        friend class WeakPtr;

        SharedPtr(T* ptr, SharedPtrRefCounter* ref):
            mPtr(ptr),
            mCounter(ref) {
                if(mCounter)
                    mCounter->incRef();
        }

        template<typename OC, typename ORP>
        friend class SharedPtr;

        template<typename T> friend class WeakPtr;
    };

    template <class C, class RP>
    inline void swap(SharedPtr<C, RP>& p1, SharedPtr<C, RP>& p2) {
        p1.swap(p2);
    }

    template<typename T>
    struct COMReleasePolicy {
        static void Release(T* pointer) {
            pointer->Release();
        }
    };

    template<typename T>
    struct COM {
        typedef SharedPtr<T, COMReleasePolicy<T> > Ptr;
    };

    template<typename T>
    static SharedPtr<T> MakeCOMPtr(T* pointer) {
        return SharedPtr<T, COMReleasePolicy<T> >(pointer);
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


    template<class T>
    class ScopedPtr: Uncopyable, public safe_bool<ScopedPtr<T> > {
    public:
        typedef T element_type;

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
            mist_assert(this->ptr != 0);
            return *this->ptr;
        }
        T* operator->() const {
            return this->ptr;
        }

        T* get() const {
            return this->ptr;
        }

        bool boolean_test() const {
            return ptr != 0;
        }

        void swap(ScopedPtr& b);

    private:
        T* ptr;
    };

    template<class T>
    void swap(ScopedPtr<T>& a, ScopedPtr<T>& b) {
        std::swap(a.ptr, b.ptr);
    }

    template<typename T>
    class WeakPtr: public safe_bool<WeakPtr<T> > {
    public:
        typedef T element_type;

        WeakPtr():
            mPtr(0),
            mCounter(0) {

        }

        WeakPtr(const WeakPtr<T>& ptr):
            mCounter(ptr.mCounter) {
                if(mCounter) {
                    mCounter->incWeakRef();
                    mPtr = ptr.lock().get();
                }
        }

        template<class Y> WeakPtr(const SharedPtr<Y>& ptr):
        mPtr(ptr.get()),
            mCounter(ptr.mCounter) {
                if(mCounter) {
                    mCounter->incWeakRef();
                }
        }

        template<class Y> WeakPtr(const WeakPtr<Y>& ptr):
        mCounter(ptr.mCounter) {
            if(mCounter) {
                mCounter->incWeakRef();
                mPtr = ptr.lock().get();
            }
        }

        ~WeakPtr() {
            if(mCounter) {
                mCounter->decWeakRef();
            }
        }

        WeakPtr& operator=(const WeakPtr& r) {
            mPtr = r.lock().get();
            mCounter = r.mCounter;
            if(mCounter)
                mCounter->incWeakRef();
            return *this;
        }

        template<class Y> WeakPtr & operator=(const WeakPtr<Y>& r) {
            mPtr = r.lock().get();
            mCounter = r.mCounter;
            if(mCounter)
                mCounter->incWeakRef();
            return *this;
        }

        template<class Y> WeakPtr & operator=(const SharedPtr<Y>& r) {
            mPtr = r.get();
            mCounter = r.mCounter;

            if(mCounter)
                mCounter->incWeakRef();
            return *this;
        }

        int32 use_count() const {
            return mCounter ? mCounter->getRef() : 0;
        }

        int32 weak_count() const {
            return mCounter ? mCounter->getWeakRef() : 0;
        }

        bool expired() const {
            return (mCounter && mCounter->getRef() == 0) || !mCounter;
        }

        SharedPtr<T> lock() const {
            return SharedPtr<T>(mPtr, mCounter);
        }

        bool boolean_test() const {
            return this->expired();
        }

        void reset() {
            mPtr = 0;
            if(mCounter) {
                mCounter->decWeakRef();
            }
            mCounter = 0;
        }

        void swap(WeakPtr<T> & b) {
            std::swap(mPtr, b.mPtr);
            std::swap(mCounter, b.mCounter);
        }

    private:
        T* mPtr;
        SharedPtrRefCounter* mCounter;

        template<typename Y>
        friend class WeakPtr;

        template<typename T, typename A>
        friend class SharedPtr;
    };

    template<class T>
    void swap(WeakPtr<T>& a, WeakPtr<T>& b) {
        std::swap(a.mPtr, b.mPtr);
        std::swap(a.mCounter, b.mCounter);
    }

    const struct {
        template<typename T>
        operator T*() { return reinterpret_cast<T*>(0); }

    private:
        void operator&() const;
    } nullptr_t = {};


    // copy of boost::enable_shared_from_this
    template<typename T>
    class enable_shared_from_this {
    protected:
        enable_shared_from_this() { 
        }

        enable_shared_from_this(const enable_shared_from_this&) {
        }

        enable_shared_from_this& operator=(const enable_shared_from_this&) {
            return *this;
        }

        ~enable_shared_from_this() {
        }

    public:
        SharedPtr<T> shared_from_this() {
            SharedPtr<T> p(_internal_weak_this);
            assert(p.get() == this);
            return p;
        }

        SharedPtr<T const> shared_from_this() const {
            SharedPtr<T const> p(_internal_weak_this);
            assert(p.get() == this);
            return p;
        }

        typedef T _internal_element_type;
        mutable WeakPtr<_internal_element_type> _internal_weak_this;
    };

} // namespace mist
#endif