//
//  RenderDevice.h
//  Project Unknown
//
//  Created by Robert Bu on 11/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_RenderDevice_h
#define Project_Unknown_RenderDevice_h

#include "Platform.h"
#include "PreDeclare.h"
#include "GraphicSettings.h"

namespace ukn {
    
    /**
     * Abstract GraphicrDevice Class
     * Represents a OpenGL or DirectX device depends on current platform 
     **/
    
    /**
     * Graphic Device Caps
     **/
    struct GraphicDeviceCaps {
        uint32 max_indices;
        uint32 max_vertices;
        
        uint32 max_texture_cube_map_size;
        uint32 max_texture_width;
        uint32 max_texture_height;
        
        uint32 max_pixel_texture_units;
        uint32 max_vertex_texture_units;
    };
    
    class GraphicDevice {
    public:
        GraphicDevice();
        ~GraphicDevice();
        
        static GraphicDevicePtr NullObject();
        
        virtual const ukn_wstring& description() const = 0;
        
        virtual WindowPtr createRenderWindow(const ukn_string& name, const RenderSettings& settings) = 0;
        virtual void onRenderBuffer(const RenderBufferPtr& buffer) = 0;
    };
    
} // namespace ukn



#endif
