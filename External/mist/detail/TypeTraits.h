//
//  TypeTraits.h
//  zeromq_client
//
//  Created by Robert Bu on 8/15/11.
//  Copyright 2011 heizi. All rights reserved.
//

#ifndef ukn_TypeTraits_h
#define ukn_TypeTraits_h

namespace mist {
    
    namespace traits {
        template<bool cont, typename T1, typename T2>
        struct selector {
            typedef T1 type;
        };
        
        template<typename T1, typename T2>
        struct selector<false, T1, T2> {
            typedef T2 type;
        };
        
        template<typename T>
        struct is_void {
            static const bool value = false;
        };
        
        template<>
        struct is_void<void> {
            static const bool value = true;
        };
        
        template<typename T>
        struct is_pointer {
            static const bool value = false;
        };
        
        template<typename T>
        struct is_pointer<T*> {
            static const bool value = true;
        };
        
        template<typename T>
        struct strip_pointer {
            typedef T type;
        };
        
        template<typename T>
        struct strip_pointer<T*> {
            typedef T type;
        };
        
        template<typename T>
        struct strip_qualifier {
            typedef T type;
        };
        
        template<typename T>
        struct strip_qualifier<T*> {
            typedef T type;
        };
        
        template<typename T>
        struct strip_qualifier<T&> {
            typedef T type;
        };
        
        template<typename T>
        struct strip_qualifier<const T&> {
            typedef T type;
        };
        
        // helper template to seperate array type and size
        template<typename T>
        struct strip_array;
        
        template<typename T, size_t LENGTH>
        struct strip_array<T[LENGTH]> {
            typedef T type;
            
            enum { Length = LENGTH };
        };
        
        template<typename T>
        struct is_carray {
            static const bool value = false;
        };
    
        template<typename T, size_t LENGTH>
        struct is_carray<T[LENGTH]> {
            static const bool value = true;
        };
        
        template<typename T>
        struct is_member_pointer {
            static const bool value = false;
        };
        
        template<typename T, typename R>
        struct is_member_pointer<R T::*> {
            static const bool value = true;
        };
        
        template<typename T, typename R>
        struct is_member_pointer<R T::* const> {
            static const bool value = true;
        };
        
        template<typename T>
        struct is_reference_wrapper {
            static const bool value = false;
        };
        
        template<typename T>
        struct is_reference_wrapper<T&> {
            static const bool value = true;
        };
        
        template<bool C, typename T1, typename T2>
        struct if_c {
            typedef T1 type;
        };
        
        template<typename T1, typename T2>
        struct if_c<false,T1,T2> {
            typedef T2 type;
        };  
        
        
        template<typename T>
        struct is_pod {
            enum { value = false };
        };
        /*
        template<typename T>
        struct function_trait {
            typedef typename function_trait<decltype(&T::operator())>::result_type result_type;
        };
        */
        template<typename T>
        struct function_trait;
        
        template<typename R>
        struct function_trait<R()> {
            typedef R result_type;
            static const int arity = 0;
        };
        
        template<typename R, typename A1>
        struct function_trait<R(A1)> {
            typedef R result_type;
            static const int arity = 1;
            
            typedef A1 arg1_type;
        };
        
        template<typename R, typename A1, typename A2>
        struct function_trait<R(A1, A2)> {
            typedef R result_type;
            static const int arity = 2;
            
            typedef A1 arg1_type;
            typedef A2 arg2_type;
        };
        
        template<typename R, typename A1, typename A2, typename A3>
        struct function_trait<R(A1, A2, A3)> {
            typedef R result_type;
            static const int arity = 3;
            
            typedef A1 arg1_type;
            typedef A2 arg2_type;
            typedef A3 arg3_type;
        };
        
        template<typename R, typename A1, typename A2, typename A3, typename A4>
        struct function_trait<R(A1, A2, A3, A4)> {
            typedef R result_type;
            static const int arity = 4;
        
            typedef A1 arg1_type;
            typedef A2 arg2_type;
            typedef A3 arg3_type;
            typedef A4 arg4_type;
        };
        
        template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        struct function_trait<R(A1, A2, A3, A4, A5)> {
            typedef R result_type;
            static const int arity = 5;

            typedef A1 arg1_type;
            typedef A2 arg2_type;
            typedef A3 arg3_type;
            typedef A4 arg4_type;
            typedef A5 arg5_type;
        };
        
        template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        struct function_trait<R(A1, A2, A3, A4, A5, A6)> {
            typedef R result_type;
            static const int arity = 6;

            typedef A1 arg1_type;
            typedef A2 arg2_type;
            typedef A3 arg3_type;
            typedef A4 arg4_type;
            typedef A5 arg5_type;
            typedef A6 arg6_type;
        };
        
        template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        struct function_trait<R(A1, A2, A3, A4, A5, A6, A7)> {
            typedef R result_type;
            static const int arity = 7;

            typedef A1 arg1_type;
            typedef A2 arg2_type;
            typedef A3 arg3_type;
            typedef A4 arg4_type;
            typedef A5 arg5_type;
            typedef A6 arg6_type;
            typedef A7 arg7_type;
        };
        
        template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        struct function_trait<R(A1, A2, A3, A4, A5, A6, A7, A8)> {
            typedef R result_type;
            static const int arity = 8;

            typedef A1 arg1_type;
            typedef A2 arg2_type;
            typedef A3 arg3_type;
            typedef A4 arg4_type;
            typedef A5 arg5_type;
            typedef A6 arg6_type;
            typedef A7 arg7_type;
            typedef A8 arg8_type;
        };
        
        template<> struct is_pod<int32>         { enum { value = true }; };
        template<> struct is_pod<uint32>        { enum { value = true }; };
        template<> struct is_pod<intPtr>        { enum { value = true }; };
        template<> struct is_pod<uintPtr>       { enum { value = true }; };
        template<> struct is_pod<uint8>         { enum { value = true }; };
        template<> struct is_pod<uint16>        { enum { value = true }; };
        template<> struct is_pod<int8>          { enum { value = true }; };
        template<> struct is_pod<int16>         { enum { value = true }; };
        template<> struct is_pod<uint64>        { enum { value = true }; };
        template<> struct is_pod<int64>         { enum { value = true }; };
        template<> struct is_pod<char>          { enum { value = true }; };
        template<> struct is_pod<wchar_t>       { enum { value = true }; };
        template<typename T> struct is_pod<T*>  { enum { value = true }; };
        template<typename T> struct is_pod<T&>  { enum { value = true }; };
        
        template<typename T, intPtr size> struct is_pod<T[size]>    { enum { value = is_pod<T>::value }; };
        template<typename T, typename C> struct is_pod<T C::*>      { enum { value = true }; };
        template<typename T> struct is_pod<const T>                 { enum { value = is_pod<T>::value }; };
        template<typename T> struct is_pod<volatile T>              { enum { value = is_pod<T>::value }; };
    } // namespace traits
    
} // namespace mist

#endif
