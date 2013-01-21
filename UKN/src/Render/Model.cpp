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
        double sina = sinf(a);
        vertex.x = r * sina * cosf(b);
        vertex.y = r * sina * sinf(b);
        vertex.z = r * cosf(a);

        Vector3 normal = Vector3(vertex.x, vertex.y, vertex.z).normalize();
        vertex.setNormal(normal.x, normal.y, normal.z);

        double phi = acos(vertex.nz);
        vertex.v = phi / d_pi;
        if(vertex.nz == 1.0f || vertex.nz == -1.0f) {
            vertex.u = 0.f;
        } else {
            float u = acosf(MAX(MIN(vertex.ny / phi, 1.0), -1.0)) / (2.0 * d_pi);
            vertex.u = vertex.nx > 0.f ? u : 1 - u;
        }
    }

    ukn::RenderBufferPtr ModelLoader::BuildFromSphere(const mist::Sphere& sphere, uint32 slices) {

        ukn::GraphicFactory& gf = Context::Instance().getGraphicFactory();
        ukn::RenderBufferPtr renderBuffer = gf.createRenderBuffer();
        if(renderBuffer) {
            int w = 2 * slices, h = slices;
            double hStep = 180.0 / (h-1);
            double wStep = 360.0 / w;
            int length = w * h;

            ukn::GraphicBufferPtr vertexBuffer = gf.createVertexBuffer(GraphicBuffer::WriteOnly,
                GraphicBuffer::Dynamic,
                length,
                0,
                VertexUVNormal::Format());
            VertexUVNormal* vertices = (VertexUVNormal*)vertexBuffer->map();

            if(vertices) {
                double a, b;
                int i, j;
                for(a = 0.0, i = 0; i < h; i++, a += hStep)
                    for(b = 0.0, j = 0; j < w; j++, b += wStep) {
                        _get_sphere_vertex(sphere.radius(),
                            degree_to_radius(a),
                            degree_to_radius(b),
                            vertices[i * w + j]);
                    }
                    
                vertexBuffer->unmap();
                renderBuffer->bindVertexStream(vertexBuffer, VertexUVNormal::Format());
            } else {
                return RenderBufferPtr();
            }

            GraphicBufferPtr indexBuffer = gf.createIndexBuffer(GraphicBuffer::WriteOnly,
                GraphicBuffer::Dynamic,
                (h-1)*6 + (h-1)*(w-1)*6,
                0);

            uint32* indexes = (uint32*)indexBuffer->map();
            if(indexes) {
                uint32 index = 0;
                int i, j;
                for(i = 0; i < h-1; ++i) {
                    for(j = 0; j < w-1; ++j) {
                        indexes[index++] = i*w+j;
                        indexes[index++] = i*w+j+1;
                        indexes[index++] = (i+1)*w+j+1;

                        indexes[index++] = (i+1)*w+j+1;
                        indexes[index++] = (i+1)*w+j;
                        indexes[index++] = i*w+j;
                    }
                    indexes[index++] = i*w+j;
                    indexes[index++] = i*w;
                    indexes[index++] = (i+1)*w;

                    indexes[index++] = (i+1)*w;
                    indexes[index++] = (i+1)*w+j;
                    indexes[index++] = i*w+j;
                }
                indexBuffer->unmap();

                renderBuffer->bindIndexStream(indexBuffer);
                renderBuffer->useIndexStream(true);
            } else {
                return RenderBufferPtr();
            }

            renderBuffer->setRenderMode(RM_Triangle);
            return renderBuffer;
        }

        return RenderBufferPtr();
    }

}