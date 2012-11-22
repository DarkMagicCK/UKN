//
//  FunctionTemplate.h
//  zeromq_client
//
//  Created by Robert Bu on 8/16/11.
//  Copyright 2011 heizi. All rights reserved.
//

// this is a template header replys on macro definitions
// must not have multiple-inclusion protection

#include "mist/Preprocessor.h"
#include "FunctionBase.h"
#include "FunctionFoward.h"

#include <cassert>

namespace mist {

#ifdef MIST_FUNCTION_NUM_ARGS

    namespace detail {

#define MIST_FUNCTION_TEMPLATE_PARAM MIST_ENUM_PARAMS(MIST_FUNCTION_NUM_ARGS, typename T)

#define MIST_FUNCTION_TEMPLATE_ARGS MIST_ENUM_PARAMS(MIST_FUNCTION_NUM_ARGS, T)

#define MIST_FUNCTION_PARAM(n, d) MIST_CAT(T, n) MIST_CAT(a, n)
#define MIST_FUNCTION_PARAMS MIST_ENUM_N(MIST_FUNCTION_NUM_ARGS, MIST_FUNCTION_PARAM, MIST_EMPTY)

#define MIST_FUNCTION_ARGS MIST_ENUM_PARAMS(MIST_FUNCTION_NUM_ARGS, a)

#define MIST_INVOKER MIST_JOIN(invoker, MIST_FUNCTION_NUM_ARGS)
#define MIST_OBJ_INVOKER MIST_JOIN(obj_invoker, MIST_FUNCTION_NUM_ARGS)

#define MIST_GET_INVOKER MIST_JOIN(get_function_invoker, MIST_FUNCTION_NUM_ARGS)
#define MIST_GET_OBJ_INVOKER MIST_JOIN(get_function_obj_invoker, MIST_FUNCTION_NUM_ARGS)

#define MIST_FUNCTION MIST_JOIN(function, MIST_FUNCTION_NUM_ARGS)

#define MIST_FUNCTOR_RELEASE MIST_JOIN(functor_release, MIST_FUNCTION_NUM_ARGS)
#define MIST_GET_FUNCTOR_RELEASE MIST_JOIN(get_functor_release, MIST_FUNCTION_NUM_ARGS)

#if MIST_FUNCTION_NUM_ARGS == 0
#define MIST_FUNCTION_COMMA
#else
#define MIST_FUNCTION_COMMA ,
#endif

        template<typename FunctionPtr, typename R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_PARAM>
        struct MIST_INVOKER {
            static R invoke(any_ptr ptr MIST_FUNCTION_COMMA MIST_FUNCTION_PARAMS) {
                FunctionPtr f = reinterpret_cast<FunctionPtr>(ptr.func_ptr);
                return f(MIST_FUNCTION_ARGS);
            }
        };

        template<typename FunctionObj, typename R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_PARAM>
        struct MIST_FUNCTOR_RELEASE {
            static void release(any_ptr ptr) {
                FunctionObj* f = reinterpret_cast<FunctionObj*>(ptr.obj_ptr);
                delete f;
            }
        };

        template<typename FunctionObj, typename R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_PARAM>
        struct MIST_OBJ_INVOKER {
            static R invoke(any_ptr ptr MIST_FUNCTION_COMMA MIST_FUNCTION_PARAMS) {
                FunctionObj* f = reinterpret_cast<FunctionObj*>(ptr.obj_ptr);
                return (*f)(MIST_FUNCTION_ARGS);
            }
        };

        template<typename FunctionPtr MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_PARAM>
        struct MIST_INVOKER<FunctionPtr, void MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS> {
            static void invoke(any_ptr ptr MIST_FUNCTION_COMMA MIST_FUNCTION_PARAMS) {
                FunctionPtr f = reinterpret_cast<FunctionPtr>(ptr.func_ptr);
                f(MIST_FUNCTION_ARGS);
            }
        };

        template<typename FunctionObj MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_PARAM>
        struct MIST_OBJ_INVOKER<FunctionObj, void MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS> {
            static void invoke(any_ptr ptr MIST_FUNCTION_COMMA MIST_FUNCTION_PARAMS) {
                FunctionObj* f = reinterpret_cast<FunctionObj*>(ptr.obj_ptr);
                (*f)(MIST_FUNCTION_ARGS);
            }
        };

        template<typename FunctionPtr, typename R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_PARAM>
        struct MIST_GET_INVOKER {
            typedef MIST_INVOKER<FunctionPtr, R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS> type;
        };

        template<typename FunctionPtr, typename R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_PARAM>
        struct MIST_GET_OBJ_INVOKER {
            typedef MIST_OBJ_INVOKER<FunctionPtr, R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS> type;
        };

        template<typename FunctionObj, typename R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_PARAM>
        struct MIST_GET_FUNCTOR_RELEASE {
            typedef MIST_FUNCTOR_RELEASE<FunctionObj, R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS> type;
        };

        template<typename R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_PARAM>
        class MIST_FUNCTION {
        public:
            typedef MIST_FUNCTION<R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS> self_type;

            template<typename Functor>
            MIST_FUNCTION(const Functor& functor):
            mInvoker(0),
            mReleaser(0),
            mRefCount(NULL) {
                this->assign_to(functor);
            }

            MIST_FUNCTION():
            mInvoker(0),
            mReleaser(0),
            mRefCount(NULL) {

            }

            virtual ~MIST_FUNCTION() {
                clear();
            }

            void clear() {
                if(mInvoker && mRefCount) {
                    --*mRefCount;
                    if(*mRefCount <= 0) {
                        delete mRefCount;

                        if(mReleaser)
                            mReleaser(mPtr);
                    }
                    mRefCount = NULL;
                    mReleaser = NULL;
                    mInvoker = NULL;
                }
            }

            template<typename Functor>
            void assign_to(Functor f) {
                clear();

                typedef typename get_function_tag<Functor>::type tag;
                this->assign_to(f, tag());
            }

            inline R operator()(MIST_FUNCTION_PARAMS) const {
                assert(mInvoker);

                return mInvoker(mPtr MIST_FUNCTION_COMMA MIST_FUNCTION_ARGS);
            }

            operator bool() const {
                return mInvoker != NULL;
            }
            
            template<typename F>
            bool operator== (F f) {
                typedef typename get_function_tag<F>::type tag;
                return this->equalTo(tag());
            }
            
            template<typename F>
            bool equalTo(F f, function_ptr_tag tag) {
                return this->mPtr.func_ptr == f;
            }
            
            template<typename F>
            bool equalTo(F f, member_ptr_tag tag) {
                return this->equalTo(MemFun(f), function_obj_tag());
            }
            
            template<typename F>
            bool equalTo(F f, function_obj_tag tag) {
                return *(F*)this->mPtr.obj_ptr == f;
            }

            typedef R (*invoker_type)(any_ptr MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS);
            typedef void (*releaser_type)(any_ptr);

        private:

            template<typename Functor>
            void assign_to(Functor f, function_ptr_tag tag) {
                if(f) {
                    typedef typename MIST_GET_INVOKER<Functor, R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS>::type invoker_type;

                    mInvoker = &invoker_type::invoke;
                    mPtr = make_any_pointer((void (*)())f);
                }
            }

            template<typename Functor>
            void assign_to(Functor f, function_obj_tag tag) {
                typedef typename MIST_GET_OBJ_INVOKER<Functor, R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS>::type invoker_type;
                typedef typename MIST_GET_FUNCTOR_RELEASE<Functor, R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS>::type releaser_type;

                mInvoker = &invoker_type::invoke;
                mReleaser = &releaser_type::release;

                mPtr = make_any_pointer(new Functor(f));
                mRefCount = new int;
                *mRefCount = 1;
            }

            template<typename Functor>
            void assign_to(Functor f, member_ptr_tag tag) {
                this->assign_to(MemFun(f));
            }

            template<typename Functor>
            void assign_to(Functor f, function_obj_ref_tag tag) {
                // ?
            }

        protected:
            any_ptr mPtr;
            invoker_type mInvoker;
            releaser_type mReleaser;
            int* mRefCount;
        };

        template<MIST_FUNCTION_TEMPLATE_PARAM>
        class MIST_FUNCTION<void MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS> {
        public:
            typedef MIST_FUNCTION<void MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS> self_type;

            template<typename Functor>
            MIST_FUNCTION(const Functor& functor):
            mInvoker(0),
            mReleaser(0),
            mRefCount(NULL) {
                this->assign_to(functor);
            }

            MIST_FUNCTION():
            mInvoker(0),
            mReleaser(0),
            mRefCount(NULL) {

            }

            virtual ~MIST_FUNCTION() {
                clear();
            }

            void clear() {
                if(mInvoker && mRefCount) {
                    --*mRefCount;
                    if(*mRefCount <= 0) {
                        delete mRefCount;

                        if(mReleaser)
                            mReleaser(mPtr);
                    }
                    mRefCount = NULL;
                    mReleaser = NULL;
                    mInvoker = NULL;
                }
            }

            template<typename Functor>
            void assign_to(Functor f) {
                clear();

                typedef typename get_function_tag<Functor>::type tag;
                this->assign_to(f, tag());
            }

            inline void operator()(MIST_FUNCTION_PARAMS) const {
                assert(mInvoker);

                mInvoker(mPtr MIST_FUNCTION_COMMA MIST_FUNCTION_ARGS);
            }

            operator bool() const {
                return mInvoker != NULL;
            }
            
            template<typename F>
            bool operator== (F f) {
                typedef typename get_function_tag<F>::type tag;
                return this->equalTo(f, tag());
            }
            
            template<typename F>
            bool equalTo(F f, function_ptr_tag tag) {
                return this->mPtr.func_ptr == f;
            }
            
            template<typename F>
            bool equalTo(F f, member_ptr_tag tag) {
                return this->equalTo(MakeMemFun(f), function_obj_tag());
            }
            
            template<typename F>
            bool equalTo(F f, function_obj_tag tag) {
                return *(F*)this->mPtr.obj_ptr == f;
            }

            typedef void (*invoker_type)(any_ptr MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS);
            typedef void (*releaser_type)(any_ptr);

        private:

            template<typename Functor>
            void assign_to(Functor f, function_ptr_tag tag) {
                if(f) {
                    typedef typename MIST_GET_INVOKER<Functor, void MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS>::type invoker_type;

                    mInvoker = &invoker_type::invoke;
                    mPtr = make_any_pointer((void (*)())f);
                }
            }

            template<typename Functor>
            void assign_to(Functor f, function_obj_tag tag) {
                typedef typename MIST_GET_OBJ_INVOKER<Functor, void MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS>::type invoker_type;
                typedef typename MIST_GET_FUNCTOR_RELEASE<Functor, void MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS>::type releaser_type;


                mInvoker = &invoker_type::invoke;
                mReleaser = &releaser_type::release;

                mPtr = make_any_pointer(new Functor(f));
                mRefCount = new int(1);
            }

            template<typename Functor>
            void assign_to(Functor f, member_ptr_tag tag) {
                this->assign_to(MakeMemFun(f));
            }

            template<typename Functor>
            void assign_to(Functor f, function_obj_ref_tag tag) {
                // ?
            }

        protected:
            any_ptr mPtr;
            invoker_type mInvoker;
            releaser_type mReleaser;
            int* mRefCount;
        };


    } // namespace detail


    template<typename R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_PARAM>
    class Function<R(MIST_FUNCTION_TEMPLATE_ARGS)>: public detail::MIST_FUNCTION<R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS>
#if MIST_FUNCTION_NUM_ARGS == 1
    , public std::unary_function<T0, R>
#elif MIST_FUNCTOR_NUM_ARGS == 2
    , public std::binary_function<T0, T1, R>
#endif
    {
    public:
        typedef detail::MIST_FUNCTION<R MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS> base_type;
        typedef Function<R(MIST_FUNCTION_TEMPLATE_ARGS)> self_type;
#if MIST_FUNCTION_NUM_ARGS == 1
        typedef T0 argument_type;
#elif MIST_FUNCTOR_NUM_ARGS == 2
        typedef T0 first_argument_type;
        typedef T1 second_argument_type;
#endif

        template<typename Functor>
        Function(Functor f):
        base_type(f) {

        }

        Function():
        base_type() {

        }

        template<typename Functor>
        Function<R(MIST_FUNCTION_TEMPLATE_ARGS)>& operator=(Functor f) {
            base_type::assign_to(f);
            return *this;
        }


        self_type& operator=(const self_type& rhs) {
            if(this != &rhs) {
                base_type::clear();

                this->mPtr = rhs.mPtr;
                this->mInvoker = rhs.mInvoker;
                this->mReleaser = rhs.mReleaser;
                this->mRefCount = rhs.mRefCount;

                if(this->mRefCount != NULL) {
                    ++*this->mRefCount;
                }
            }
            return *this;
        }

        Function(const self_type& rhs) {
            if(this != &rhs) {
                base_type::clear();

                this->mPtr = rhs.mPtr;
                this->mInvoker = rhs.mInvoker;
                this->mReleaser = rhs.mReleaser;
                this->mRefCount = rhs.mRefCount;

                if(this->mRefCount != NULL) {
                    ++*this->mRefCount;
                }
            }
        }
    };

    template<MIST_FUNCTION_TEMPLATE_PARAM>
    class Function<void(MIST_FUNCTION_TEMPLATE_ARGS)>: public detail::MIST_FUNCTION<void MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS>
#if MIST_FUNCTION_NUM_ARGS == 1
    , public std::unary_function<T0, void>
#elif MIST_FUNCTOR_NUM_ARGS == 2
    , public std::binary_function<T0, T1, void>
#endif
    {
    public:
        typedef detail::MIST_FUNCTION<void MIST_FUNCTION_COMMA MIST_FUNCTION_TEMPLATE_ARGS> base_type;
        typedef Function<void(MIST_FUNCTION_TEMPLATE_ARGS)> self_type;
#if MIST_FUNCTION_NUM_ARGS == 1
        typedef T0 argument_type;
#elif MIST_FUNCTOR_NUM_ARGS == 2
        typedef T0 first_argument_type;
        typedef T1 second_argument_type;
#endif

        template<typename Functor>
        Function(Functor f):
        base_type(f) {

        }

        Function():
        base_type() {

        }

        template<typename Functor>
        Function<void(MIST_FUNCTION_TEMPLATE_ARGS)>& operator=(Functor f) {
            base_type::assign_to(f);
            return *this;
        }


        self_type& operator=(const self_type& rhs) {
            if(this != &rhs) {
                base_type::clear();

                this->mPtr = rhs.mPtr;
                this->mInvoker = rhs.mInvoker;
                this->mReleaser = rhs.mReleaser;
                this->mRefCount = rhs.mRefCount;

                if(this->mRefCount != NULL) {
                    ++*this->mRefCount;
                }
            }
            return *this;
        }

        Function(const self_type& rhs) {
            if(this != &rhs) {
                base_type::clear();

                this->mPtr = rhs.mPtr;
                this->mInvoker = rhs.mInvoker;
                this->mReleaser = rhs.mReleaser;
                this->mRefCount = rhs.mRefCount;

                if(this->mRefCount != NULL) {
                    ++*this->mRefCount;
                }
            }
        }
    };

#undef MIST_FUNCTION_TEMPLATE_PARAM
#undef MIST_FUNCTION_TEMPLATE_ARGS
#undef MIST_FUNCTION_PARAMS
#undef MIST_FUNCTION_PARAM
#undef MIST_FUNCTION_ARGS
#undef MIST_INVOKER
#undef MIST_OBJ_INVOKER
#undef MIST_GET_INVOKER
#undef MIST_GET_OBJ_INVOKER
#undef MIST_FUNCTION_COMMA
#undef MIST_FUNCTION
#undef MIST_GET_FUNCTOR_RELEASE
#undef MIST_FUNCTOR_RELEASE

#endif // MIST_FUNCTION_NUM_ARGS


} // namespace sora
