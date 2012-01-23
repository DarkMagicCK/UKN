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
        
    }
    
    BinaryStreamWriter::~BinaryStreamWriter() {
        
    }
    
    bool BinaryStreamWriter::canRead() const {
        return mStream->canRead();
    }
    
    bool BinaryStreamWriter::canWrite() const {
        return mStream->canWrite();
    }
    
    bool BinaryStreamWriter::canSeek() const {
        return mStream->canSeek();
    }
    
    bool BinaryStreamWriter::seek(size_t pos) {
        return mStream->seek(pos);
    }
    
    size_t BinaryStreamWriter::read(uint8* buffer, size_t length) {
        return mStream->read(buffer, length);
    }
    
    size_t BinaryStreamWriter::write(const uint8* buffer, size_t length) {
        return mStream->write(buffer, length);
    }
    
    size_t BinaryStreamWriter::pos() const {
        return mStream->pos();
    }
    
    size_t BinaryStreamWriter::size() const {
        return mStream->size();
    }
    
    bool BinaryStreamWriter::isValid() const {
        return mStream->isValid();
    }
    
    void BinaryStreamWriter::close() {
        mStream->close();
    }
    
    StreamType BinaryStreamWriter::getStreamType() const {
        return mStream->getStreamType();
    }
    
} // namespace ukn
