#include "mist/Basic.h"
#include "mist/Ptr.h"

#include "mist/detail/TypeTraits.h"

namespace mist {
	
	template<typename T>
	class ForEachIterator: public Object {
	public:
		virtual bool      available() const = 0;
		virtual void      next() const = 0;
        virtual const T&  current() const = 0;
		
		operator bool() const {
			return true;
		}
	};
	
	template<typename T>
	class EnumerableForEachIterator: public ForEachIterator<T> {
	protected:
		SharedPtr<IEnumerator<T> > iterator;
	public:
		EnumerableForEachIterator(const IEnumerable<T>& enumerable):
		iterator(enumerable.createEnumerator()) {
		
		}
		
		EnumerableForEachIterator(const EnumerableForEachIterator<T>& enumerableIterator):
		iterator(enumerableIterator.iterator) {
		
		}
		
		bool available() const {
			return iterator->available();
		}
		
		void next() const {
			iterator->next();
		}
        
        const T& current() const {
            return iterator->current();
        }
	};
	
	template<typename T>
	EnumerableForEachIterator<T> CreateForEachIterator(const IEnumerable<T>& enumerable) {
		return enumerable;
	}
	
	#define MIST_SCOPE_VARIABLE(TYPE, VARIABLE, VALUE) \
		if(bool __scope_variable_flag__=true) \
			for(TYPE VARIABLE = VALUE; __scope_variable_flag__; __scope_variable_flag__=false)
			
	#define MIST_ENUMERABLE_FOREACH(TYPE, VARIABLE, COLLECTION) \
		UKN_SCOPE_VARIABLE(const mist::ForEachIterator<mist::traits::strip_qualifier<TYPE>::type >&, __foreach_iterator__, mist::CreateForEachIterator<mist::traits::strip_qualifier<TYPE>::type >(COLLECTION))\
		for(bool more=true; __foreach_iterator__.available(); __foreach_iterator__.next()) \
            if((more = false)) {} else \
            for(TYPE VARIABLE=__foreach_iterator__.current(); !more; more=true) \
		
	#define MIST_ENUMERABLE_FOREACH_INDEXED(TYPE, VARIABLE, INDEXER, COLLECTION)\
		MIST_SCOPE_VARIABLE(const mist::ForEachIterator<mist::traits::strip_qualifier<TYPE>::type >&, __foreach_iterator__, CreateForEachIterator<mist::traits::strip_qualifier<TYPE>::type >(COLLECTION))\
		MIST_SCOPE_VARIABLE(intPtr, indexer, 0) \
        for(bool more=true; __foreach_iterator__.available(); __foreach_iterator__.next(), INDEXER++) \
            if((more = false)) {} else \
            for(const VARIABLE=__foreach_iterator__.current(); !more; more=true)
    
} // namespace mist