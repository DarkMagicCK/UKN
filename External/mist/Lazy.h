#ifndef MISTLazy_h_
#define MISTLazy_h_

#include "mist/Function.h"
#include "mist/SharedPtr.h"

namespace mist {

	template<typename T>
	class Lazy {
	protected:
		struct LazyInfo {
			Function<T()> evaluator;
			bool		  evaluated;
			T			  value;
		};
		
		SharedPtr<LazyInfo> mLazyInfo;
		
	public:
		Lazy() {
		
		}
		
		Lazy(const Function<T()>& eval) {
			this->mLazyInfo = new LazyInfo;
			this->mLazyInfo->evaluated = false;
			this->mLazyInfo->evaluator = eval;
		}
		
		Lazy(const T& value) {
			this->mLazyInfo = new LazyInfo;
			this->mLazyInfo->evaluated = true;
			this->mLazyInfo->value = value;
		}
		
		Lazy(const Lazy<T>& lazy):
		mLazyInfo(lazy.mLazyInfo) {
		
		}
		
		Lazy<T>& operator=(const Function<T()>& eval) {
			this->mLazyInfo = new LazyInfo;
			this->mLazyInfo->evaluated = false;
			this->mLazyInfo->evaluator = eval;
			return *this;
		}
		
		Lazy<T>& operator=(const T& value) {
			this->mLazyInfo = new LazyInfo;
			this->mLazyInfo->evaluated = true;
			this->mLazyInfo->value = value;
			return *this;
		}
		
		Lazy<T>& operator=(const Lazy<T>& rhs) {
			if(this != &rhs) {
				this->mLazyInfo = rhs.mLazyInfo;
			}
			return *this;
		}
		
		const T& value() const {
			if(!mLazyInfo->evaluated) {
				mLazyInfo->evaluated = true;
				mLazyInfo->value = mLazyInfo->evaluator();
				mLazyInfo->evaluator = Function<T()>();
			}
			return mLazyInfo->value;
		}
		
		bool isAvailable() const {
			return mLazyInfo;
		}
	};

} // namespace mist

#endif