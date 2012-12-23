//
//  Thread.cpp
//  Mist
//
//  Created by Robert Bu on 12/13/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/Thread.h"

#if defined(MIST_OS_WINDOWS)
#include <process.h>

#elif defined(MIST_OS_FAMILY_UNIX)
#include <unistd.h>
#include <map>

#endif

#include "mist/Logger.h"
#include "mist/Singleton.h"

#if defined(MIST_OS_OSX) || defined(MIST_OS_IOS)
#include <dispatch/dispatch.h>
#endif

namespace mist {
    
    namespace {
        static mist::thread::ThreadId _main_thread_id = mist::thread::Thread::GetCurrentThreadId();
    }
    
    namespace thread {
        
        Mutex::Mutex():
        mutex(new std::mutex()) {
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            mLocked = false;
            InitializeCriticalSectionAndSpinCount(&cs, 4000);
#elif defined(MIST_OS_FAMILY_UNIX)
            pthread_mutex_init(&mutex, NULL);
#endif
#endif
        }
        
        Mutex::~Mutex() {
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            DeleteCriticalSection(&cs);
#elif defined(MIST_OS_FAMILY_UNIX)
            pthread_mutex_destroy(&mutex);
#endif
#endif
        }
        
        inline void Mutex::lock() {
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            EnterCriticalSection(&cs);
            while(mLocked) Sleep(1000);
            mLocked = true;
#elif defined(MIST_OS_FAMILY_UNIX)
            pthread_mutex_lock(&mutex);
#endif
#else
            try {
                mutex->lock();
            } catch(std::system_error& e) {
                log_error(e.what());
            }
#endif
        }
        
        inline bool Mutex::tryLock() {
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            bool ret = (TryEnterCriticalSection(&cs) ? true: false);
            if(ret && mLocked) {
                LeaveCriticalSection(&cs);
                ret = false;
            }
            return ret;
#elif defined(MIST_OS_FAMILY_UNIX)
            return (pthread_mutex_trylock(&mutex) == 0) ? true: false;
#endif
#else
            return mutex->try_lock();
#endif
        }
        
        inline void Mutex::unlock() {
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            mLocked= false;
            LeaveCriticalSection(&cs);
#elif defined(MIST_OS_FAMILY_UNIX)
            pthread_mutex_unlock(&mutex);
#endif
#else
            mutex->unlock();
#endif
        }
        
        void* Mutex::getSysMutex() {
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            return &cs;
#elif defined(MIST_OS_FAMILY_UNIX)
            return &mutex;
#endif
#else
            return mutex->native_handle();
#endif
        }
        
        RecursiveMutex::RecursiveMutex() {
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            InitializeCriticalSection(&cs);
#elif defined(MIST_OS_FAMILY_UNIX)
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            pthread_mutex_init(&mutex, &attr);
#endif
#endif
        }
        
        RecursiveMutex::~RecursiveMutex() {
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            DeleteCriticalSection(&cs);
#elif defined(MIST_OS_FAMILY_UNIX)
            pthread_mutex_destroy(&mutex);
#endif
#endif
        }
        
        inline void RecursiveMutex::lock() {
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            EnterCriticalSection(&cs);
#elif defined(MIST_OS_FAMILY_UNIX)
            pthread_mutex_lock(&mutex);
#endif
#else
            mutex.lock();
#endif
        }
        
        inline bool RecursiveMutex::try_lock() {
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            return TryEnterCriticalSection(&cs) ? true : false;
#elif defined(MIST_OS_FAMILY_UNIX)
            return (pthread_mutex_trylock(&mutex) == 0) ? true : false;
#endif
#else
            return mutex.try_lock();
#endif
        }
        
        inline void RecursiveMutex::unlock() {
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            LeaveCriticalSection(&cs);
#elif defined(MIST_OS_FAMILY_UNIX)
            pthread_mutex_unlock(&mutex);
#endif
#else
            mutex.unlock();
#endif
        }

        Semaphore::Semaphore(int _n)
#ifndef MIST_OS_WINDOWS
        :
        mutex(),
        cond(mutex),
        n(_n),
        max(_n)     
#endif
        {
                
#ifdef MIST_OS_WINDOWS
            sema = CreateSemaphoreW(NULL, _n, _n, NULL);
#endif
        }
                
        Semaphore::Semaphore(int _n, int _max)
//#ifndef MIST_OS_WINDOWS
        :
        mutex(),
        cond(mutex),
        n(_n),
        max(_max)
//#endif
        {
            assert( _n>=0 && _max>0 && _n<=_max);
/*
#ifdef MIST_OS_WINDOWS
                    
            sema = CreateSemaphoreW(NULL, _n, _max, NULL);
#endif*/
        }
                    
        Semaphore::~Semaphore() { 
/*#ifdef MIST_OS_WINDOWS
            CloseHandle(sema);
#endif*/
        }
                    
        void Semaphore::wait() {
/*#ifdef MIST_OS_WINDOWS
            switch(WaitForSingleObject(sema,INFINITE)) {
                case WAIT_OBJECT_0:
                    return;
                default:
                    MIST_THROW_EXCEPTION("mist::Thread::Semaphore: error wait");
            }
#elif defined(MIST_OS_FAMILY_UNIX)*/
            MutexGuard<Mutex> lock(mutex);
            while(n < 1) {
                cond.wait();
            }
            --n;
//#endif
        }
                    
        void Semaphore::set() {
/*#ifdef MIST_OS_WINDOWS
            if(!ReleaseSemaphore(sema, 1, NULL)) {
                    MIST_THROW_EXCEPTION("mist::Thread::Semaphore: error set");
            }
#elif defined(MIST_OS_FAMILY_UNIX)*/
            MutexGuard<Mutex> lock(mutex);
            if(n < max) {
                ++n;
            } else {
                MIST_THROW_EXCEPTION("mist::Thread::Semaphore: Cannot signal semaphore, exceed maximun");
            }
            cond.notify();
//#endif
        }
             
#ifdef MIST_OS_WINDOWS
        
        static int 
        _win32_pthread_cond_init (pthread_cond_t *cv,
                                  const void *) {
            cv->waiters_count_ = 0;
            cv->was_broadcast_ = 0;
            cv->sema_ = CreateSemaphoreA (NULL,       // no security
                                          0,          // initially 0
                                          0x7fffffff, // max count
                                          NULL);      // unnamed 
            InitializeCriticalSection (&cv->waiters_count_lock_);
            cv->waiters_done_ = CreateEventA (NULL,  // no security
                                              FALSE, // auto-reset
                                              FALSE, // non-signaled initially
                                              NULL); // unnamed
            return 0;
        }
        
        static void _win32_pthread_cond_destroy(pthread_cond_t *cv) {
            CloseHandle(cv->waiters_done_);
            DeleteCriticalSection (&cv->waiters_count_lock_);
        }
        
        static int
        _win32_pthread_cond_wait (pthread_cond_t *cv, 
                                  pthread_mutex_t *external_mutex) {
            // Avoid race conditions.
            EnterCriticalSection (&cv->waiters_count_lock_);
            cv->waiters_count_++;
            LeaveCriticalSection (&cv->waiters_count_lock_);
            
            SignalObjectAndWait (*external_mutex, cv->sema_, INFINITE, FALSE);
            EnterCriticalSection (&cv->waiters_count_lock_);
            
            cv->waiters_count_--;
            
            int last_waiter = cv->was_broadcast_ && cv->waiters_count_ == 0;
            
            LeaveCriticalSection (&cv->waiters_count_lock_);
            
            if (last_waiter)
                SignalObjectAndWait (cv->waiters_done_, *external_mutex, INFINITE, FALSE);
            else
                WaitForSingleObject (*external_mutex, INFINITE);
            return 0;
        }
        
        static int
        _win32_pthread_cond_signal (pthread_cond_t *cv) {
            EnterCriticalSection (&cv->waiters_count_lock_);
            int have_waiters = cv->waiters_count_ > 0;
            LeaveCriticalSection (&cv->waiters_count_lock_);
            
            if (have_waiters)
                ReleaseSemaphore (cv->sema_, 1, 0);
            return 0;
        }
        
        static int
        _win32_pthread_cond_broadcast (pthread_cond_t *cv) {
            EnterCriticalSection (&cv->waiters_count_lock_);
            int have_waiters = 0;
            
            if (cv->waiters_count_ > 0) {
                cv->was_broadcast_ = 1;
                have_waiters = 1;
            }
            
            if (have_waiters) {
                ReleaseSemaphore (cv->sema_, cv->waiters_count_, 0);
                LeaveCriticalSection (&cv->waiters_count_lock_);
                
                WaitForSingleObject (cv->waiters_done_, INFINITE);
                cv->was_broadcast_ = 0;
            }
            else
                LeaveCriticalSection (&cv->waiters_count_lock_);
            return 0;
        }
#endif // MIST_OS_WINDOWS
        
        Condition::Condition(Mutex& mutex):
        _mutex(mutex) {
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            _win32_pthread_cond_init(&cond, NULL);
#elif defined(MIST_OS_FAMILY_UNIX)
            pthread_cond_init(&cond, NULL);
#endif
            
#endif
        }
        
        Condition::~Condition() {
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            _win32_pthread_cond_destroy(&cond);
#elif defined(MIST_OS_FAMILY_UNIX)
            pthread_cond_destroy(&cond);
#endif
#endif
        }
        
        void Condition::wait() {
#ifndef MIST_CPP11
#ifndef MIST_OS_WINDOWS
            if(pthread_cond_wait(&cond,
                                 static_cast<pthread_mutex_t*>(_mutex.getSysMutex())))
                MIST_THROW_EXCEPTION("mist::Thread::Condition: error wait");
#elif defined(MIST_OS_FAMILY_UNIX)
            if(_win32_pthread_cond_wait(&cond,
                                        static_cast<pthread_mutex_t*>(_mutex.getSysMutex())))
                MIST_THROW_EXCEPTION("mist::Thread::Condition: error wait");
#endif
#else
            std::unique_lock<std::mutex> lock(*(std::mutex*)_mutex.getSysMutex());
            cond.wait(lock);
#endif
        }
        
        void Condition::notify() {
#ifndef MIST_CPP11
#ifndef MIST_OS_WINDOWS
            if(pthread_cond_signal(&cond))
                MIST_THROW_EXCEPTION("mist::Thread::Condition: error notify");
#elif defined(MIST_OS_FAMILY_UNIX)
            if(_win32_pthread_cond_signal(&cond))
                MIST_THROW_EXCEPTION("mist::Thread::Condition: error notify");
#endif
#else
            cond.notify_one();
#endif
        }
        
        void Condition::notifyAll() {
#ifndef MIST_CPP11
#ifndef MIST_OS_WINDOWS
            if(pthread_cond_broadcast(&cond))
                MIST_THROW_EXCEPTION("mist::Thread::Condition: error broadcast");
#elif defined(MIST_OS_FAMILY_UNIX)
            if(_win32_pthread_cond_broadcast(&cond))
                MIST_THROW_EXCEPTION("mist::Thread::Condition: error broadcast");
#endif
#else
            cond.notify_all();
#endif
        }
        
        ThreadTask::ThreadTask():
        mArg(0) {
            
        }
        
        ThreadTask::ThreadTask(const ThreadFunc& func):
        mArg(0),
        mFunc(func) {
            
        }
        
        ThreadTask::ThreadTask(const ThreadFunc& func, void* arg):
        mArg(arg),
        mFunc(func) {
            
        }
        
        void ThreadTask::setArg(void* arg) {
            mArg = arg;
        }
        
        void* ThreadTask::getArg() const {
            return mArg;
        }
        
        void ThreadTask::operator()() const {
            if(mFunc)
                mFunc(mArg);
            if(mCallback)
                mCallback();
        }
        
        bool ThreadTask::isValid() const {
            return (bool)mFunc;
        }
        
        ThreadTask& ThreadTask::then(const ThreadTask::CallbackFunc& cb) {
            mCallback = cb;
            return *this;
        }
        
#ifndef MIST_CPP11
        
#ifdef MIST_OS_FAMILY_UNIX
        static ThreadId _pthread_t_to_ID(const pthread_t& handle) {
            static Mutex idMapLoc;
            static std::map<pthread_t, uint32> idMap;
            static uint32 idCount(1);
            
            MutexGuard<Mutex> guard(idMapLoc);
            if(idMap.find(handle) == idMap.end()) 
                idMap[handle] = idCount++;
            return ThreadId(idMap[handle]);
        }
#endif
        
        
#endif
        
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
        
        unsigned int WINAPI Thread::WrapperFunc(LPVOID pthis) {
            
#elif defined(MIST_OS_FAMILY_UNIX)
            
            void* Thread::WrapperFunc(void* pthis) {
                
#endif
#else
                void Thread::WrapperFunc(void* pthis) {
#endif
    
                Thread* pthread = (Thread*)pthis;
                
                try {
                    pthread->mTask();
                } catch(std::exception& e) {
                    log_error(e.what());
                }
                
                MutexGuard<Mutex> guard(pthread->mDataMutex);
                pthread->mIsActive = false;
#ifndef MIST_CPP11
                return 0;
#endif
            }

            
            Thread::Thread(const ThreadTask& task):
            mIsActive(false) {
                this->start(task);
            }
            
            Thread::~Thread() {
                
                this->join();
            }
            
            bool Thread::start(const ThreadTask& task) {
                if(mIsActive) 
                    return 0;
                
                mIsActive = true;
                mTask = task;
                
#ifndef MIST_CPP11
                
#ifdef MIST_OS_WINDOWS
                mHandle = (HANDLE)_beginthreadex(0, 0, Thread::WrapperFunc, (void*)this, 0, &mWin32ThreadId);
                mId = ThreadId(mWin32ThreadId);
#elif defined(MIST_OS_FAMILY_UNIX)
                if(pthread_create(&mHandle, NULL, Thread::WrapperFunc, (void*)this) != 0)
                    mHandle = 0;
                
                mId = _pthread_t_to_ID(mHandle);
#endif
          
                if(!mHandle) {
                    mIsActive = false;
                }
                
#else
                mThread = new std::thread(Thread::WrapperFunc, this);
                mId =  ThreadId(mThread->get_id());
#endif
                
                return mIsActive;
            }
            
            Thread::native_handle_type Thread::getNativeHandle() {
#ifndef MIST_CPP11
                return mHandle;
#else
                if(mThread)
                    return mThread->native_handle();
                return 0;
#endif
            }
            
            bool Thread::isActive() const {
                return mIsActive;
            }
            
            bool Thread::joinable() {
                mDataMutex.lock();
                bool result = !mIsActive;
                mDataMutex.unlock();
                return result;
            }
            
            void Thread::join() {
                if(joinable()) {
#ifndef MIST_CPP11
                    
#ifdef MIST_OS_WINDOWS
                    WaitForSingleObject(mHandle, INFINITE);
#elif defined(MIST_OS_FAMILY_UNIX)
                    pthread_join(mHandle, 0);
#endif
                    
#else
                    if(mThread && mThread->joinable())
                        mThread->join();
#endif
                }
            }
            
            ThreadId Thread::getId() const {
                return mId;
            }
            
            uint32 Thread::HardwareConcurrency() {
#ifndef MIST_CPP11
                
#ifdef MIST_OS_WINDOWS
                SYSTEM_INFO si;
                GetSystemInfo(&si);
                return (uint32)si.dwNumberOfProcessors;
#elif defined(MIST_OS_FAMILY_UNIX)
                return (uint32)sysconf(_SC_NPROCESSORS_ONLN);
#endif
                
#else
                return std::thread::hardware_concurrency();
#endif
                return 0;
            }
            
            void Thread::exit() {
                
#ifndef MIST_CPP11
                
#ifdef MIST_OS_WINDOWS
                TerminateThread(mHandle, 0);
#elif defined(MIST_OS_FAMILY_UNIX)
                pthread_exit(&mHandle);
#endif
                
#else
 
// todo with windows, since there's no exit in std::thread
#if defined(MIST_OS_FAMILY_UNIX)

                if(mThread)
                    pthread_exit(mThread->native_handle());
#endif
                    
#endif
            }
            
            ThreadId Thread::GetCurrentThreadId() {
#ifndef MIST_CPP11
                
#ifdef MIST_OS_WINDOWS
                return ThreadId((uint32)::GetCurrentThreadId());
#elif defined(MIST_OS_FAMILY_UNIX)
                return _pthread_t_to_ID(pthread_self());
#endif
                
#else
                return ThreadId(std::this_thread::get_id());
#endif
            }
            
#ifdef MIST_OS_WINDOWS
            
            RWLock::RWLock(): 
            readers(0),
            writersWaiting(0),
            writers(0) {
                mutex = CreateMutexW(NULL, FALSE, NULL);
                if(mutex != NULL) {
                    readEvent = CreateEventW(NULL, TRUE, TRUE, NULL);
                    if(readEvent != NULL) {
                        writeEvent = CreateEventW(NULL, TRUE, TRUE, NULL);
                    }
                }
            }
            
#elif defined(MIST_OS_FAMILY_UNIX)
            RWLock::RWLock() {
                pthread_rwlock_init(&rwl, NULL);
            }
#endif         
            
            RWLock::~RWLock() {
#ifdef MIST_OS_WINDOWS
                CloseHandle(mutex);
                CloseHandle(readEvent);
                CloseHandle(writeEvent);
#elif defined(MIST_OS_FAMILY_UNIX)
                pthread_rwlock_destroy(&rwl);
#endif
            }
            
            void RWLock::readLock() {
#ifdef MIST_OS_WINDOWS
                HANDLE h[2];
                h[0] = mutex;
                h[1] = readEvent;
                switch(WaitForMultipleObjects(2, h, TRUE, INFINITE)) {
                    case WAIT_OBJECT_0:
                    case WAIT_OBJECT_0+1:
                        ++readers;
                        ResetEvent(writeEvent);
                        ReleaseMutex(mutex);
                        break;
                    default:
                        MIST_THROW_EXCEPTION("mist::Thread::RWLock: Cannot lock reader/writer lock");
                }
#elif defined(MIST_OS_FAMILY_UNIX)
                if(pthread_rwlock_rdlock(&rwl))
                    MIST_THROW_EXCEPTION("mist::Thread::RWLock: Cannot lock reader/writer lock");
#endif
            }
            
            void RWLock::writeLock() {
#ifdef MIST_OS_WINDOWS
                addWriter();
                HANDLE h[2];
                h[0] = mutex;
                h[1] = writeEvent;
                switch(WaitForMultipleObjects(2, h, TRUE, INFINITE)) {
                    case WAIT_OBJECT_0:
                    case WAIT_OBJECT_0+1:
                        --writersWaiting;
                        ++readers;
                        ++writers;
                        ResetEvent(readEvent);
                        ResetEvent(writeEvent);
                        ReleaseMutex(mutex);
                        break;
                    default:
                        removeWriter();
                        MIST_THROW_EXCEPTION("mist::Thread::RWLock: Cannot lock reader/writer lock");
                }
#elif defined(MIST_OS_FAMILY_UNIX)
                if(pthread_rwlock_wrlock(&rwl))
                    MIST_THROW_EXCEPTION("mist::Thread::RWLock: Cannot lock reader/writer lock");
#endif     
            }
            
            void RWLock::unlock() {
#ifdef MIST_OS_WINDOWS
                switch(WaitForSingleObject(mutex, INFINITE)) {
                    case WAIT_OBJECT_0:
                        writers = 0;
                        if(writersWaiting == 0)
                            SetEvent(readEvent);
                        if(--readers == 0)
                            SetEvent(writeEvent);
                        ReleaseMutex(mutex);
                        break;
                    default:
                        MIST_THROW_EXCEPTION("mist::Thread::RWLock: Cannot unlock reader/writer lock");
                }
#elif defined(MIST_OS_FAMILY_UNIX)
                if(pthread_rwlock_unlock(&rwl))
                    MIST_THROW_EXCEPTION("mist::Thread::RWLock: Cannot unlock reader/writer lock");
#endif   
            }
            
#ifdef MIST_OS_WINDOWS
            
            inline bool RWLock::isValid() const {
                return mutex && readEvent && writeEvent;
            }
            
            inline void RWLock::addWriter() {
                switch(WaitForSingleObject(mutex, INFINITE)) {
                    case WAIT_OBJECT_0:
                        if(++writersWaiting == 1)
                            ResetEvent(readEvent);
                        ReleaseMutex(mutex);
                        break;
                    default:
                        MIST_THROW_EXCEPTION("mist::Thread::RWLock: Cannot add writer");
                }
            }
            
            inline void RWLock::removeWriter() {
                switch(WaitForSingleObject(mutex, INFINITE)) {
                    case WAIT_OBJECT_0:
                        if(--writersWaiting == 0 && writers == 0)
                            SetEvent(readEvent);
                        ReleaseMutex(mutex);
                        break;
                    default:
                        MIST_THROW_EXCEPTION("mist::Thread::RWLock: Cannot remove writer");
                }
            }  
			  
#endif

        ThreadPool::ThreadPool():
        mMutex(),
        mCond(mMutex),
        mRunning(false) {
            
        }
        
        ThreadPool::~ThreadPool() {
            mRunning = false;
            join();
            
            std::for_each(mThreads.begin(),
                          mThreads.end(),
                          DeleteSTLPtr());
        }
        
        void ThreadPool::start(uint32 numThreads) {
            if(!mThreads.empty()) {
                log_error("mist::ThreadPool::Start: thread pool already started");
            }
            
            mRunning = false;
            mThreads.reserve(numThreads);
            for(uint32 i=0; i<numThreads; ++i) {
                ThreadTask task(Bind(this, &ThreadPool::runInThread), (void*)0);
                
                Thread* thread = new Thread(task);
                mThreads.push_back(thread);
            }
        }
        
        void ThreadPool::join() {
            if(mRunning) {
                mRunning = false;
                mCond.notifyAll();
                
                ThreadList::const_iterator it = mThreads.begin();
                for(; it != mThreads.end(); ++it)
                    (*it)->join();
            }
        }
        
        void ThreadPool::run(const ThreadTask& task) {
            if(mThreads.empty()) {
                task();
            } else {
                MutexGuard<Mutex> lock(mMutex);
                mTasks.push(task);
                mCond.notify();
            }
        }
        
        ThreadTask ThreadPool::take() {
            MutexGuard<Mutex> lock(mMutex);
            while(mTasks.empty() && mRunning)
                mCond.wait();
            
            ThreadTask task;
            if(!mTasks.empty()) {
                task = mTasks.front();
                mTasks.pop();
            }
            return task;
        }
        
        void ThreadPool::runInThread(void* arg) {
            try {
                while(mRunning) {
                    ThreadTask task(take());
                    task();
                }
            } catch(...) {
                log_error("mist::ThreadPool::runInThread: error executing task");
            }
        }
        
        bool ThreadPool::isRunning() const {
            return mRunning;
        }
        
        namespace {
            static SingletonHolder<ThreadPool, void> _threadpool_instance;
        }
        
        ThreadPool& ThreadPool::DefaultObject() {
            return *_threadpool_instance.get();
        }
            
        ThreadTaskPool::ThreadTaskPool() {
            
        }
        
        ThreadTaskPool::~ThreadTaskPool() {
            
        }
        
        void ThreadTaskPool::add(const ThreadTask& task) {
            MutexGuard<Mutex> lock(mMutex);
            
            mTasks.push(task);
        }
        
        void ThreadTaskPool::run() {
            MutexGuard<Mutex> lock(mMutex);
            
            while(!mTasks.empty()) {
                ThreadTask task = mTasks.back();
                
                task();
                
                mTasks.pop();
            }
        }
        
        namespace {
            static SingletonHolder<ThreadTaskPool, void> _threadtaskpool_instance;
        }
        
        ThreadTaskPool& ThreadTaskPool::DefaultObject() {
            return *_threadtaskpool_instance.get();
        }
            
                
        void RunInMainThread(const mist::thread::ThreadTask& task) {
            if(mist::thread::Thread::GetCurrentThreadId() == _main_thread_id) {
                task();
            } else {
#if defined(MIST_OS_OSX) || defined(MIST_OS_IOS)
                /* on OSX and iOS, use GCD directly */
                __block mist::thread::ThreadTask _task = task;
                dispatch_async(
                   dispatch_get_main_queue(),
                   ^() {
                       _task();
                   }
                );
#else
                /* otherwise add to main thread task pool, and let app run it when update */
                mist::thread::ThreadTaskPool::DefaultObject().add(task);
#endif
            }
        }

            
    }
    
} // namespace mist
