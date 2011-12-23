//
//  Stream.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Stream_h
#define Project_Unknown_Stream_h

#include "UKN/Platform.h"
#include "UKN/Util.h"
#include "UKN/PreDeclare.h"
#include "UKN/StringUtil.h"

namespace ukn {
    
    class UKN_API Stream {
    public:
        enum StreamType {
            ST_Memory,
            ST_File,
            ST_Net,
        };
        
        virtual bool    seek(size_t pos) = 0;
        virtual size_t  read(uint8* buffer, size_t length) = 0;
        virtual size_t  write(const uint8* buffer, size_t length) = 0;

        virtual size_t  getPos() const = 0;
        virtual size_t  getSize() const = 0;
        virtual bool    isValid() const = 0;
        
        virtual void close() = 0;
        
        virtual StreamType getStreamType() const = 0;
        
        template<typename T>
        Stream& operator<<(const T& val) {
            write((uint8*)&val, sizeof(T));
            return *this;
        }
        template<typename T>
        Stream& operator>>(T& val) {
            read((uint8*)&val, sizeof(T));
            return *this;
        }
        Stream& operator<<(const char* val) {
            write((const uint8*)val, strlen(val));
            return *this;
        }
        
        template<typename T>
        Stream& operator<<(const Array<T>& array) {
            for(size_t i=0; i<array.size(); ++i) {
                *this<<array[i];
            }
            return *this;
        }
    };

    
    class UKN_API MemoryStream: public Stream {
	public:
		MemoryStream();
		MemoryStream(const uint8* data, size_t length);
		
		virtual ~MemoryStream();
        
        void close();
        
        StreamType getStreamType() const {
            return ST_Memory;
        }
		
        // give memory management to MemoryStream
		void set(uint8* data, size_t length);
        // only holds the data pointer, do not hold memory management
        void map(uint8* data, size_t length);
        size_t write(const uint8* data, size_t length);

        bool seek(size_t pos);
		
		size_t getSize() const;
		size_t getRealsize() const;
		size_t getPos() const;
		bool isValid() const;
		
		/* alloc a block of memory */
		bool alloc(size_t size);
		
		// reduce size to real size, beware that this->getSize() have different meaning if you have used seek
		void resize();
				
		template<typename T>
		bool push(T t) {
			return push(&t, sizeof(t));
		}
		        
		/* 
		 read a block of memory, size = sizeof(T) 
		 */
		template<typename T>
		T read();
        
        template<typename T>
        bool read(T* t);
        
		/* 
		 read a block of memory, size specified
		 the memory would be copied to pv
		 so remember to free
		 if buffersize < size, then the size would be set to the bytes accuraly read
		 */
		size_t read(uint8* pv, size_t size);		
		/*
		 read a block of memory from offset 
		 */
		size_t read(size_t offset, uint8* pv, size_t size);
		
		const uint8* get() const;
		const uint8* get(size_t offset) const;
		
		template<typename datatype> 
        datatype& get(size_t offset) const { 
            if(offset > this->getSize()+sizeof(datatype)) offset = 0;
            return *(datatype*)(mData[offset]);
        }
        
        uint8 operator[](size_t index);
        
	private:
		size_t mCurrPos;
        Array<uint8> mData;
	};
	
    
    template<typename T>
    T MemoryStream::read() {
        if(!isValid()) return 0;
        if(this->mCurrPos == this->getSize()) return 0;
        
        uint32 size = sizeof(T);
        if(this->mCurrPos+size <= this->getSize()) {
            T t;
            memcpy(&t, (void*)(get()+this->mCurrPos), size);
            this->mCurrPos += size;
            return t;
        }
        return 0;
    }
    
    template<typename T>
    bool MemoryStream::read(T* t) {
        if(!isValid()) return 0;
        if(this->mCurrPos == this->getSize()) return 0;
        
        uint32 size = sizeof(T);
        if(this->mCurrPos+size <= this->getSize()) {
            memcpy(t, (void*)(get()+this->mCurrPos), size);
            this->mCurrPos += size;
            return true;
        }
        return false;
    }
    
    class UKN_API FileStreamBasic: public Stream {
    public:
        enum SeekType {
            Begin = 0,
            Current = 1,
            End = 2,
        };
        
        virtual ~FileStreamBasic() {}
        virtual bool open(const String& filename, bool canwrite = false, bool append = false, bool nocache = false) = 0;
        
        StreamType getStreamType() const {
            return ST_File;
        }
        
        virtual void close() = 0;
        virtual void truncate() = 0;
        virtual size_t getSize() const = 0;
        virtual size_t getPos() const = 0;
        
        virtual bool isValid() const = 0;
        virtual bool seek(size_t pos) = 0;
        virtual bool seek(size_t pos, SeekType type) = 0;
        virtual size_t read(uint8* data, size_t len) = 0;
        virtual size_t write(const uint8* data, size_t len) = 0;
        
        StreamPtr readIntoMemory();
    };
    
#ifdef UKN_OS_WINDOWS
    /*
     Win32 implemention
     */
    class UKN_API FileStreamWin32: public FileStreamBasic {
    public:
        FileStreamWin32();
        virtual ~FileStreamWin32();
        virtual bool open(const String& filename, bool canwrite = false, bool append = false, bool nocache = false);

        
        virtual void close();
        virtual void truncate();
        virtual size_t getSize() const;
        virtual size_t getPos() const;
        
        virtual bool isValid() const;
        virtual bool seek(size_t pos);
        virtual bool seek(size_t pos, SeekType type);
        virtual size_t read(uint8* data, size_t len);
        virtual size_t write(const uint8* data, size_t len);
        
    private:
        HANDLE file;
    };
    
    typedef FileStreamWin32 FileStream;
    
#else
    /*
     POSIX implemention
     */
    class UKN_API FileStreamPosix: public FileStreamBasic {
    public:
        FileStreamPosix();
        virtual ~FileStreamPosix();
        virtual bool open(const String& filename, bool canwrite = false, bool append = false, bool nocache = false);
        
        virtual void close();
        virtual void truncate();
        virtual size_t getSize() const;
        virtual size_t getPos() const;
        
        virtual bool isValid() const;
        virtual bool seek(size_t pos);
        virtual bool seek(size_t pos, SeekType type);
        virtual size_t read(uint8* data, size_t len);
        virtual size_t write(const uint8* data, size_t len);
        
    private:
        bool nocache;
        FILE* file;
    };
    
    typedef FileStreamPosix FileStream;
    
#endif //OS_WIN32
    
    StreamPtr stream_to_memory_stream(const StreamPtr& stream);
    bool write_stream_to_file(const StreamPtr& stream, const String& file);
    
    bool write_stream_to_file(Stream& stream, const String& file);
    
    /**
     * simple stream that receives / sends data through internet
     * some stream actions may not available for net stream
     * such as size and pos
     * implementation from http://www.keithschwarz.com/interesting/code/?dir=nstream by Keith Schwarz
     **/
    class NetStream: public Stream, public std::iostream {
    public:
        NetStream();
        explicit NetStream(const ukn_string& hostName, uint16 portNum);
        explicit NetStream(uint16 port);

        ~NetStream();
        
        // wait for a connection
        // port no. 0-32767
        void open(uint16 port);
        
        // connect to a server
        void open(const ukn_string& server, uint16 port);
        
        bool is_open() const;
        
        void close();
        
        bool    seek(size_t pos);
        size_t  read(uint8* buffer, size_t length);
        size_t  write(const uint8* buffer, size_t length);
        
        size_t  getPos() const;
        size_t  getSize() const;
        bool    isValid() const;
        
    private:
        class SocketStreambuf;
        
        SocketStreambuf* connection;
    };
    
} // namespace ukn


#endif
