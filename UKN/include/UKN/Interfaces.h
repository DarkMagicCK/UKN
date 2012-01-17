#include "UKN/Basic.h"

namespace ukn {

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
	
	

} // namespace ukn