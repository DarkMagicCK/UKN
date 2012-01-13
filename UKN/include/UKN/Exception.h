/*
 *  Exception.h
 *  Hoshizora
 *
 *  Created by Robert Bu on 8/27/10.
 *  Copyright 2010 Robert Bu. All rights reserved.
 *
 */

#ifndef ukn_exception_h
#define ukn_exception_h

#include "UKN/Platform.h"
#include "UKN/StackTrace.h"
#include <string>
#include <sstream>
#include <typeinfo>

#define UKN_THROW_EXCEPTION(mess) throw ukn::Exception(mess,   \
                                                        __FUNCTION__,     \
                                                        __FILE__,         \
                                                        __LINE__)

namespace ukn {

	class Exception: public std::exception {
	public:
		Exception(const ukn_string& mssg, const ukn_string& function, const ukn_string& file, int line):
        mMssg(mssg),
        mFunction(function),
        mFile(file),
        mLine(line) {

        }

        Exception(const ukn_string& mssg):
        mMssg(mssg),
        mFunction("?"),
        mFile("?"),
        mLine(-1) {

        }

        virtual ~Exception() throw() {}

		// get a formatted exception mssg
		virtual const char* what() const throw() {
            std::ostringstream mssg;
            mssg << "ukn::Exception: class=" << name() << "\nMessage=" << mMssg;
            if(mFunction[0] != '?') mssg << "\nFunction=" << mFunction;
            if(mFile[0] != '?') mssg << "\nFile=" << mFile;
            if(mLine != -1) mssg << "\nLine=" << mLine;

            return mssg.str().c_str();
        }

        // the name of the exception
        virtual const char* name() const throw() {
            return "ukn::Exception";
        }

        // the class name of the exception
        virtual const char* className() const throw() {
            return typeid(*this).name();
        }

		const ukn_string& mssg() const {
            return mMssg;
        }
		const ukn_string& function() const {
            return mFunction;
        }
		const ukn_string& file() const {
            return mFile;
        }

		int32 line() const {
            return mLine;
        }

        virtual void rethrow() {
            throw *this;
        }

        ukn_string getFormattedCallStack() const {
            return ukn_get_formatted_stack_trace_string();
        }

	private:
		ukn_string mMssg, mFunction, mFile;
        int32 mLine;
	};


} // namespace sora

#endif
