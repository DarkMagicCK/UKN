#include "UKN/Basic.h"
#include "UKN/Ptr.h"

namespace ukn {
	
	template<typename T>
	class ForEachIterator: public Object {
	public:
		virtual bool available(T& var) const = 0;
		virtual void next() const = 0;
		
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
		
		bool available(T& var) const {
			if(iterator->available()) {
				var = iterator->current();
				return true;
			} else {
				return false;
			}
		}
		
		void next() const {
			iterator->next();
		}
	};
	
	template<typename T>
	EnumerableForEachIterator<T> CreateForEachIterator(const IEnumerable<T>& enumerable) {
		return enumerable;
	}
	
	#define UKN_SCOPE_VARIABLE(TYPE, VARIABLE, VALUE) \
		if(bool __scope_variable_flag__=true) \
			for(TYPE VARIABLE = VALUE; __scope_variable_flag__; __scope_variable_flag__=false)
			
	#define UKN_ENUMERABLE_FOREACH(TYPE, VARIABLE, COLLECTION) \
		UKN_SCOPE_VARIABLE(const ukn::ForEachIterator<TYPE>&, __foreach_iterator__, CreateForEachIterator(COLLECTION))\
		for(TYPE VARIABLE; __foreach_iterator__.available(VARIABLE); __foreach_iterator__.next())
		
	#define UKN_ENUMERABLE_FOREACH_INDEXED(TYPE, VARIABLE, INDEXER, COLLECTION)\
		UKN_SCOPE_VARIABLE(const ukn::ForEachIterator<TYPE>&, __foreach_iterator__, CreateForEachIterator(COLLECTION))\
		UKN_SCOPE_VARIABLE(intPtr, indexer, 0) \
		for(TYPE VARIABLE; __foreach_iterator__.available(VARIABLE); __foreach_iterator__.next(), INDEXER++)
    
} // namespace ukn