#ifndef MISTInterfaces_H_
#define MISTInterfaces_H_

#include "mist/Basic.h"
#include "mist/Ptr.h"

namespace mist {

	template<typename T>
	class IEnumerator: public virtual Interface {
	public:
		virtual IEnumerator<T>* clone() const = 0;
		virtual const T& 		current() const = 0;
		virtual intPtr			index() const = 0;
		virtual bool			next() = 0;
		virtual bool			available() const = 0;
		virtual void			reset() = 0;
	};
	
	template<typename T>
	class IEnumerable: public virtual Interface {
	public:
		virtual IEnumerator<T>*	createEnumerator() const = 0;
	};
	
    class IResource: public virtual Interface {
    public:
        virtual const MistString& getName() const = 0;
    };
    
    template<typename T>
    class ISerializable: public virtual Interface {
        
    };
    
} // namespace mist

#endif