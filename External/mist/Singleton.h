//
//  Singleton.h
//  Mist
//
//  Created by Robert Bu on 12/16/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTSingleton_h
#define MISTSingleton_h


namespace mist {
    
    template<typename T>
    struct SingletonClassInstantiator {
        static T* Instantiate() {
            return new T();
        }
        
        template<typename R>
        static T* Instantiate(R arg) {
            return new T(arg);
        }
        
        static void Release(T* obj) {
            delete obj;
        }
    };
    
    // class for allocating singleton on the heap
    template<typename T, typename R>
    class SingletonHolder {
    public:
        SingletonHolder():
        mInstance(0) {
            
        }
        
        ~SingletonHolder() {
            if(this->mInstance)
                SingletonClassInstantiator<T>::Release(this->mInstance);
        }
        
        T* get(R arg) {
            if(!mInstance) mInstance = SingletonClassInstantiator<T>::template Instantiate<R>(arg);
            return mInstance;
        }
        
    private:
        T* mInstance;
    };
    
    template<typename T>
    class SingletonHolder<T, void> {
    public:
        SingletonHolder():
        mInstance(0) {
            
        }
        
        ~SingletonHolder() {
            if(this->mInstance)
                SingletonClassInstantiator<T>::Release(this->mInstance);
        }
        
        T* get(void) {
            if(!mInstance) mInstance = SingletonClassInstantiator<T>::Instantiate();
            return mInstance;
        }
        
    private:
        T* mInstance;
    };
    
    template<typename T>
    class Singleton {
    public:
        static T& Instance() {
            return *InstancePtr();
        }
        
        static T* InstancePtr() {
            if(!mInstance) mInstance = SingletonClassInstantiator<T>::Instantiate();
            return mInstance;
        }
        
        static void Destroy() {
            if(mInstance) 
                SingletonClassInstantiator<T>::Release(mInstance);
            mInstance = 0;
        }
        
    private:
        static T* mInstance;
        
    protected:
        Singleton() { }
        virtual ~Singleton() { }
    };
    
    template<typename T>
    T* Singleton<T>::mInstance = 0;
    
} // namespace mist


#endif
