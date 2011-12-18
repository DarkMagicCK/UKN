//
//  Singleton.h
//  Project Unknown
//
//  Created by Robert Bu on 12/16/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Singleton_h
#define Project_Unknown_Singleton_h


namespace ukn {
    
    // class for allocating singleton on the heap
    template<typename T, typename R>
    class SingletonHolder {
    public:
        SingletonHolder():
        mInstance(0) {
            
        }
        
        ~SingletonHolder() {
            if(this->mInstance)
                delete this->mInstance;
        }
        
        T* get(R arg) {
            if(!mInstance) mInstance = new T(arg);
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
                delete this->mInstance;
        }
        
        T* get(void) {
            if(!mInstance) mInstance = new T();
            return mInstance;
        }
        
    private:
        T* mInstance;
    };
    
    template<typename T>
    class Singleton {
    public:
        static T& Instance() {
            if(!mInstance) mInstance = new T();
            return *mInstance;
        }
        
        static void Destroy() {
            if(mInstance) 
                delete mInstance;
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
    
} // namespace ukn


#endif
