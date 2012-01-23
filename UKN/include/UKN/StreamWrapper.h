//
//  StreamWrapper.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/22/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_StreamWrapper_h
#define Project_Unknown_StreamWrapper_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/Stream.h"

namespace ukn {
    
    /**
     * Binary writer wrapper which writes binar data into the stream
     **/
    class BinaryStreamWriter: public Object {
    public:
        BinaryStreamWriter(const StreamPtr& stream);
        virtual ~BinaryStreamWriter();
            
        // flush or close be called to flush the underlying buffer to the stream
        void flush();
        void close();
        
    public:
        // 1 bytes
        void write(uint8 val);
        void write(char val);
        
        // 2 bytes
        void write(int16 val);
        void write(uint16 val);
        
        // 4 bytes
        void write(int32 val);
        void write(uint32 val);
        
        // 8 bytes
        void write(int64 val);
        void write(uint64 val);
        
        // 4 bytes
        void write(float val);
        // 8 bytes
        void write(double val);
        
        // byte array
        void write(const uint8* bytes, size_t len);
        
        // byte array with specified range
        void write(const uint8* bytes, size_t start, size_t end);
        
        // encoded string
        void write(const String& str);
        
        template<typename T>
        void writePOD(const T& data);
                
    private:
        Array<uint8> mBuffer;
        
        StreamPtr mStream;
    };
    
    template<typename T>
    void BinaryStreamWriter::writePOD(const T& data) {
        // ukn_assert(POD<T>::Value);
        write(&data, sizeof(T));
    }
    
    class BinaryStreamReader: public Object {
    public:
        BinaryStreamReader(const StreamPtr& stream);
        virtual ~BinaryStreamReader();
        
        void close();
        
    public:
        // peek next char, does not advance stream
        char peekChar();
        
        size_t read(uint8* buffer, size_t size_to_read);
        
        uint8   readByte();
        uint16  readUInt16();
        uint32  readUInt32();
        uint64  readUInt64();
        
        int8    readChar();
        int16   readInt16();
        int32   readInt32();
        int64   readInt64();
        
        float   readFloat();
        double  readDouble();
        
        // read a string with specified encoding
        String  readString(size_t char_count, StringFormat encoding_format);
        
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
    
} // namespace ukn


#endif
