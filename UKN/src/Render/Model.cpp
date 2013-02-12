#include "UKN/Model.h"
#include "UKN/Context.h"
#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"
#include "UKN/RenderBuffer.h"

namespace ukn {

    using namespace mist;

#define MAX(a, b) (a > b) ? (a) : (b)
#define MIN(a, b) (a < b) ? (a) : (b)

    inline void _get_sphere_vertex(double r, double a, double b, VertexUVNormal& vertex) {
        double sina = sin(a);
        vertex.position = Vector3(r * sina * cos(b),
                                  r * sina * sin(b),
                                  r * cos(a));

        vertex.normal = vertex.position.normalize();

        /*
        double phi = acos(vertex.nz);
        vertex.v = phi / d_pi;
        if(vertex.nz == 1.0f || vertex.nz == -1.0f) {
            vertex.u = 0.f;
        } else {
            float u = acosf(MAX(MIN(vertex.ny / phi, 1.0), -1.0)) / (2.0 * d_pi);
            vertex.u = vertex.nx > 0.f ? u : 1 - u;
        }*/
        vertex.uv = Vector2(0.5 + asin(vertex.normal.x()) / d_pi,
                            0.5 + asin(vertex.normal.y()) / (d_pi));
    }

    ukn::RenderBufferPtr ModelLoader::BuildFromSphere(const mist::Sphere& sphere, uint32 slices) {

        ukn::GraphicFactory& gf = Context::Instance().getGraphicFactory();
        ukn::RenderBufferPtr renderBuffer = gf.createRenderBuffer();
        if(renderBuffer) {
            int w = 2 * slices, h = slices;
            double hStep = 180.0 / (h-1);
            double wStep = 360.0 / w;
           
            std::vector<VertexUVNormal> vertices;
            {
                double a, b;
                int i, j;
                for(a = 0.0, i = 0; i < h; i++, a += hStep)
                    for(b = 0.0, j = 0; j < w; j++, b += wStep) {
                        VertexUVNormal vertex;
                        _get_sphere_vertex(sphere.radius(),
                                           math::degree_to_radius(a),
                                           math::degree_to_radius(b),
                                           vertex);
                        vertices.push_back(vertex);
                    }
            }
            
            ukn::GraphicBufferPtr vertexBuffer = gf.createVertexBuffer(
                GraphicBuffer::None,
                GraphicBuffer::Static,
                (uint32)vertices.size(),
                &vertices[0],
                VertexUVNormal::Format());
            renderBuffer->bindVertexStream(vertexBuffer, VertexUVNormal::Format());

            std::vector<uint32> indices;
            {
                int i, j;
                for(i = 0; i < h-1; ++i) {
                    for(j = 0; j < w-1; ++j) {
                        indices.push_back(i*w+j);
                        indices.push_back(i*w+j+1);
                        indices.push_back((i+1)*w+j+1);

                        indices.push_back((i+1)*w+j+1);
                        indices.push_back((i+1)*w+j);
                        indices.push_back(i*w+j);
                    }
                    indices.push_back(i*w+j);
                    indices.push_back(i*w);
                    indices.push_back((i+1)*w);

                    indices.push_back((i+1)*w);
                    indices.push_back((i+1)*w+j);
                    indices.push_back(i*w+j);
                }
            } 
            GraphicBufferPtr indexBuffer = gf.createIndexBuffer(
                GraphicBuffer::None,
                GraphicBuffer::Static,
                (uint32)indices.size(),
                &indices[0]);
            
            renderBuffer->bindIndexStream(indexBuffer);
            renderBuffer->useIndexStream(true);
            renderBuffer->setRenderMode(RM_Triangle);
            return renderBuffer;
        }

        return RenderBufferPtr();
    }

    ukn::RenderBufferPtr ModelLoader::BuildFromBox(const mist::Box& box) {
        ukn::GraphicFactory& gf = Context::Instance().getGraphicFactory();
        ukn::RenderBufferPtr renderBuffer = gf.createRenderBuffer();
        if(renderBuffer) {
            Vector3 min = box.getMin();
            Vector3 max = box.getMax();
            float3 v[8] = {
                float3(min.x(), min.y(), min.z()), // 0
                float3(min.x(), max.y(), min.z()), // 1
                float3(max.x(), max.y(), min.z()), // 2
                float3(max.x(), min.y(), min.z()), // 3
            
                float3(max.x(), min.y(), max.z()), // 4
                float3(min.x(), min.y(), max.z()), // 5
                float3(min.x(), max.y(), max.z()), // 6
                float3(max.x(), max.y(), max.z()) // 7
            };
        }
        return renderBuffer;
    }

    ukn::RenderBufferPtr ModelLoader::LoadFromCSV(const mist::ResourcePtr& file, const vertex_elements_type& elements) {
        mist::TextStreamReader reader(file->getResourceStream());
        while(!reader.peek() != -1) {
            MistString line = reader.readLine();
            StringTokenlizer tokens(line, L",");
            for(vertex_elements_type::const_iterator it = elements.begin(), end = elements.end();
                it != end;
                ++it) {
                if(it-> format == EF_Float) {

                }
            }
        }
        return RenderBufferPtr();
    }
}