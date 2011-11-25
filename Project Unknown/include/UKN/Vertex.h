//
//  Vertex.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Vertex_h
#define Project_Unknown_Vertex_h

#include "Platform.h"

namespace ukn {
    
    enum VertexFormatFlag {
        VF_XYZ       = 1UL, 
        VF_UV        = 1UL << 1,
        VF_Color0    = 1UL << 2, /* Ambient */
        VF_Color1    = 1UL << 3, /* Specular */
        VF_Normal    = 1UL << 4,
    };
    
    enum VertexElementType {
        VE_Float          = 1,
        VE_UnsignedByte,
        VE_Int,
        VE_UnsignedInt,
    };
    
    struct VertexFormat {
        VertexFormat():
        mFlag(0),
        mOffsetXYZ(0),
        mOffsetUV(0),
        mOffsetColor0(0),
        mOffsetColor1(0),
        mOffsetNormal(0) { }
        
        VertexFormat(uint32 flag, uint32 xyz, uint32 uv, uint32 c0, uint32 c1, uint32 nor):
        mFlag(flag),
        mOffsetXYZ(xyz),
        mOffsetUV(uv),
        mOffsetColor0(c0),
        mOffsetColor1(c1),
        mOffsetNormal(nor) { }
        
        VertexFormat& xyz(uint32 offset) {
            this->mFlag |= VF_XYZ;
            this->mOffsetXYZ = offset;
            return *this;
        }
        
        VertexFormat& uv(uint32 uv) {
            this->mFlag |= VF_UV;
            this->mOffsetUV = uv;
            return *this;
        }
        
        VertexFormat& color0(uint32 color0) {
            this->mFlag |= VF_Color0;
            this->mOffsetColor0 = color0;
            return *this;
        }
        
        VertexFormat& color1(uint32 color1) {
            this->mFlag |= VF_Color1;
            this->mOffsetColor1 = color1;
            return *this;
        }
        
        VertexFormat& normal(uint32 normal) {
            this->mFlag |= VF_Normal;
            this->mOffsetNormal = normal;
            return *this;
        }
        
        uint32 totalSize() const {
            uint32 size = 0;
            size += (mFlag & VF_XYZ) ? sizeof(float) * 3 : 0;
            size += (mFlag & VF_UV) ? sizeof(float) * 2 : 0;
            size += (mFlag & VF_Color0) ? sizeof(uint32) : 0;
            size += (mFlag & VF_Color1) ? sizeof(uint32) : 0;
            size += (mFlag & VF_Normal) ? sizeof(float) * 3 : 0;
            return size;
        }
        
        static uint32 TotalSizeOf(uint32 flag) {
            uint32 size = 0;
            size += (flag & VF_XYZ) ? sizeof(float) * 3 : 0;
            size += (flag & VF_UV) ? sizeof(float) * 2 : 0;
            size += (flag & VF_Color0) ? sizeof(uint32) : 0;
            size += (flag & VF_Color1) ? sizeof(uint32) : 0;
            size += (flag & VF_Normal) ? sizeof(float) * 3 : 0;
            return size;
        }
        
        uint32 mFlag;
        uint32 mOffsetXYZ, mOffsetUV, mOffsetColor0, mOffsetColor1, mOffsetNormal;
        
        friend bool operator==(const VertexFormat& lhs, const VertexFormat& rhs) {
            return (lhs.mFlag == rhs.mFlag) &&
                    (lhs.mOffsetXYZ     == rhs.mOffsetXYZ)      &&
                    (lhs.mOffsetUV      == rhs.mOffsetUV)       &&
                    (lhs.mOffsetColor0  == rhs.mOffsetColor0)   &&
                    (lhs.mOffsetColor1  == rhs.mOffsetColor1)   &&
                    (lhs.mOffsetNormal  == rhs.mOffsetNormal);
        }
    };
    
    /**
     * vertex format for 2d textures
     **/
    
    struct Vertex2D {
        float x, y, z;
        float u, v;
        uint32 color;
        
        Vertex2D():
        x(0.f), y(0.f), z(0.f),
        u(0.f), v(0.f),
        color(0xffffffff) { }
        
        static VertexFormat& Format() {
            static VertexFormat static_vertex2d_format(VF_XYZ | VF_UV | VF_Color0,
                                                       0,
                                                       sizeof(float) * 3,
                                                       sizeof(float) * 5,
                                                       0,
                                                       0);
            return static_vertex2d_format;
        }
        
        static uint32 FormatFlag() {
            return VF_XYZ | VF_UV | VF_Color0;
        }
    };
    
} // namespace ukn

#endif
