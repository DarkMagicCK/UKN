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