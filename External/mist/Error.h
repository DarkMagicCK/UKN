#ifndef MISTError_H_
#define MISTError_H_

namespace mist {

	class Error {
	public:
		Error(const wchar_t* mssg):
		mErrorMssg(mssg) {
		
		}
		
		const whar_t* getErrorMssg() const {
			return mErrorMssg;
		}
		
	private:	
		const wchar_t* mErrorMssg;
	
	};


} // namespace mist

#endif