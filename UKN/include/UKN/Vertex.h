//
//  Vertex.h
//  Project Unknown
//
//  Created by Robert Bu on 11/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Vertex_h
#define Project_Unknown_Vertex_h

#include "mist/Platform.h"
#include "mist/Logger.h"
#include "mist/Util.h"
#include "ukn/GraphicSettings.h"
#include "ukn/PreDeclare.h"

namespace ukn {

    using namespace mist;
    
    enum VertexUsage {
        VU_Position  = 1UL, 
        VU_UV        = 1UL << 1,
        VU_Diffuse   = 1UL << 2, /* Diffuse */
        VU_Specular  = 1UL << 3, /* Specular */
        VU_Normal    = 1UL << 4,
        VU_Tangent   = 1UL << 5,
        VU_Binormal  = 1UL << 6,
        VU_BlendWeight = 1UL << 7,
        VU_BlendIndices = 1UL << 8,
        VU_PSize     = 1UL << 9,

        VU_Unknown = -1,
    };

    struct VertexElement {
        VertexUsage usage;
        ElementFormat format;
        uint8 usage_index;

        VertexElement(VertexUsage _usage, ElementFormat _type, uint32 _usage_index):
            usage(_usage),
            format(_type),
            usage_index(_usage_index) {

        }

        uint32 size() const {
            return GetElementSize(format);
        }

        friend bool operator==(const VertexElement& lhs, const VertexElement& rhs) {
            return lhs.usage == rhs.usage && 
                   lhs.format == rhs.format &&
                   lhs.usage_index == rhs.usage_index;
        }
    };
    
    typedef std::vector<VertexElement> vertex_elements_type;

    inline uint32 GetVertexElementsTotalSize(const vertex_elements_type& elements) {
        uint32 ret = 0;
        for(vertex_elements_type::const_iterator it = elements.begin(),
            end = elements.end();
            it != end;
            ++it) {
                ret += it->size();
        }
        return ret;
    }

    template<typename _Tuple>
    vertex_elements_type TupleToVertexElements(const _Tuple& t) {
        if(t.empty()) return vertex_elements_type();

        vertex_elements_type ret;
        ret.push_back(t.get0());

        vertex_elements_type ret2(TupleToVertexElements(t.tail()));
        ret.insert(ret.end(), ret2.begin(), ret2.end());
        return ret;
    }

    struct VertexElementsBuilder {
        VertexElementsBuilder& add(const VertexElement& element) {
            elements.push_back(element);
            return *this;
        }

        const vertex_elements_type& to_vector() const {
            return elements;
        }

        vertex_elements_type elements;
    };

    /* a helper class that maps a vertex buffer in constructor
        and unmap it in destrcutor
        
     */
    struct VertexMapper {
        VertexMapper(const GraphicBufferPtr& _buffer, const vertex_elements_type& _elements);
        ~VertexMapper();

        friend struct VertexIterator;
        struct VertexIterator {
            VertexIterator();
            VertexIterator(const VertexIterator& rhs);
            VertexIterator(void* ptr, VertexMapper* parent);

            VertexIterator& operator++();
            VertexIterator operator=(const VertexIterator& rhs);

            bool operator!=(const VertexIterator& rhs) const;
            bool operator==(const VertexIterator& rhs) const;

            void* operator*() const;
            void* curr() const;

            VertexUsage currUsage() const;

            // advance to next element in the vertex
            void nextElement();

            void* getElement(VertexUsage usage, uint32 index = 0);
           
            void* base_ptr;
            void* curr_ptr;
            uint32 index;
            VertexMapper* parent;
        };
        
        typedef VertexIterator iterator;

        iterator begin();
        iterator end();

        // total size of vertex elements
        // = vertex_elements_type.size()
        uint32 element_count();

        void* data;
        GraphicBufferPtr buffer;
        const vertex_elements_type& elements;
        uint32 element_size;
    };
  
    /**
     * vertex format for 2d textures
     **/
    
    struct Vertex2D {
        // GL_T2F_C4UB_V3F
        Vector2 uv;
        uint32 color;
        Vector3 xyz;
        
        Vertex2D():
        uv(float2(0, 0)),
        color(0xffffffff),        
        xyz(float3(0, 0, 0))
        { }
        
        static vertex_elements_type& Format() {
            static vertex_elements_type static_format = VertexElementsBuilder()
                                                        .add(VertexElement(VU_UV, EF_Float2, 0))
                                                        .add(VertexElement(VU_Diffuse, EF_RGBA8, 0))
                                                        .add(VertexElement(VU_Position, EF_Float3, 0))
                                                        .to_vector();
            return static_format;
        }
    };

    struct VertexUVNormal {
        float u, v;
        float x, y, z;
        float nx, ny, nz;

        VertexUVNormal():
        u(0.f), v(0.f),
        x(0.f), y(0.f), z(0.f),
        nx(0.f), ny(0.f), nz(0.f) {

        }

        void setNormal(float _x, float _y, float _z) {
            nx = _x; ny = _y; nz = _z;
        }
        void setPosition(float _x, float _y, float _z) {
            x = _x; y = _y; z = _z;
        }

        static vertex_elements_type& Format() {
            static vertex_elements_type static_format = VertexElementsBuilder()
                                                        .add(VertexElement(VU_UV, EF_Float2, 0))
                                                        .add(VertexElement(VU_Position, EF_Float3, 0))
                                                        .add(VertexElement(VU_Normal, EF_Float3, 0))
                                                        .to_vector();
            return static_format;
        }
    };
    
} // namespace ukn

#endif
