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
    
    enum StreamType {
        ST_Memory,
        ST_File,
        ST_Net,
    };
    
    class UKN_API IStream {
    public:
    	virtual bool	canRead() const = 0;
    	virtual bool	canWrite() const = 0;
    	virtual bool 	canSeek() const = 0;
    	
        virtual bool    seek(size_t pos) = 0;
        virtual size_t  read(uint8* buffer, size_t length) = 0;
        virtual size_t  write(const uint8* buffer, size_t length) = 0;

        virtual size_t  pos() const = 0;
        virtual size_t  size() const = 0;
        virtual bool    isValid() const = 0;
        
        virtual void close() = 0;
        
        virtual StreamType getStreamType() const = 0;
        
        template<typename T>
        IStream& operator<<(const T& val) {
            write((uint8*)&val, sizeof(T));
            return *this;
        }
        
        template<typename T>
        IStream& operator>>(T& val) {
            read((uint8*)&val, sizeof(T));
            return *this;
        }
        
        IStream& operator<<(const char* val) {
            write((const uint8*)val, strlen(val));
            return *this;
        }
        
        template<typename T>
        IStream& operator<<(const Array<T>& array) {
            for(size_t i=0; i<array.size(); ++i) {
                *this<<array[i];
            }
            return *this;
        }
    };

    class UKN_API MemoryStream: public IStream {
	public:
		MemoryStream();
        MemoryStream(const MemoryStream& rhs);
		MemoryStream(const uint8* data, size_t length);
		
		virtual ~MemoryStream();
        
        MemoryStream& operator=(const MemoryStream& rhs);
        
        void close();
        
        StreamType getStreamType() const {
            return ST_Memory;
        }
		
        // give memory management to MemoryStream
		void set(uint8* data, size_t length);
        // only holds the data pointer, does not hold memory management
        void map(uint8* data, size_t length);
        size_t write(const uint8* data, size_t length);

        bool seek(size_t pos);
		
		size_t size() const;
		size_t capacity() const;
		size_t pos() const;
		bool isValid() const;
		
		/* alloc a block of memory */
		bool alloc(size_t size);
		
		// reduce size to real size
		void resize();
        void resize(size_t newsize);
				
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
		
        uint8* data();
        uint8* dataOf(size_t offset);
		const uint8* data() const;
		const uint8* dataOf(size_t offset) const;
		
		template<typename datatype> 
        datatype& get(size_t offset) const { 
            if(offset > this->size()+sizeof(datatype)) offset = 0;
            return *(datatype*)(mData[offset]);
        }
        
        uint8 operator[](size_t index);
        
        bool canRead() const;
        bool canWrite() const;
        bool canSeek() const;
        
	private:
		size_t mCurrPos;
        Array<uint8> mData;
	};
    
    template<typename T>
    T MemoryStream::read() {
        if(!isValid()) return 0;
        if(this->mCurrPos == this->size()) return 0;
        
        uint32 size = sizeof(T);
        if(this->mCurrPos + size <= this->size()) {
            T t;
            memcpy(&t, (void*)(data()+this->mCurrPos), size);
            this->mCurrPos += size;
            return t;
        }
        return 0;
    }
    
    template<typename T>
    bool MemoryStream::read(T* t) {
        if(!isValid()) return 0;
        if(this->mCurrPos == this->size()) return 0;
        
        uint32 size = sizeof(T);
        if(this->mCurrPos+size <= this->size()) {
            memcpy(t, (void*)(data()+this->mCurrPos), size);
            this->mCurrPos += size;
            return true;
        }
        return false;
    }
    
    class UKN_API FileStreamBasic: public IStream {
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
        virtual size_t size() const = 0;
        virtual size_t pos() const = 0;
        
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
	
		bool canRead() const;
        bool canWrite() const;
        bool canSeek() const;
        
        virtual void close();
        virtual void truncate();
        virtual size_t size() const;
        virtual size_t pos() const;
        
        virtual bool isValid() const;
        virtual bool seek(size_t pos);
        virtual bool seek(size_t pos, SeekType type);
        virtual size_t read(uint8* data, size_t len);
        virtual size_t write(const uint8* data, size_t len);
        
    private:
        HANDLE file;
        bool canwrite;
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
        
        bool canRead() const;
        bool canWrite() const;
        bool canSeek() const;
        
        virtual void close();
        virtual void truncate();
        virtual size_t size() const;
        virtual size_t pos() const;
        
        virtual bool isValid() const;
        virtual bool seek(size_t pos);
        virtual bool seek(size_t pos, SeekType type);
        virtual size_t read(uint8* data, size_t len);
        virtual size_t write(const uint8* data, size_t len);
        
    private:
        bool nocache;
        bool canwrite;
        FILE* file;
    };
    
    typedef FileStreamPosix FileStream;
    
#endif //OS_WIN32
    
    StreamPtr stream_to_memory_stream(const StreamPtr& stream);
    bool write_stream_to_file(const StreamPtr& stream, const String& file);
    
    bool write_stream_to_file(IStream& stream, const String& file);
    
    /**
     * simple stream that receives / sends data through internet
     * some stream actions may not available for net stream
     * such as size and pos
     * implementation from http://www.keithschwarz.com/interesting/code/?dir=nstream by Keith Schwarz
     **/
    class NetStream: public IStream, public std::iostream {
    public:
        NetStream();
        explicit NetStream(const ukn_string& hostName, uint16 portNum);
        explicit NetStream(uint16 port);

        ~NetStream();
        
        bool canRead() const;
        bool canWrite() const;
        bool canSeek() const;
        
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
        
        size_t  pos() const;
        size_t  size() const;
        bool    isValid() const;
        
    private:
        class SocketStreambuf;
        
        SocketStreambuf* connection;
    };
    
} // namespace ukn


#endif
