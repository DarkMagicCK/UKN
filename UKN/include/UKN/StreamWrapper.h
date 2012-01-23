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
    class BinaryStreamWriter: public IStream {
    public:
        BinaryStreamWriter(const StreamPtr& stream);
        virtual ~BinaryStreamWriter();
        
        // IStream
        bool	canRead() const;
        bool	canWrite() const;
        bool 	canSeek() const;
    	
        bool    seek(size_t pos);
        size_t  read(uint8* buffer, size_t length);
        size_t  write(const uint8* buffer, size_t length);
        
        size_t  pos() const;
        size_t  size() const;
        bool    isValid() const;
        
        void close();
        
        StreamType getStreamType() const;
        
    public:
        void writeByte(uint8 val);
        void writeChar(char val);
        
        void writeInt16(int16 val);
        void writeInt32(int32 val);
        void writeInt64(int64 val);
        void writeUInt16(uint16 val);
        void writeUInt32(uint32 val);
        void writeUInt64(uint64 val);
        
    private:
        StreamPtr mStream;
    };
    
} // namespace ukn


#endif
