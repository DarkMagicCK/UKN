//
//  RenderHelper.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/21/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "UKN/GraphicContext.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/Vertex.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicFactory.h"

namespace ukn {
    
    GraphicContext::GraphicContext() {
        

    }
    
    GraphicContext::~GraphicContext() {
        
    }
    
    const UknString& GraphicContext::getName() const {
        static UknString name (L"GraphicContext");
        return name;
    }
    
    Box GraphicContext::getBound() const {
        return Box();
    }
    
    RenderBufferPtr GraphicContext::getRenderBuffer() const {
        return mRenderBuffer;
    }
    
    void GraphicContext::onRenderBegin() {
        
    }
    
    void GraphicContext::onRenderEnd() {
        
    }
    
    void GraphicContext::render() {
        
    }
    
    
} // namespace ukn
