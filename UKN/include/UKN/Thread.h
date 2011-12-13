//
//  Thread.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/13/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Thread_h
#define Project_Unknown_Thread_h

#include "UKN/Platform.h"
#include "UKN/Uncopyable.h"

#ifndef UKN_OS_WINDOWS
#include <pthread.h>
#else
#include <Window.h>
#endif // UKN_OS_WINDOWS

#include "UKN/Exception.h"

#include <deque>

namespace ukn {
    
    namespace thread {
        
        class Mutex {
        public:
            Mutex() {
#ifdef UKN_OS_WINDOWS
                InitializeCriticalSectionAndSpinCount(&cs, 4000);
#else
                pthread_mutex_init(&mutex, NULL);
#endif
            }
            
            ~Mutex() {
#ifdef UKN_OS_WINDOWS
                DeleteCriticalSection(&cs);
#else
                pthread_mutex_destroy(&mutex);
#endif
            }
            
            void lock() {
#ifdef UKN_OS_WINDOWS
                try {
                    EnterCriticalSection(&cs);
                } catch(...) {
                    UKN_THROW_EXCEPTION("ukn::Thread::Mutex: Cannot lock mutex");
                }
#else
                if(pthread_mutex_lock(&mutex))
                    UKN_THROW_EXCEPTION("ukn::Thread::Mutex: Cannot lock mutex");
#endif
            }
            
            void unlock() {
#ifdef UKN_OS_WINDOWS
                try {
                    LeaveCriticalSection(&cs);
                } catch(...) {
                    UKN_THROW_EXCEPTION("ukn::Thread::Mutex: Cannot unlock mutex");
                }
#else
                if(pthread_mutex_unlock(&mutex))
                    UKN_THROW_EXCEPTION("ukn::Thread::Mutex: Cannot unlock mutex");
#endif
            }
            
            void* getSysMutex() {
#ifdef UKN_OS_WINDOWS
                return &cs;
#else
                return &mutex;
#endif
            }
            
        private:
#ifdef UKN_OS_WINDOWS
            CRITICAL_SECTION cs;
#else
            pthread_mutex_t mutex;
#endif
            
        };
        
        class MutexGuard: Uncopyable {
        public:
            explicit MutexGuard(Mutex& mutex): _mutex(mutex) {
                _mutex.lock();
            }
            
            ~MutexGuard() {
                _mutex.unlock();
            }
            
        private:
            Mutex& _mutex;
        };
        
#ifdef UKN_OS_WINDOWS
        typedef struct {
            int waiters_count_;
            // Number of waiting threads.
            
            CRITICAL_SECTION waiters_count_lock_;
            // Serialize access to <waiters_count_>.
            
            HANDLE sema_;
            // Semaphore used to queue up threads waiting for the condition to
            // become signaled. 
            
            HANDLE waiters_done_;
            // An auto-reset event used by the broadcast/signal thread to wait
            // for all the waiting thread(s) to wake up and be released from the
            // semaphore. 
            
            size_t was_broadcast_;
            // Keeps track of whether we were broadcasting or signaling.  This
            // allows us to optimize the code if we're just signaling.
        } pthread_cond_t;
        
        typedef HANDLE pthread_mutex_t;
        
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
        
        static void _win32_pthread_cont_destroy(pthread_cont_t *cv) {
            CloseHandle(cv->waiters_done_);
            DeleteCriticalSectionA(cv->waiters_count_lock_);
        }
        
        static int
        _win32_pthread_cond_wait (pthread_cond_t *cv, 
                                  pthread_mutex_t *external_mutex) {
            // Avoid race conditions.
            EnterCriticalSection (&cv->waiters_count_lock_);
            cv->waiters_count_++;
            LeaveCriticalSection (&cv->waiters_count_lock_);
            
            // This call atomically releases the mutex and waits on the
            // semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
            // are called by another thread.
            SignalObjectAndWait (*external_mutex, cv->sema_, INFINITE, FALSE);
            
            // Reacquire lock to avoid race conditions.
            EnterCriticalSection (&cv->waiters_count_lock_);
            
            // We're no longer waiting...
            cv->waiters_count_--;
            
            // Check to see if we're the last waiter after <pthread_cond_broadcast>.
            int last_waiter = cv->was_broadcast_ && cv->waiters_count_ == 0;
            
            LeaveCriticalSection (&cv->waiters_count_lock_);
            
            // If we're the last waiter thread during this particular broadcast
            // then let all the other threads proceed.
            if (last_waiter)
                // This call atomically signals the <waiters_done_> event and waits until
                // it can acquire the <external_mutex>.  This is required to ensure fairness. 
                SignalObjectAndWait (cv->waiters_done_, *external_mutex, INFINITE, FALSE);
            else
                // Always regain the external mutex since that's the guarantee we
                // give to our callers. 
                WaitForSingleObject (*external_mutex, INFINITE);
            return 0;
        }
        
        static int
        _win32_pthread_cond_signal (pthread_cond_t *cv) {
            EnterCriticalSection (&cv->waiters_count_lock_);
            int have_waiters = cv->waiters_count_ > 0;
            LeaveCriticalSection (&cv->waiters_count_lock_);
            
            // If there aren't any waiters, then this is a no-op.  
            if (have_waiters)
                ReleaseSemaphore (cv->sema_, 1, 0);
            return 0;
        }
        
        static int
        _win32_pthread_cond_broadcast (pthread_cond_t *cv) {
            // This is needed to ensure that <waiters_count_> and <was_broadcast_> are
            // consistent relative to each other.
            EnterCriticalSection (&cv->waiters_count_lock_);
            int have_waiters = 0;
            
            if (cv->waiters_count_ > 0) {
                // We are broadcasting, even if there is just one waiter...
                // Record that we are broadcasting, which helps optimize
                // <pthread_cond_wait> for the non-broadcast case.
                cv->was_broadcast_ = 1;
                have_waiters = 1;
            }
            
            if (have_waiters) {
                // Wake up all the waiters atomically.
                ReleaseSemaphore (cv->sema_, cv->waiters_count_, 0);
                
                LeaveCriticalSection (&cv->waiters_count_lock_);
                
                // Wait for all the awakened threads to acquire the counting
                // semaphore. 
                WaitForSingleObject (cv->waiters_done_, INFINITE);
                // This assignment is okay, even without the <waiters_count_lock_> held 
                // because no other waiter threads can wake up to access it.
                cv->was_broadcast_ = 0;
            }
            else
                LeaveCriticalSection (&cv->waiters_count_lock_);
            return 0;
        }
#endif // UKN_OS_WINDOWS
        
        class Condition: Uncopyable {
        public:
            explicit Condition(Mutex& mutex):
            _mutex(mutex) {
#ifdef UKN_OS_WINDOWS
                _win32_pthread_cond_init(&cond, NULL);
#else
                pthread_cond_init(&cond, NULL);
#endif
            }
            
            ~Condition() {
#if UKN_OS_WINDOWS
                _win32_pthread_cond_destroy(&cond);
#else
                pthread_cond_destroy(&cond);
#endif
            }
            
            void wait() {
#ifndef UKN_OS_WINDOWS
                if(pthread_cond_wait(&cond,
                                     static_cast<pthread_mutex_t*>(_mutex.getSysMutex())))
                    UKN_THROW_EXCEPTION("ukn::Thread::Condition: error wait");
#else
                if(_win32_pthread_cond_wait(&cond,
                                            static_cast<pthread_mutex_t*>(_mutex.getSysMutex())))
                    UKN_THROW_EXCEPTION("ukn::Thread::Condition: error wait");
#endif
            }
            
            void notify() {
#ifndef UKN_OS_WINDOWS
                if(pthread_cond_signal(&cond))
                    UKN_THROW_EXCEPTION("ukn::Thread::Condition: error notify");
#else
                if(_win32_pthread_cond_signal(&cond))
                    UKN_THROW_EXCEPTION("ukn::Thread::Condition: error notify");
#endif
            }
            
            void notifyAll() {
#ifndef UKN_OS_WINDOWS
                if(pthread_cond_broadcast(&cond))
                    UKN_THROW_EXCEPTION("ukn::Thread::Condition: error broadcast");
#else
                if(_win32_pthread_cond_broadcast(&cond))
                    UKN_THROW_EXCEPTION("ukn::Thread::Condition: error broadcast");
#endif
            }
            
        private:
            pthread_cond_t cond;
            
        protected:
            Mutex& _mutex;
        };
        
        
        class Semaphore {
            Semaphore(int _n)
#ifndef UKN_OS_WINDOWS
            :
            mutex(),
            cond(mutex),
            n(_n),
            max(_n) {     
#endif
                
#ifdef UKN_OS_WINDOWS             
            {
                sema = CreateSemaphoreW(NULL, n, n, NULL);
#endif
            }
            
            Semaphore(int _n, int _max)
#ifndef UKN_OS_WINDOWS
            :
            mutex(),
            cond(mutex),
            n(_n),
            max(_max) {
                assert( n >= 0 && max > 0 && n <= max);
#endif
                
#ifdef UKN_OS_WINDOWS
            {
                assert( n>=0 && max>0 && n<=max);
                
                sema = CreateSemaphoreW(NULL, n, max, NULL);
#endif
            }
            
            ~Semaphore() { 
#ifdef UKN_OS_WINDOWS
                CloseHandle(sema);
#else
                
#endif
            }
            
            void wait() {
#ifdef UKN_OS_WINDOWS
                switch(WaitForSingleObject(sema,INFINITE)) {
                    case WAIT_OBJECT_0:
                        return;
                    default:
                        UKN_THROW_EXCEPTION("ukn::Thread::Semaphore: error wait");
                }
#else
                MutexGuard lock(mutex);
                while(n < 1) {
                    cond.wait();
                }
                --n;
#endif
            }
            
            void set() {
#ifdef UKN_OS_WINDOWS
                if(!ReleaseSemaphore(sema, 1, NULL)) {
                    UKN_THROW_EXCEPTION("ukn::Thread::Semaphore: error set");
                }
#else
                MutexGuard lock(mutex);
                if(n < max) {
                    ++n;
                } else {
                    UKN_THROW_EXCEPTION("ukn::Thread::Semaphore: Cannot signal semaphore, exceed maximun");
                }
                cond.notify();
#endif
            }
            
        private:
#ifdef UKN_OS_WINDOWS
            HANDLE sema;
#else
            volatile int n;
            int max;
            
            mutable Mutex mutex;
            Condition cond;
#endif
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
                MutexGuard lock(mutex);
                queue.push_back(x);
                cond.notify();
            }
            
            T take() {
                while(queue.empty())  
                    cond.wait();
                
                MutexGuard lock(mutex);
                assert(!queue.empty());
                T front(queue.front());
                queue.pop_front();
                return front;
            }
            
            size_t size() const {
                MutexGuard lock(mutex);
                return queue.size();
            }
            
        private:
            mutable Mutex mutex;
            Condition cond;
            
            std::deque<T> queue;
        };
        
        class CountDownLatch: Uncopyable {
        public:
            explicit CountDownLatch(int32 _count):
            mutex(),
            condition(mutex),
            count(_count) {
                
            }
            
            void wait() {
                MutexGuard lock(mutex);
                while(count > 0) {
                    condition.wait();
                }
            }
            
            void countDown() {
                MutexGuard lock(mutex);
                --count;
                if(count == 0)
                    condition.notifyAll();
            }
            
            int32 getCount() const {
                MutexGuard lock(mutex);
                return count;
            }
            
        private:
            mutable Mutex mutex;
            Condition condition;
            int32 count;
        };
        
        class RWLock {
            RWLock() {
#ifdef UKN_OS_WINDOWS
            :
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
#endif
                
#ifndef UKN_OS_WINDOWS
                pthread_rwlock_init(&rwl, NULL);
#endif          
            }
            
            ~RWLock() {
#ifdef UKN_OS_WINDOWS
                CloseHandle(mutex);
                CloseHandle(readEvent);
                Closehandle(writeEvent);
#else
                pthread_rwlock_destroy(&rwl);
  
#endif
            }
            
            void readLock() {
#ifdef UKN_OS_WINDOWS
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
                        THROW_SORA_EXCEPTION(RuntimeException, "Cannot lock reader/writer lock");
                }
#else
                if(pthread_rwlock_rdlock(&rwl))
                    UKN_THROW_EXCEPTION("ukn::Thread::RWLock: Cannot lock reader/writer lock");
#endif
            }
            
            void writeLock() {
#ifdef UKN_OS_WINDOWS
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
                        THROW_SORA_EXCEPTION(RuntimeException, "Cannot lock reader/writer lock");
                }
#else
                if(pthread_rwlock_wrlock(&rwl))
                    UKN_THROW_EXCEPTION("ukn::Thread::RWLock: Cannot lock reader/writer lock");
#endif     
            }
            
            void unlock() {
#ifdef UKN_OS_WINDOWS
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
                        THROW_SORA_EXCEPTION(RuntimeException, "Cannot unlock reader/writer lock");
                }
#else
                if(pthread_rwlock_unlock(&rwl))
                    UKN_THROW_EXCEPTION("ukn::Thread::RWLock: Cannot lock reader/writer lock");
#endif   
            }
            
        private:
#ifdef UKN_OS_WINDOWS
            inline bool isValid() const {
                return mutex && readEvent && writeEvent;
            }
            
            inline void addWriter() {
                switch(WaitForSingleObject(mutex, INFINITE)) {
                    case WAIT_OBJECT_0:
                        if(++writersWaiting == 1)
                            ResetEvent(readEvent);
                        ReleaseMutex(mutex);
                        break;
                    default:
                        THROW_SORA_EXCEPTION(RuntimeException, "Cannot lock reader/writer lock");
                }
            }
            
            inline void removeWriter() {
                switch(WaitForSingleObject(mutex, INFINITE)) {
                    case WAIT_OBJECT_0:
                        if(--writersWaiting == 0 && writes == 0)
                            SetEvent(readEvent);
                        ReleaseMutex(mutex);
                        break;
                    default:
                        THROW_SORA_EXCEPTION(RuntimeException, "Cannot lock reader/writers lock");
                }
            }
            
            HANDLE mutex;
            HANDLE readEvent;
            HANDLE writeEvent;
            unsigned readers;
            unsigned writersWaiting;
            unsigned writers;
#else
            pthread_rwlock_t rwl;
#endif
        };
        
    } // namespace thread
    
} // namespace ukn

#endif
