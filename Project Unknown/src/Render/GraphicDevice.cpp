//
//  GraphicDevice.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GraphicDevice.h"

namespace ukn {
    
    class NullGraphicDevice: public GraphicDevice {
        
    };
    
    GraphicDevicePtr GraphicDevice::NullObject() {
        static GraphicDevicePtr static_ptr = MakeSharedPtr<NullGraphicDevice>();
        return static_ptr;
    }
    
    
} // namespace ukn
