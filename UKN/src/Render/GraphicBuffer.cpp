//
//  GraphicBuffer.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/GraphicBuffer.h"

namespace ukn {
    
    class NullGraphicBuffer: public GraphicBuffer {
    public:
        NullGraphicBuffer():
        GraphicBuffer(ReadWrite, Static) {
            
        }
        
        void* map() {
            return 0;
        }
        
        void unmap() {
            
        }
        
        void activate() {
            
        }
        
        void deactivate() {
            
        }
        
        uint32 count() const {
            return 0;
        }
        
        void resize(uint32 desired_count) {
            
        }
        
        bool isInMemory() const {
            return false;
        }

        void copyBuffer(const GraphicBufferPtr& rhs) {

        }
    };
        
    GraphicBufferPtr GraphicBuffer::NullObject() {
        static GraphicBufferPtr static_ptr = MakeSharedPtr<NullGraphicBuffer>();
        return static_ptr;
    }
    
    GraphicBuffer::GraphicBuffer(Access a, Usage u):
    mAccessHint(a),
    mUsage(u) {
        
    }
    
    GraphicBuffer::~GraphicBuffer() {
        
    }
    
    GraphicBuffer::Access GraphicBuffer::access() const { 
        return mAccessHint;
    }
    
    GraphicBuffer::Usage GraphicBuffer::usage() const { 
        return mUsage;
    }
    
    
} // namespace ukn