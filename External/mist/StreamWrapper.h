//
//  StreamWrapper.h
//  Mist
//
//  Created by Ruiwei Bu on 1/22/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef MISTStreamWrapper_h
#define MISTStreamWrapper_h

#include "mist/Platform.h"
#include "mist/PreDeclare.h"
#include "mist/Stream.h"

namespace mist {
    
    /** virtual base class for stream writers */
    class MIST_API StreamWriter: public Object {
    public:
        virtual ~StreamWriter();
        
        virtual void flush() = 0;
        virtual void close() = 0;
        
    public:
        virtual void write(uint8 val) = 0;
        virtual void write(char val) = 0;
        
        virtual void write(int16 val) = 0;
        virtual void write(uint16 val) = 0;
        
        virtual void write(int32 val) = 0;
        virtual void write(uint32 val) = 0;
        
        virtual void write(int64 val) = 0;
        virtual void write(uint64 val) = 0;
        
        virtual void write(float val) = 0;
        virtual void write(double val) = 0;
        
        virtual void write(const uint8* bytes, size_t len) = 0;
        virtual void write(const uint8* bytes, size_t start, size_t end) = 0;
        virtual void write(const MistString& str) = 0;
        
        virtual void writeLineSeparator() = 0;
        
        virtual void writeLine(uint8 val);
        virtual void writeLine(char val);
        
        virtual void writeLine(int16 val);
        virtual void writeLine(uint16 val);
        
        virtual void writeLine(int32 val);
        virtual void writeLine(uint32 val);
        
        virtual void writeLine(int64 val);
        virtual void writeLine(uint64 val);
        
        virtual void writeLine(float val);
        virtual void writeLine(double val);
        
        virtual void writeLine(const uint8* bytes, size_t len);
        virtual void writeLine(const uint8* bytes, size_t start, size_t end);
        virtual void writeLine(const MistString& str);
        
    private:
    };
    
    /**
     * Binary writer wrapper which writes binar data into the stream
     **/
    class MIST_API BinaryStreamWriter: public StreamWriter {
    public:
        BinaryStreamWriter(const StreamPtr& stream);
        virtual ~BinaryStreamWriter();
            
        // flush or close be called to flush the underlying buffer to the stream
        virtual void flush();
        virtual void close();
        
    public:
        // 1 bytes
        virtual void write(uint8 val);
        virtual void write(char val);
        
        // 2 bytes
        virtual void write(int16 val);
        virtual void write(uint16 val);
        
        // 4 bytes
        virtual void write(int32 val);
        virtual void write(uint32 val);
        
        // 8 bytes
        virtual void write(int64 val);
        virtual void write(uint64 val);
        
        // 4 bytes
        virtual void write(float val);
        // 8 bytes
        virtual void write(double val);
        
        // byte array
        virtual void write(const uint8* bytes, size_t len);
        
        // byte array with specified range
        virtual void write(const uint8* bytes, size_t start, size_t end);
        
        // encoded string
        virtual void write(const MistString& str);
        
        virtual void writeLineSeparator();
        
        template<typename T>
        void writePOD(const T& data);
                
    private:
        Array<uint8> mBuffer;
        
        StreamPtr mStream;
    };
    
    template<typename T>
    void BinaryStreamWriter::writePOD(const T& data) {
        // mist_assert(POD<T>::Value);
        write(&data, sizeof(T));
    }
    
    class MIST_API TextStreamWriter: public StreamWriter {
    public:
        TextStreamWriter(const StreamPtr& stream);
        virtual ~TextStreamWriter();
        
        virtual void flush();
        virtual void close();
        
    public:
        virtual void write(uint8 val);
        virtual void write(char val);
        
        virtual void write(int16 val);
        virtual void write(uint16 val);
        
        virtual void write(int32 val);
        virtual void write(uint32 val);
        
        virtual void write(int64 val);
        virtual void write(uint64 val);
        
        virtual void write(float val);
        virtual void write(double val);
        
        virtual void write(const uint8* bytes, size_t len);
        
        virtual void write(const uint8* bytes, size_t start, size_t end);
        
        virtual void write(const MistString& str);
        
        virtual void writeLineSeparator();
        
    private:
        StreamPtr mStream;
    };
    
    class MIST_API StreamReader: public Object {
    public:
        virtual ~StreamReader();
        
        virtual void close() = 0;
        
    public:
        virtual char peek() = 0;
        
        virtual size_t read(uint8* buffer, size_t size_to_read) = 0;
        
        virtual uint8   readByte() = 0;
        virtual uint16  readUInt16() = 0;
        virtual uint32  readUInt32() = 0;
        virtual uint64  readUInt64() = 0;
        
        virtual int8    readChar() = 0;
        virtual int16   readInt16() = 0;
        virtual int32   readInt32() = 0;
        virtual int64   readInt64() = 0;
        
        virtual float   readFloat() = 0;
        virtual double  readDouble() = 0;
        
        virtual MistString  readString() = 0;
    };
    
    class MIST_API BinaryStreamReader: public StreamReader {
    public:
        BinaryStreamReader(const StreamPtr& stream);
        virtual ~BinaryStreamReader();
        
        virtual void close() override;
        
    public:
        // peek next char, does not advance stream
        virtual char peek() override;
        
        virtual size_t read(uint8* buffer, size_t size_to_read) override;
        
        virtual uint8   readByte() override;
        virtual uint16  readUInt16() override;
        virtual uint32  readUInt32() override;
        virtual uint64  readUInt64() override;
        
        virtual int8    readChar() override;
        virtual int16   readInt16() override;
        virtual int32   readInt32() override;
        virtual int64   readInt64() override;
        
        virtual float   readFloat() override;
        virtual double  readDouble() override;
        
        virtual MistString  readString() override;
        
        // read a string with specified encoding
        MistString  readString(size_t char_count);
        
        template<typename T>
        T readPOD();
        
    private:
        StreamPtr mStream;
    };
    
    template<typename T>
    T  BinaryStreamReader::readPOD() {
        T buffer;
        read(&buffer, sizeof(T));
        return buffer;
    }
    
    class MIST_API TextStreamReader: public StreamReader {
    public:
        TextStreamReader(const StreamPtr& stream);
        virtual ~TextStreamReader();
        
        virtual void close() override;
        
    public:
        virtual char peek() override;
        
        virtual size_t read(uint8* buffer, size_t size_to_read);
        
        virtual uint8   readByte() override;
        virtual uint16  readUInt16() override;
        virtual uint32  readUInt32() override;
        virtual uint64  readUInt64() override;
        
        virtual int8    readChar() override;
        virtual int16   readInt16() override;
        virtual int32   readInt32() override;
        virtual int64   readInt64() override;
        
        virtual float   readFloat() override;
        virtual double  readDouble() override;
        
        virtual MistString  readString() override;
    
        MistString readLine();
        MistString readNumber();
        MistString readTill(char end);
        
    private:
        StreamPtr mStream;
    };

} // namespace mist


#endif
