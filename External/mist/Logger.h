//
//  Logger.h
//  Mist
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTLogger_h
#define MISTLogger_h

#include "mist/Platform.h"
#include "mist/PreDeclare.h"
#include "mist/StringUtil.h"
#include "mist/Stream.h"

#include <deque>

namespace mist {
    
    /**
     * Simple logger implementation
     **/
    enum LoggerFeature {
        // prepend time since app start
        LF_PrependRunningTime,
        // prepend log level name except LL_Info
        LF_PrependLevelName,
        // also output to console
        LF_OutputToConsole,
    };
    
    enum LogLevel {
        LL_Error,
        LL_Warning,
        LL_Notice,
        LL_Info,
    };
    
    class MIST_API Logger {
    protected:
        Logger(StreamPtr outputStream = StreamPtr());
        ~Logger();
        
    public:
        static Logger& Instance();
        
        // redirect to another stream
        // will output to standard output when a null stream is passed(StreamPtr())
        void redirect(StreamPtr);
        void setFeature(LoggerFeature feature, bool flag);
        
        void log(const MistString& log, LogLevel level=LL_Info);

        // log with LL_Info level
        Logger& operator<<(const MistString& log);
        
        void clear();
        
        size_t getLogSize() const;
        const std::deque<MistString>& getLogQueue() const;
        
    private:
        std::deque<MistString> mLogQueue;
        
        StreamPtr mOutputStream;
        
        bool mPrependTime;
        bool mPrependLevel;
        bool mOutputToConsole;
    };
    
    
    inline void log_error(const std::wstring& log) {
        Logger::Instance().log(log, LL_Error);
    }
    
    inline void log_notice(const std::wstring& log) {
        Logger::Instance().log(log, LL_Notice);
    }
    
    inline void log_warning(const std::wstring& log) {
        Logger::Instance().log(log, LL_Warning);
    }
    
    inline void log_info(const std::wstring& log) {
        Logger::Instance().log(log, LL_Info);
    }
    
    inline void log_error(const std::string& log) {
        Logger::Instance().log(string::StringToWString(log), LL_Error);
    }
    
    inline void log_notice(const std::string& log) {
        Logger::Instance().log(string::StringToWString(log), LL_Notice);
    }
    
    inline void log_warning(const std::string& log) {
        Logger::Instance().log(string::StringToWString(log), LL_Warning);
    }
    
    inline void log_info(const std::string& log) {
        Logger::Instance().log(string::StringToWString(log), LL_Info);
    }
    


#define mist_assert_l(cond, log) {\
        if(!(cond)) { \
            std::wstring message = mist::string::StringToWString(mist::format_string("mist::assertion failed with mssg %s at function %s, file %s, line %d", log, __FUNCTION__, __FILE__, __LINE__)); \
            mist::log_error(message); \
            throw mist::Exception(message, __FUNCTION__, __FILE__, __LINE__); \
        }\
    }
} // namespace mist

#endif
