//
//  Preprocessor.h
//  Project Unknown
//
//  Created by Robert Bu on 11/21/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Preprocessor_h
#define Project_Unknown_Preprocessor_h

namespace ukn {
    template<bool x> struct STATIC_ASSERTION_FAILURE;
    template<> struct STATIC_ASSERTION_FAILURE<true> { enum {Value = 1}; };
    template<int x> struct static_assert_test{};
}

#define UKN_JOIN(x, y) UKN_DO_JOIN(x, y)
#define UKN_DO_JOIN(x ,y) UKN_DO_JOIN_2(x, y)
#define UKN_DO_JOIN_2(x, y) x##y

#define ukn_static_assert(B) \
typedef ::ukn::static_assert_test<\
    sizeof(::ukn::STATIC_ASSERTION_FAILURE<(bool)(B)>)>\
        UKN_JOIN(__ukn_static_assert_typedef_, __LINE__)

#define UKN_UNIQUE_NAME(name) \
    UKN_JOIN(name, __LINE__)

#define UKN_STATIC_RUN_CODE_I(name, code) \
    namespace { \
        static struct name { \
            name() { \
                code; \
            } \
        } UKN_JOIN(g_, name); \
    }

#define UKN_STATIC_RUN_CODE(code) \
    UKN_STATIC_RUN_CODE_I(UKN_UNIQUE_NAME(UKN_static_), code) 

#define UKN_STATIC_CALL_FUNC_I(name, FN) \
    namespace { \
        static struct name { \
            name() { \
            static int counter = 0; \
            if(counter++ > 0) return; \
                FN(); \
            } \
        } g_##name; \
    }

#define UKN_STATIC_CALL_FUNC(FN) \
    UKN_STATIC_CALL_FUNC_I(UKN_static_init_##FN, FN)

#define UKN_REPEAT_1_0(m, d)
#define UKN_REPEAT_1_1(m, d) m(0, d)
#define UKN_REPEAT_1_2(m, d) UKN_REPEAT_1_1(m, d) m(1, d)
#define UKN_REPEAT_1_3(m, d) UKN_REPEAT_1_2(m, d) m(2, d)
#define UKN_REPEAT_1_4(m, d) UKN_REPEAT_1_3(m, d) m(3, d)
#define UKN_REPEAT_1_5(m, d) UKN_REPEAT_1_4(m, d) m(4, d)
#define UKN_REPEAT_1_6(m, d) UKN_REPEAT_1_5(m, d) m(5, d)
#define UKN_REPEAT_1_7(m, d) UKN_REPEAT_1_6(m, d) m(6, d)
#define UKN_REPEAT_1_8(m, d) UKN_REPEAT_1_7(m, d) m(7, d)
#define UKN_REPEAT_1_9(m, d) UKN_REPEAT_1_8(m, d) m(8, d)
#define UKN_REPEAT_1_10(m, d) UKN_REPEAT_1_9(m, d) m(9, d)
#define UKN_REPEAT_1_11(m, d) UKN_REPEAT_1_10(m, d) m(10, d)
#define UKN_REPEAT_1_12(m, d) UKN_REPEAT_1_11(m, d) m(11, d)

#define UKN_REPEAT_2_0(m, d, d2) 
#define UKN_REPEAT_2_1(m, d, d2) m(0, d, d2)
#define UKN_REPEAT_2_2(m, d, d2) UKN_REPEAT_2_1(m, d, d2) m(1, d, d2)
#define UKN_REPEAT_2_3(m, d, d2) UKN_REPEAT_2_2(m, d, d2) m(2, d, d2)
#define UKN_REPEAT_2_4(m, d, d2) UKN_REPEAT_2_3(m, d, d2) m(3, d, d2)
#define UKN_REPEAT_2_5(m, d, d2) UKN_REPEAT_2_4(m, d, d2) m(4, d, d2)
#define UKN_REPEAT_2_6(m, d, d2) UKN_REPEAT_2_5(m, d, d2) m(5, d, d2)
#define UKN_REPEAT_2_7(m, d, d2) UKN_REPEAT_2_6(m, d, d2) m(6, d, d2)
#define UKN_REPEAT_2_8(m, d, d2) UKN_REPEAT_2_7(m, d, d2) m(7, d, d2)
#define UKN_REPEAT_2_9(m, d, d2) UKN_REPEAT_2_8(m, d, d2) m(8, d, d2)
#define UKN_REPEAT_2_10(m, d, d2) UKN_REPEAT_2_9(m, d, d2) m(9, d, d2)
#define UKN_REPEAT_2_11(m, d, d2) UKN_REPEAT_2_10(m, d, d2) m(10, d, d2)
#define UKN_REPEAT_2_12(m, d, d2) UKN_REPEAT_2_11(m, d, d2) m(11, d, d2)

// would expanded to m([0-(n-1)], d)
// m should be a macro provided by user, receiving two params
// for example
//      #define expand(n, d) d##n = n;
//      UKN_REPEAT_N(4, expand, myVar)
// would be expanded to
// myVar0 = 0; myVar1 = 1; myVar2 = 2; myVar3 = 3;
// n max = 12
#define UKN_REPEAT_N(n, m, d) UKN_REPEAT_1_##n(m, d)
#define UKN_REPEAT_N_2(n, m, d, d2) UKN_REPEAT_2_##n(m, d, d2)

#define UKN_ENUM_N(n, m, d) UKN_REPEAT_N_2(n, UKN_ENUM_M, m, d)

#define UKN_ENUM_M(n, m, d) UKN_COMMA_IF(n) m(n, d)

// function param repeat definition macro
// would expanded to d##0, d##1, d##2, d##n-1
// n max = 12
#define UKN_ENUM_PARAMS(n, d) UKN_REPEAT_N(n, UKN_FUNCTION_M, d)

#define UKN_PARAM_REPEAT(n, m, d)

#define UKN_BOOL_0 0
#define UKN_BOOL_1 1
#define UKN_BOOL_2 1
#define UKN_BOOL_3 1
#define UKN_BOOL_4 1
#define UKN_BOOL_5 1
#define UKN_BOOL_6 1
#define UKN_BOOL_7 1
#define UKN_BOOL_8 1
#define UKN_BOOL_9 1
#define UKN_BOOL_10 1
#define UKN_BOOL_11 1
#define UKN_BOOL_12 1
#define UKN_BOOL_13 1
#define UKN_BOOL_14 1
#define UKN_BOOL_15 1
#define UKN_BOOL_16 1
#define UKN_BOOL_17 1
#define UKN_BOOL_18 1
#define UKN_BOOL_19 1
#define UKN_BOOL_20 1

#define UKN_COMMA() ,
#define UKN_EMPTY() 

#define UKN_IF_IIF_0(t, f) t
#define UKN_IF_IIF_1(t, f) f

#define UKN_BOOL_N(n) UKN_BOOL_##n

#define UKN_IIF_I(cond, t, f) UKN_IF_IIF_##cond(t, f)
#define UKN_IIF(cond, t, f) UKN_IIF_I(cond, t, f)

// condition select macro
// cond must be a number >= 0
// if cond = 0, then t is used
// otherwise f is used
#define UKN_IF(cond, t, f) UKN_IIF(UKN_BOOL_N(cond), t, f)

#define UKN_COMMA_IF(n) UKN_IF(n, UKN_EMPTY, UKN_COMMA)()

#define UKN_FUNCTION_M(n, d) UKN_COMMA_IF(n) d ## n

// cat
#define UKN_CAT(a, b) UKN_CAT_I(a, b)
#define UKN_CAT_I(a, b) a##b

#endif // Project_Unknown_Preprocessor_h
