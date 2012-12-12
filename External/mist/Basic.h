#ifndef MISTBasic_H_
#define MISTBasic_H_

#include "mist/Uncopyable.h"
#include "mist/Platform.h"

namespace mist {

	class Interface {
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
   

} // namespace mist

#endif