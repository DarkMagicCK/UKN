//
//  StreamWrapper.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/22/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "UKN/StreamWrapper.h"

namespace ukn {
    
    BinaryStreamWriter::BinaryStreamWriter(const StreamPtr& stream):
    mStream(stream) {
        ukn_assert(stream && stream->canWrite());
    }
    
    BinaryStreamWriter::~BinaryStreamWriter() {
        
    }
       
    void BinaryStreamWriter::close() {
        flush();
        mStream->close();
    }
    
    void BinaryStreamWriter::flush() {
        if(mStream.isValid()) {
            mStream->write(mBuffer.begin(), mBuffer.size());
            mBuffer.clear();
        } else {
            UKN_THROW_EXCEPTION("ukn::BinaryStreamWriter::flush: stream not valid, maybe it has been closed before flush?");
        }
    }
    
    void BinaryStreamWriter::write(uint8 val) {
        mBuffer.append(val);
    }
    
    void BinaryStreamWriter::write(char val) {
        mBuffer.append(val);
    }
    
    void BinaryStreamWriter::write(int16 val) {
        mBuffer.append((uint8*)&val, 2);
    }
    
    void BinaryStreamWriter::write(int32 val) {
        mBuffer.append((uint8*)&val, 4);
    }
    
    void BinaryStreamWriter::write(int64 val) {
        mBuffer.append((uint8*)&val, 8);
    }
    
    void BinaryStreamWriter::write(uint16 val) {
        mBuffer.append((uint8*)&val, 2);
    }
    
    void BinaryStreamWriter::write(uint32 val) {
        mBuffer.append((uint8*)&val, 4);
    }
    
    void BinaryStreamWriter::write(uint64 val) {
        mBuffer.append((uint8*)&val, 8);
    }
    
    void BinaryStreamWriter::write(float val) {
        mBuffer.append((uint8*)&val, sizeof(float));
    }
    
    void BinaryStreamWriter::write(const String& str) {
        mBuffer.append((uint8*)str.c_str(), str.length());
    }
    
    BinaryStreamReader::BinaryStreamReader(const StreamPtr& stream):
    mStream(stream) {
        ukn_assert(stream && stream->canWrite());
    }
    
    BinaryStreamReader::~BinaryStreamReader() {
        
    }
    
    void BinaryStreamReader::close() {
        mStream->close();
    }
    
    char BinaryStreamReader::peekChar() {
        ukn_assert(mStream->isValid());
        
        char buffer;
        size_t pos = mStream->pos();
        mStream->read((uint8*)&buffer, 1);
        mStream->seek(pos);
        return buffer;
    }
    
    size_t BinaryStreamReader::read(uint8* buffer, size_t size_to_read) {
        return mStream->read(buffer, size_to_read);
    }
    
    uint8 BinaryStreamReader::readByte() {
        uint8 buffer;
        mStream->read(&buffer, 1);
        return buffer;
    }
    
    uint16 BinaryStreamReader::readUInt16() {
        uint16 buffer;
        mStream->read((uint8*)&buffer, 2);
        return buffer;
    }
    
    uint32 BinaryStreamReader::readUInt32() {
        uint32 buffer;
        mStream->read((uint8*)&buffer, 4);
        return buffer;
    }
    
    uint64 BinaryStreamReader::readUInt64() {
        uint64 buffer;
        mStream->read((uint8*)&buffer, 8);
        return buffer;
    }
    
    int8 BinaryStreamReader::readChar() {
        int8 buffer;
        mStream->read((uint8*)&buffer, 1);
        return buffer;
    }
    
    int16 BinaryStreamReader::readInt16() {
        int16 buffer;
        mStream->read((uint8*)&buffer, 2);
        return buffer;
    }
    
    int32 BinaryStreamReader::readInt32() {
        int32 buffer;
        mStream->read((uint8*)&buffer, 4);
        return buffer;
    }
    
    int64 BinaryStreamReader::readInt64() {
        int64 buffer;
        mStream->read((uint8*)&buffer, 8);
        return buffer;
    }
    
    float BinaryStreamReader::readFloat() {
        float buffer;
        mStream->read((uint8*)&buffer, 4);
        return buffer;
    }
    
    double BinaryStreamReader::readDouble() {
        double buffer;
        mStream->read((uint8*)&buffer, 8);
        return buffer;
    }
    
    String BinaryStreamReader::readString(size_t char_count, StringFormat encoding_format) {
        uint8* char_data = ukn_malloc_t(uint8, char_count);
        size_t read_count = mStream->read(char_data, char_count);
        if(read_count != char_count) {
            // exception
        }
        String str((const char*)char_data, encoding_format);
        ukn_free(char_data);
        return str;
    }
    
} // namespace ukn
