//
//  Serial.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/2/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "Serial.h"
#include "Logger.h"
#include "MemoryUtil.h"
#include "TimeUtil.h"

#ifdef MIST_OS_FAMILY_UNIX

#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <unistd.h>
#include <sys/ioctl.h>
#include <getopt.h>

#else

#include <Windows.h>

#endif

namespace mist {
    
#ifdef MIST_OS_FAMILY_UNIX

    struct Serial::SerialImpl {
        SerialImpl():
        _fd(-1),
        _baudRate(B9600) {
            
        }
        
        bool open(const std::string& port, int baudRate) {
            struct termios toptions;
            
            _fd = ::open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
            if (_fd == -1)  {
                log_error("init_serialport: Unable to open port - " + port);
                return false;
            }
            
            if (tcgetattr(_fd, &toptions) < 0) {
                log_error("init_serialport: Couldn't get term attributes");
                
                ::close(_fd);
                _fd = -1;
                return false;
            }
            
            switch(baudRate) {
                case 4800:   _baudRate=B4800;   break;
                case 9600:   _baudRate=B9600;   break;
#ifdef B14400
                case 14400:  _baudRate=B14400;  break;
#endif
                case 19200:  _baudRate=B19200;  break;
#ifdef B28800
                case 28800:  _baudRate=B28800;  break;
#endif
                case 38400:  _baudRate=B38400;  break;
                case 57600:  _baudRate=B57600;  break;
                case 115200: _baudRate=B115200; break;
            }
            cfsetispeed(&toptions, _baudRate);
            cfsetospeed(&toptions, _baudRate);
            
            // 8N1
            toptions.c_cflag &= ~PARENB;
            toptions.c_cflag &= ~CSTOPB;
            toptions.c_cflag &= ~CSIZE;
            toptions.c_cflag |= CS8;
            // no flow control
            toptions.c_cflag &= ~CRTSCTS;
            
            toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
            toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl
            
            toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
            toptions.c_oflag &= ~OPOST; // make raw
            
            // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
            toptions.c_cc[VMIN]  = 0;
            toptions.c_cc[VTIME] = 20;
            
            if( tcsetattr(_fd, TCSANOW, &toptions) < 0) {
                log_error("init_serialport: Couldn't set term attributes");
                
                ::close(_fd);
                _fd = -1;
                return false;
            }
            
            return true;
        }
        
        void close() {
            if(_fd != -1) {
                ::close(_fd);
                _fd = -1;
            }
        }
        
        int write(const char* data, size_t len) {
            if(_fd != -1) {
                long bytesWritten = ::write(_fd, data, len);
                return (int)bytesWritten;
            }
            return -1;
        }
        
        std::string read(char until) {
            std::string result;
            
            char b[1];
            do {
                long n = ::read(_fd, b, 1);  // read a char at a time
                if( n == -1 ) return std::string();
                if( n == 0 ) {
                    usleep( 10 * 1000 ); // wait 10 msec try again
                    continue;
                }
                result += b[0];
            } while( b[0] != until );
            
            return result;
        }

		std::string readN(size_t nSize) {
			char* buffer = mist_malloc_t(char, nSize);
			if(!buffer)
				return std::string();

			long n = ::read(_fd, buffer, nSize);
			if(n == -1) {
				mist_free(buffer);
				return std::string();
			}
			std::string result = std::string(buffer, buffer+nSize);
			mist_free(buffer);
			return result;
		}

		std::string readAll() {
			std::string result;

			char buffer[100];
            long n;
			do {
				n = ::read(_fd, buffer, 100);
				if(n == -1)
					return std::string();
				if(n == 0)
					break;
				result += std::string(buffer, buffer + n);
			} while(n > 0);
			
			return result;
		}
        
        std::string _port;
        speed_t _baudRate;
        int _fd;
    };
    
    
#elif defined(MIST_OS_WINDOWS)

	struct Serial::SerialImpl {
		Serial::SerialImpl():
		mPortHandle(0) {

		}

		bool open(const std::string& port, int baudRate) {
			mPortHandle = CreateFileA(port.c_str(),
									  GENERIC_READ | GENERIC_WRITE,
									  0,
									  NULL,
									  OPEN_EXISTING,
									  0,
									  NULL);
			DCB config;
			if(GetCommState(mPortHandle, &config) == 0) {
				CloseHandle(mPortHandle);
				return false;
			}

			config.BaudRate = baudRate;

			if(SetCommState(mPortHandle, &config) == 0) {
				CloseHandle(mPortHandle);
				return false;
			}

			COMMTIMEOUTS comTimeout;
			comTimeout.ReadIntervalTimeout = 3;
			comTimeout.ReadTotalTimeoutMultiplier = 3;
			comTimeout.ReadTotalTimeoutConstant = 2;
			comTimeout.WriteTotalTimeoutConstant = 2;
			comTimeout.WriteTotalTimeoutMultiplier = 3;

			if(SetCommTimeouts(mPortHandle, &comTimeout) == 0) {
				CloseHandle(mPortHandle);
				return false;
			}
			return false;
        }
        void close() {
            if(mPortHandle) {
				CloseHandle(mPortHandle);
			}
        }
        int write(const char* data, size_t len) {
			DWORD bytesWritten;
			if(WriteFile(mPortHandle,
				         data,
						 len,
						 &bytesWritten,
						 NULL) == 0) {
				return -1;
			}
            return (int)bytesWritten;
        }
        std::string read(char until) {
			std::string result;
            
            char b[1];
            do {
				DWORD bytesRead;
                if(ReadFile(mPortHandle, b, 1, &bytesRead, NULL) == 0) {
					return result;
				}
                if( bytesRead == 0 ) {
                    msleep( 10 ); // wait 10 msec try again
                    continue;
                }
                result += b[0];
            } while( b[0] != until );
            
            return result;
        }
		std::string readN(size_t nSize) {
			char* buffer = mist_malloc_t(char, nSize);
			if(!buffer)
				return std::string();

			DWORD bytesRead = 0;
		
			if(ReadFile(mPortHandle, buffer, nSize, &bytesRead, NULL) == 0) {
				mist_free(buffer);
				return std::string();
			}
			
			std::string result = std::string(buffer, buffer+nSize);
			mist_free(buffer);
			return result;
		}
		std::string readAll() {
			std::string result;
			DWORD bytesRead;
			
			char buffer[100];
			do {
				if(ReadFile(mPortHandle, buffer, sizeof(buffer), &bytesRead, NULL) == 0)
					return result;
				if(bytesRead == 0)
					break;
				result += std::string(buffer, buffer + bytesRead);
			} while(bytesRead > 0);
			
			return result;
		}

		HANDLE mPortHandle;
	};
    
    
#else
    
    /* Null Impl */
    struct Serial::SerialImpl {
        bool open(const std::string& port, int baudRate) {
            return false;
        }
        void close() {
            
        }
        int write(const char* data, size_t len) {
            return -1;
        }
        std::string read(char until) {
            return std::string();
        }
    };
    
#endif
    
    
    Serial::Serial():
    mPort(std::string()),
    mBaudRate(9600),
    mImpl(new SerialImpl) {
    
    }
    
    Serial::Serial(const std::string& port, int baudRate):
    mImpl(new SerialImpl),
    mPort(port),
    mBaudRate(baudRate) {
    
    }
    
    Serial::~Serial() {
        if(mImpl) {
            mImpl->close();
            delete mImpl;
        }
    }
    
    void Serial::init() {
        
    }
    
    bool Serial::open() {
        if(mPort.empty())
            return false;
        return mImpl->open(mPort, mBaudRate);
    }
    
    void Serial::close() {
        mImpl->close();
    }
    
    int Serial::write(const char* data, size_t len) {
        return mImpl->write(data, len);
    }
    
    std::string Serial::read(char until) {
        return mImpl->read(until);
    }

	std::string Serial::readN(size_t nSize) {
		return mImpl->readN(nSize);
	}

	std::string Serial::readAll() {
		return mImpl->readAll();
	}
    
    Serial& Serial::port(const std::string& port) {
        mPort = port;
        return *this;
    }
    
    Serial& Serial::baudRate(int baudRate) {
        mBaudRate = baudRate;
        return *this;
    }
    
    const std::string& Serial::port() const {
        return mPort;
    }
    
    int Serial::baudRate() const {
        return mBaudRate;
    }
    
}