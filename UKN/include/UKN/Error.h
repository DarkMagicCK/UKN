#ifndef Project_Unknown_Error_H_
#define Project_Unknown_Error_H_

namespace ukn {

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


} // namespace ukn

#endif