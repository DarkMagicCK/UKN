//
//  Thread.h
//  Mist
//
//  Created by Robert Bu on 12/13/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTThread_h
#define MISTThread_h

#include "mist/Platform.h"
#include "mist/Uncopyable.h"

#ifndef MIST_OS_WINDOWS
#include <pthread.h>
#include <signal.h>
#include <sched.h>
#include <unistd.h>
#else
#include <Windows.h>
#endif // MIST_OS_WINDOWS

#include "mist/Exception.h"
#include "mist/Function.h"

#include <vector>
#include <deque>
#include <queue>

#ifdef MIST_CPP11
#include <thread>
#include <mutex>
#include <future>
#include <functional>

#endif

#include "mist/Ptr.h"
#include "mist/TimeUtil.h"

namespace mist {
    
    namespace thread {
        
        class MIST_API Mutex {
        public:
            Mutex();
            ~Mutex();
            
            void lock();
            bool tryLock();
            
            void unlock();
            void* getSysMutex();
            
        private:
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            CRITICAL_SECTION cs;
            bool mLocked;
#elif defined(MIST_OS_FAMILY_UNIX)
            pthread_mutex_t mutex;
#endif
#else
            ScopedPtr<std::mutex> mutex;
#endif
        };
        
        class RecursiveMutex {
        public:
            RecursiveMutex();
            ~RecursiveMutex();
            
            void lock();
            bool try_lock();
            void unlock();
            
        private:
#ifndef MIST_CPP11
#ifdef MIST_OS_WINDOWS
            CRITICAL_SECTION cs;
#elif defined(MIST_OS_FAMILY_UNIX)
            pthread_mutex_t mutex;
#endif
#else
            std::recursive_mutex mutex;
#endif
            friend class Condition;
        };

#ifdef MIST_OS_WINDOWS
        typedef struct {
            int waiters_count_;            
            CRITICAL_SECTION waiters_count_lock_;
            
            HANDLE sema_;
            HANDLE waiters_done_;
            size_t was_broadcast_;
        } pthread_cond_t;
        
        typedef HANDLE pthread_mutex_t;
#endif
        
        template<typename T>
        class MutexGuard: Uncopyable {
        public:
            typedef T mutex_type;
            
            explicit MutexGuard(mutex_type& mutex): _mutex(mutex) {
                _mutex.lock();
            }
            
            ~MutexGuard() {
                _mutex.unlock();
            }
            
        private:
            mutex_type& _mutex;
        };
        
        class MIST_API Condition: Uncopyable {
        public:
            explicit Condition(Mutex& mutex);
            
            ~Condition();
            
            void wait();
            void notify();
            void notifyAll();
        
        private:
            
#ifndef MIST_CPP11
            pthread_cond_t cond;
#else
            std::condition_variable cond;
#endif
        protected:
            Mutex& _mutex;
        };
        
        
        class Thread;
        
        class MIST_API ThreadId {
        public:
#ifndef MIST_CPP11
            typedef uint32 id_type;
            ThreadId(): mId(0) { }

#else
            typedef std::thread::id id_type;
            ThreadId() { }
#endif
            
            ThreadId(id_type aid): mId(aid) { }
            
            ThreadId(const ThreadId& rhs): mId(rhs.mId) { }
            
            inline ThreadId& operator=(const ThreadId& rhs) {
                if(this != &rhs) {
                    mId  = rhs.mId;
                }
                return *this;
            }
            
            inline friend bool operator==(const ThreadId& id1, const ThreadId& id2) {
                return id1.mId == id2.mId;
            }
            
            inline friend bool operator!=(const ThreadId& id1, const ThreadId& id2) {
                return id1.mId != id2.mId;
            }
            
            inline friend bool operator<=(const ThreadId& id1, const ThreadId& id2) {
                return id1.mId <= id2.mId;
            }
            
            inline friend bool operator>=(const ThreadId& id1, const ThreadId& id2) {
                return id1.mId >= id2.mId;
            }
            
            inline friend bool operator<(const ThreadId& id1, const ThreadId& id2) {
                return id1.mId < id2.mId;
            }
            
            inline friend bool operator>(const ThreadId& id1, const ThreadId& id2) {
                return id1.mId > id2.mId;
            }
            
            inline friend std::ostream& operator<<(std::ostream& os, const ThreadId& aid) {
                os << aid.mId;
                return os;
            }
            
        private:
            /* thread id is an point on darwin, so we need uintPtr here */
            id_type mId;
        };
        
        class MIST_API ThreadTask {
        public:
            typedef std::function<void(void*)> ThreadFunc;
            typedef std::function<void()> CallbackFunc;
            
            ThreadTask();
            ThreadTask(const ThreadFunc& func);
            ThreadTask(const ThreadFunc& func, void* arg);
            
#if defined(MIST_CPP11) 
            
#ifdef MIST_VARIADIC_TEMPLATE
      
            template<typename _Fp, typename ...ARGS>
            ThreadTask(_Fp fp, ARGS... args) {
                mFunc = [=](void*) {
                    fp(args...);
                };
            }
            
#endif
            
#endif
            
            void    setArg(void* arg);
            void*   getArg() const;
            
            void operator()() const;
            
            bool isValid() const;
            
            ThreadTask& then(const CallbackFunc& cb);
            
        private:
            void* mArg;
            ThreadFunc mFunc;
            CallbackFunc mCallback;
        };
        
        class MIST_API Thread {
        public:
#ifndef MIST_CPP11
            
#ifdef MIST_OS_WINDOWS
            typedef HANDLE native_handle_type;
#elif defined(MIST_OS_FAMILY_UNIX)
            typedef pthread_t native_handle_type;
#endif
            
#else
            typedef std::thread::native_handle_type native_handle_type;
#endif
            
        public:
            Thread(const ThreadTask& task);
            ~Thread();
            
            bool isActive() const;
            
            void exit();
            void join();
            
            bool joinable();
            
            ThreadId getId() const;
            
            native_handle_type getNativeHandle();
            
            static uint32 HardwareConcurrency();
            static ThreadId GetCurrentThreadId();
            
        private:
            bool start(const ThreadTask& task);
            
            ThreadTask mTask;
            bool mIsActive;
            Mutex mDataMutex;
            
            ThreadId mId;
#ifndef MIST_CPP11
            
#ifdef MIST_OS_WINDOWS
            uint32 mWin32ThreadId;
            HANDLE mHandle;
            
            static unsigned int WINAPI WrapperFunc(LPVOID param);
#elif defined(MIST_OS_FAMILY_UNIX)
            static void* WrapperFunc(void* pthis);
            
            pthread_t mHandle;
            pthread_attr_t mAttr;
#endif
            
#else
            std::future<int> f;
            std::thread* mThread;
            static void WrapperFunc(void* pthis);
#endif
        };
        
        class MIST_API Semaphore {
        public:
            Semaphore(int _n);
            Semaphore(int _n, int _max);
            ~Semaphore();
            
            void wait();
            void set();
            
        private:
            volatile int n;
            int max;
            
            mutable Mutex mutex;
            Condition cond;
        };
        
        template<typename T>
        class BlockingQueue: Uncopyable {
        public:
            BlockingQueue():
            mutex(),
            cond(mutex),
            queue() {
            }
            
            void put(const T& x) {
                MutexGuard<Mutex> lock(mutex);
                queue.push_back(x);
                cond.notify();
            }
            
            T take() {
                while(queue.empty())  
                    cond.wait();
                
                MutexGuard<Mutex> lock(mutex);
                assert(!queue.empty());
                T front(queue.front());
                queue.pop_front();
                return front;
            }
            
            size_t size() const {
                MutexGuard<Mutex> lock(mutex);
                return queue.size();
            }
            
        private:
            mutable Mutex mutex;
            Condition cond;
            
            std::deque<T> queue;
        };
                
        class MIST_API CountDownLatch: Uncopyable {
        public:
            explicit CountDownLatch(int32 _count):
            mutex(),
            condition(mutex),
            count(_count) {
                
            }
            
            void wait() {
                MutexGuard<Mutex> lock(mutex);
                while(count > 0) {
                    condition.wait();
                }
            }
            
            void countDown() {
                MutexGuard<Mutex> lock(mutex);
                --count;
                if(count == 0)
                    condition.notifyAll();
            }
            
            int32 getCount() const {
                MutexGuard<Mutex> lock(mutex);
                return count;
            }
            
        private:
            mutable Mutex mutex;
            Condition condition;
            int32 count;
        };
        
        class MIST_API RWLock {
        public:
            RWLock();
            ~RWLock();
            
            void readLock();
            void writeLock();
            void unlock();
                
        private:
            bool isValid() const;            
            void addWriter();
            
            void removeWriter();
                
#ifdef MIST_OS_WINDOWS
            HANDLE mutex;
            HANDLE readEvent;
            HANDLE writeEvent;
            unsigned readers;
            unsigned writersWaiting;
            unsigned writers;
#elif defined(MIST_OS_FAMILY_UNIX)
            pthread_rwlock_t rwl;
#endif
        };
        
        class MIST_API ThreadPool: Uncopyable {
        public:
            ThreadPool();
            ~ThreadPool();
            
            void start(uint32 numThreads);
            void join();
            
            void run(const ThreadTask&);
            
            bool isRunning() const;
            
            static ThreadPool& DefaultObject();
            
        private:
            void runInThread(void* arg);
            
            ThreadTask take();
            
            Mutex mMutex;
            Condition mCond;
            
            typedef std::vector<Thread*> ThreadList;
            ThreadList mThreads;
            
            typedef std::queue<ThreadTask> ThreadTaskQueue;
            ThreadTaskQueue mTasks;
            
            bool mRunning;
        };
        
        
        class MIST_API ThreadTaskPool: Uncopyable {
        public:
            ThreadTaskPool();
            ~ThreadTaskPool();
            
            void add(const ThreadTask&);
            
            /* run all in current thread */
            void run();
            
            static ThreadTaskPool& DefaultObject();
            
        private:
            Mutex mMutex;
            typedef std::queue<ThreadTask> ThreadTaskQueue;
            ThreadTaskQueue mTasks;
        };
        
        template<typename _R>
        struct RTask {
#if defined(MIST_CPP11) && defined(MIST_VARIADIC_TEMPLATE)
            
            template<typename _Fp, typename ...ARGS>
            RTask(_Fp fp, ARGS... args) {
                task = ThreadTask([this, fp, args...]() {
                    result = fp(args...);
                });
            }
            
#endif
            explicit RTask(const std::function<_R()>& f):
            fp(f) {
                task = ThreadTask(Bind(this, &RTask<_R>::execute), (void*)0);
            }
            
            void execute(void* args) {
                printf("tett");
                this->result = fp();
            }
            
            explicit RTask(const ThreadTask& task):
            task(task) {
                
            }
            
            std::function<_R()> fp;
            ThreadTask task;
            _R result;
        };
        
        template<>
        struct RTask<void> {
#if defined(MIST_CPP11) && defined(MIST_VARIADIC_TEMPLATE)
            
            template<typename _Fp, typename ...ARGS>
            RTask(_Fp fp, ARGS... args) {
                task = ThreadTask([=]() {
                    fp(args...);
                });
            }
            
#endif
            explicit RTask(const std::function<void()>& f):
            fp(f) {
                task = ThreadTask(Bind(this, &RTask<void>::execute), (void*)0);
            }
            
            void execute(void* args) {
                fp();
            }
            
            explicit RTask(const ThreadTask& task):
            task(task) {
                
            }
            
            std::function<void()> fp;
            ThreadTask task;
        };
        
        template<typename _T>
        class Future {
        public:
            Future(SharedPtr<RTask<_T> > task):
            mTask(task) {
                mThread = new Thread(task->task);
            }
            
            void wait() {
                while(true) {
                    if(!mThread->isActive())
                        break;
                    mist::msleep(1);
                }
            }
            
            bool isFinished() const {
                return mThread->isActive();
            }
            
            _T getResult() const {
                return mTask->result;
            }
            
        private:
            SharedPtr<RTask<_T> > mTask;
            SharedPtr<Thread> mThread;
        };
        
        template<typename _T>
        Future<_T> RunAsync(std::function<_T()> f) {
            return Future<_T>(new RTask<_T>(f));
        }
        
        static void RunInMainThread(const mist::thread::ThreadTask&);

    } // namespace thread
    
} // namespace mist

#endif
