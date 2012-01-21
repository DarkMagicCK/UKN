#ifndef Project_Unknown_Basic_H_
#define Project_Unknown_Basic_H_

#include "UKN/Uncopyable.h"
#include "UKN/Platform.h"

namespace ukn {

	class Interface: private Uncopyable {
	public:
		virtual ~Interface() { }
	};
	
	class Object {
	public:
		virtual ~Object() { }
	};
	
	template<typename T, size_t minSize>
	union BinaryRetriever {
		T 		data;
		uint8 	binary[sizeof(T)>minSize?sizeof(T):minSize]; 
	};
    
    template<typename T>
    struct POD {
        enum { Value = false };
    };
    
    template<> struct POD<int32> { enum { Value = true }; };
    template<> struct POD<uint32> { enum { Value = true }; };
    template<> struct POD<intPtr> { enum { Value = true }; };
    template<> struct POD<uintPtr> { enum { Value = true }; };
    template<> struct POD<uint8> { enum { Value = true }; };
    template<> struct POD<uint16> { enum { Value = true }; };
    template<> struct POD<int8> { enum { Value = true }; };
    template<> struct POD<int16> { enum { Value = true }; };
    template<> struct POD<uint64> { enum { Value = true }; };
    template<> struct POD<int64> { enum { Value = true }; };
    template<> struct POD<char> { enum { Value = true }; };
    template<> struct POD<wchar_t> { enum { Value = true }; };
    template<typename T> struct POD<T*> { enum { Value = true }; };
    template<typename T> struct POD<T&> { enum { Value = true }; };
    template<typename T, intPtr size> struct POD<T[size]> { enum { Value = POD<T>::Value }; };
    template<typename T, typename C> struct POD<T C::*> { enum { Value = true }; };
    template<typename T> struct POD<const T> { enum { Value = POD<T>::Value }; };
    template<typename T> struct POD<volatile T> { enum { Value = POD<T>::Value }; };

} // namespace ukn

#endif