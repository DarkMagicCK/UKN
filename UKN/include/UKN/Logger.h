//
//  Logger.h
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Logger_h
#define Project_Unknown_Logger_h

#include "Platform.h"
#include "PreDeclare.h"

#include <deque>

namespace ukn {
    
    /**
     * Simple logger implementation
     **/
    enum LoggerFeature {
        // prepend time since app start
        LF_PrependRunningTime,
        // prepend log leven name except LL_Info
        LF_PrependLevelName,
    };
    
    enum LogLevel {
        LL_Error,
        LL_Warning,
        LL_Notice,
        LL_Info,
    };
    
    class Logger {
    protected:
        Logger(StreamPtr outputStream = StreamPtr());
        ~Logger();
        
    public:
        static Logger& Instance();
        
        // redirect to another stream
        // will output to standard output when a null stream is passed(StreamPtr())
        void redirect(StreamPtr);
        void setFeature(LoggerFeature feature, bool flag);
        
        void log(const ukn_string& log, LogLevel level=LL_Info);
        void log(const ukn_wstring& log, LogLevel level=LL_Info);

        // log with LL_Info level
        Logger& operator<<(const ukn_string& log);
        Logger& operator<<(const ukn_wstring& log);
        
        void clear();
        
        size_t getLogSize() const;
        const std::deque<ukn_string>& getLogQueue() const;
        
    private:
        std::deque<ukn_string> mLogQueue;
        
        StreamPtr mOutputStream;
        
        bool mPrependTime;
        bool mPrependLevel;
    };
    
    void log_error(const ukn_string& log);
    void log_notice(const ukn_string& log);
    void log_warning(const ukn_string& log);
    void log_info(const ukn_string& log);
    
    void log_error(const ukn_wstring& log);
    void log_notice(const ukn_wstring& log);
    void log_warning(const ukn_wstring& log);
    void log_info(const ukn_wstring& log);
    
} // namespace ukn

#endif
