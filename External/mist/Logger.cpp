//
//  Logger.cpp
//  Mist
//
//  Created by Robert Bu on 12/1/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/Logger.h"
#include "mist/Stream.h"
#include "mist/TimeUtil.h"
#include "mist/Common.h"
#include "mist/StringUtil.h"

#include <iostream>

namespace mist {
    
    Logger& Logger::Instance() {
        static Logger* g_Instance = 0;
        if(!g_Instance) {
            FileStream* default_stream = new FileStream();
            // open with no write buffer, no append and write mode
            default_stream->open(L"ukn_log.txt", true, false, true);
            g_Instance = new Logger(StreamPtr(default_stream));
        }
        return *g_Instance;
    }
    
    Logger::Logger(StreamPtr outputStream):
    mOutputStream(outputStream),
    mPrependTime(true),
    mPrependLevel(true) {
        
    }
    
    Logger::~Logger() {
        if(mOutputStream)
            mOutputStream->close();
    }
    
    void Logger::redirect(StreamPtr outputStream) {
        mOutputStream = outputStream;
    }
    
    void Logger::setFeature(LoggerFeature feature, bool flag) {
        switch(feature) {
            case LF_PrependRunningTime:
                mPrependTime = flag;
                break;
            case LF_PrependLevelName:
                mPrependLevel = flag;
                break;
            case LF_OutputToConsole:
                mOutputToConsole = flag;
                break;
        }
    }
    
    inline MistString loglevel_to_string(LogLevel level) {
        switch(level) {
            case LL_Error:
                return L"*Error* ";
            case LL_Warning:
                return L"*Warning* ";
            case LL_Notice:
                return L"*Notice* ";
            case LL_Info:
            default:
                return MistString();
        }
    }
    
    void Logger::log(const MistString& log, LogLevel level) {
        MistString realLog;
        if(mPrependTime) {
            realLog += string::StringToWString((format_string("[%.3f] ", (float)FrameCounter::Instance().getRunningTime() / Timestamp::Resolution())));
        }
        if(mPrependLevel) {
            realLog += loglevel_to_string(level);
        }
        realLog += log;
        
        mLogQueue.push_back(realLog);
        
        if(mOutputStream) {
            std::string log = string::WStringToString(realLog);
#ifdef MIST_OS_WINDOWS
            log += "\r\n";
            mOutputStream->write(log.c_str(), log.size());
#else
			log += "\n";
            mOutputStream->write((uint8*)log.c_str(), log.size());
#endif // MIST_OS_WINDOWS
            if(mOutputToConsole) {
                std::wcout<<realLog<<std::endl;
            }
        }
    }
    
    Logger& Logger::operator<<(const MistString& log) {
        this->log(log, LL_Info);
        return *this;
    }
    
    void Logger::clear() {
        mLogQueue.clear();
    }
    
    size_t Logger::getLogSize() const {
        return mLogQueue.size();
    }
   
    const std::deque<MistString>& Logger::getLogQueue() const {
        return mLogQueue;
    }
   
} // namespace mist
