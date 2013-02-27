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
        vertex.position = Vector3(r * sina * cosf(b),
                                  r * sina * sinf(b),
                                  r * cosf(a));

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
            
            uint32 idx = 0;

            Vertex2D vertices[6* 6];
            
            float half4 = 0.25f;
            float half2 = 0.5f;
            float half43 = 0.75f;
            float half3 = 1.0f / 3;
            float half32 = 2.0f / 3;
            
            // front, 0123
            vertices[idx++].xyz = v[0]; vertices[idx++].xyz = v[1]; vertices[idx++].xyz = v[2];
            vertices[idx++].xyz = v[2]; vertices[idx++].xyz = v[3]; vertices[idx++].xyz = v[0];
            
            // left, 1265
            vertices[idx++].xyz = v[1]; vertices[idx++].xyz = v[2]; vertices[idx++].xyz = v[6];
            vertices[idx++].xyz = v[6]; vertices[idx++].xyz = v[5]; vertices[idx++].xyz = v[1];
            
            // back, 4567
            vertices[idx++].xyz = v[4]; vertices[idx++].xyz = v[5]; vertices[idx++].xyz = v[6];
            vertices[idx++].xyz = v[6]; vertices[idx++].xyz = v[7]; vertices[idx++].xyz = v[4];
            
            // right, 0374
            vertices[idx++].xyz = v[0]; vertices[idx++].xyz = v[3]; vertices[idx++].xyz = v[7];
            vertices[idx++].xyz = v[7]; vertices[idx++].xyz = v[4]; vertices[idx++].xyz = v[0];
            
            // top, 2376
            vertices[idx++].xyz = v[2]; vertices[idx++].xyz = v[3]; vertices[idx++].xyz = v[7];
            vertices[idx++].xyz = v[7]; vertices[idx++].xyz = v[6]; vertices[idx++].xyz = v[2];
            
            // bottom, 0154
            vertices[idx++].xyz = v[0]; vertices[idx++].xyz = v[1]; vertices[idx++].xyz = v[5];
            vertices[idx++].xyz = v[5]; vertices[idx++].xyz = v[4]; vertices[idx++].xyz = v[0];
            
            idx = 0;
            
            vertices[idx++].uv.set(half2, half32); vertices[idx++].uv.set(half4, half32); vertices[idx++].uv.set(half4, half3);
            vertices[idx++].uv.set(half4, half3); vertices[idx++].uv.set(half2, half3); vertices[idx++].uv.set(half2, half32);
            
            vertices[idx++].uv.set(half4, half32); vertices[idx++].uv.set(half4, half3); vertices[idx++].uv.set(0, half3);
            vertices[idx++].uv.set(0, half3); vertices[idx++].uv.set(0, half32); vertices[idx++].uv.set(half4, half32);
            
            vertices[idx++].uv.set(half43, half32); vertices[idx++].uv.set(1, half32); vertices[idx++].uv.set(1, half3);
            vertices[idx++].uv.set(1, half3); vertices[idx++].uv.set(half43, half3); vertices[idx++].uv.set(half43, half32);
            
            vertices[idx++].uv.set(half2, half32); vertices[idx++].uv.set(half2, half3); vertices[idx++].uv.set(half43, half3);
            vertices[idx++].uv.set(half43, half3); vertices[idx++].uv.set(half43, half32); vertices[idx++].uv.set(half2, half32);
            
            vertices[idx++].uv.set(half4, half3); vertices[idx++].uv.set(half2, half3); vertices[idx++].uv.set(half2, 0);
            vertices[idx++].uv.set(half2, 0); vertices[idx++].uv.set(half4, 0); vertices[idx++].uv.set(half4, half3);
            
            vertices[idx++].uv.set(half2, half32); vertices[idx++].uv.set(half4, half32); vertices[idx++].uv.set(half4, 1);
            vertices[idx++].uv.set(half4, 1); vertices[idx++].uv.set(half2, 1); vertices[idx++].uv.set(half2, half32);
            
            
            GraphicBufferPtr vertexBuffer = gf.createVertexBuffer(GraphicBuffer::None,
                                                                  GraphicBuffer::Static,
                                                                  6 * 6,
                                                                  vertices,
                                                                  Vertex2D::Format());
            if(vertexBuffer) {
                renderBuffer->bindVertexStream(vertexBuffer, Vertex2D::Format());
                
            }
        }
        return renderBuffer;
    }
    
    ModelLoader::ModelDataPtr ModelLoader::LoadFromPly(const mist::ResourcePtr& file) {
        mist::TextStreamReader reader(file->getResourceStream());
        while(reader.peek() != -1) {
            MistString magic = reader.readString();
            if(magic == L"ply") {
                MistString header = reader.readString();
                
                uint32 vertexCount = 0;
                uint32 indexCount = 0;
                float version = 0;
                
                struct VertexComponent {
                    enum Component {
                        X, Y, Z,
                        R, G, B,
                        NX, NY, NZ,
                        U, V
                    };
                    typedef std::vector<Component> ComponentVector;
                    ComponentVector components;
                    /* x y z r g b nx ny nz u v*/
                    bool vertex_flag[11];
                    
                    VertexComponent() {
                        memset(vertex_flag, 0, sizeof(vertex_flag));
                    }
                    
                    void add(Component comp) {
                        components.push_back(comp);
                        vertex_flag[comp] = true;
                    }
                    
                    bool hasXYZ() const {
                        return vertex_flag[0] || vertex_flag[1] || vertex_flag[2];
                    }
                    bool hasNormal() const {
                        return vertex_flag[3] || vertex_flag[4] || vertex_flag[5];
                    }
                    bool hasColor() const {
                        return vertex_flag[6] || vertex_flag[7] || vertex_flag[8];
                    }
                    bool hasUV() const {
                        return vertex_flag[10] || vertex_flag[9];
                    }
                    
                    ComponentVector::const_iterator begin() const { return components.begin(); }
                    ComponentVector::const_iterator end() const { return components.end(); }
                    
                } vertex_components;
                
                while(header != L"end_header") {
                    if(header == L"format") {
                        MistString format = reader.readString();
                        assert(format == L"ascii");
                        version = reader.readFloat();
                        
                    } else if(header == L"comment") {
                        reader.readLine();
                        
                    } else if(header == L"element") {
                        MistString element = reader.readString();
                        if(element == L"vertex") {
                            vertexCount = reader.readUInt32();
                            MistString prop = reader.readString();
                            
                            while(prop == L"property") {
                                MistString type = reader.readString();
                                MistString component = reader.readString();
                                if(component == L"x") { vertex_components.add(VertexComponent::X);
                                }
                                else if(component == L"y") { vertex_components.add(VertexComponent::Y);
                                }
                                else if(component == L"z") { vertex_components.add(VertexComponent::Z);
                                }
                                else if(component == L"red") { vertex_components.add(VertexComponent::R);
                                }
                                else if(component == L"green") { vertex_components.add(VertexComponent::G);
                                }
                                else if(component == L"blue") { vertex_components.add(VertexComponent::B);
                                }
                                else if(component == L"nx") { vertex_components.add(VertexComponent::NX);
                                }
                                else if(component == L"ny") { vertex_components.add(VertexComponent::NY);
                                }
                                else if(component == L"nz") { vertex_components.add(VertexComponent::NZ);
                                }
                                else if(component == L"u") { vertex_components.add(VertexComponent::U);
                                }
                                else if(component == L"v") { vertex_components.add(VertexComponent::V);
                                }
                                
                                prop = reader.readString();
                            }
                            continue;
                        } else if(element == L"face") {
                            indexCount = reader.readUInt32();
                        }
                    }
                    header = reader.readString();
                }
                
                ModelData* data = new ModelData();
                if(vertex_components.hasXYZ()) {
                    data->position.resize(vertexCount);
                }
                if(vertex_components.hasColor()) {
                    data->color.resize(vertexCount);
                }
                if(vertex_components.hasNormal()) {
                    data->normal.resize(vertexCount);
                }
                if(vertex_components.hasUV()) {
                    data->uv.resize(vertexCount);
                }
                
                /* begin data */
                for(uint32 i=0; i<vertexCount; ++i) {
                    for(const VertexComponent::Component& c: vertex_components) {
                        switch(c) {
                            case VertexComponent::X: data->position[i][0] = reader.readFloat(); break;
                            case VertexComponent::Y: data->position[i][1] = reader.readFloat(); break;
                            case VertexComponent::Z: data->position[i][2] = reader.readFloat(); break;
                            case VertexComponent::NX: data->normal[i][0] = reader.readFloat(); break;
                            case VertexComponent::NY: data->normal[i][1] = reader.readFloat(); break;
                            case VertexComponent::NZ: data->normal[i][2] = reader.readFloat(); break;
                            case VertexComponent::R: data->color[i][0] = reader.readFloat(); break;
                            case VertexComponent::G: data->color[i][1] = reader.readFloat(); break;
                            case VertexComponent::B: data->color[i][2] = reader.readFloat(); break;
                            case VertexComponent::U: data->uv[i][0] = reader.readFloat(); break;
                            case VertexComponent::V: data->uv[i][1] = reader.readFloat(); break;
                        }
                    }
                }
                for(uint32 i=0; i<indexCount; ++i) {
                    uint32 count = reader.readInt32();
                    for(uint32 j=0; j<count; ++j) {
                        /* triangle strip */
                        if(j >= 3) {
                            size_t size = data->indices.size();
                            data->indices.push_back(data->indices[size-2]);
                            data->indices.push_back(data->indices[size-1]);
                        }
                        data->indices.push_back(reader.readInt32());
                    }
                }
                
                return data;
            }
        }
        return ModelDataPtr();
    }
    
    ModelLoader::ModelDataPtr ModelLoader::LoadFromObj(const mist::ResourcePtr& file) {
        return ModelDataPtr();
    }
    
    ModelLoader::ModelDataPtr ModelLoader::LoadFromPlaneFile(const mist::ResourcePtr& file) {
        return ModelDataPtr();
    }
    
    /* Mesh */

    Mesh::Mesh(const ModelPtr& model, const UknString& name):
    mName(name),
    mMaterialId(0),
    mModel(model) {

    }

    Mesh::~Mesh() {

    }

    const UknString& Mesh::getName() const {
        return mName;
    }

    Box Mesh::getBound() const {
        return mBoundingBox;
    }

    RenderBufferPtr Mesh::getRenderBuffer() const {
        return mRenderBuffer;
    }

    int32 Mesh::getMaterialId() const {
        return mMaterialId;
    }

    void Mesh::setMaterailId(int32 mid) {
        mMaterialId = mid;
    }

    uint32 Mesh::getNumVertices() const {
        return mRenderBuffer->getVertexCount();
    }

    void Mesh::setNumVertices(uint32 num) {
        mRenderBuffer->setVertexCount(num);
    }

    uint32 Mesh::getNumTriangles() const {
        return mRenderBuffer->getIndexCount() / 3;
    }

    void Mesh::setNumTriangles(uint32 num) {
        mRenderBuffer->setIndexCount(num * 3);
    }

    uint32 Mesh::getNumIndicies() const {
        return mRenderBuffer->getIndexCount();
    }

    void Mesh::setNumIndicies(uint32 num) {
        mRenderBuffer->setIndexCount(num);
    }

    uint32 Mesh::getVertexStartIndex() const {
        return mRenderBuffer->getVertexStartIndex();
    }

    void Mesh::setVertexStartIndex(uint32 index) {
        mRenderBuffer->setVertexStartIndex(index);
    }

    uint32 Mesh::getIndexStartIndex() const {
        return mRenderBuffer->getIndexStartIndex();
    }

    void Mesh::setIndexStartIndex(uint32 index) {
        mRenderBuffer->setIndexStartIndex(index);
    }
        
    void Mesh::setIndexStream(const GraphicBufferPtr& index_stream) {
        mRenderBuffer->bindIndexStream(index_stream);
    }

    void Mesh::setVertexStream(const GraphicBufferPtr& vtx_stream, const vertex_elements_type& format) {
        mRenderBuffer->bindVertexStream(vtx_stream, format);
    }

    void Mesh::buildInfo() {
        ModelPtr model = mModel.lock();

        /* retrieve textures from material */
        const MaterialPtr& mat = model->getMaterial(mMaterialId);
        for(std::pair<std::string, std::string>& texel: mat->textures) {
            TexturePtr tex = model->getTexture(texel.second);
            if(tex) {
                if(texel.first == "diffuse")
                    mDiffuseTex = tex;
                else if(texel.first == "specular")
                    mSpecularTex = tex;
                else if(texel.first == "normal" || texel.first == "bump")
                    mNormalTex = tex;
                else if(texel.first == "height")
                    mHeightTex = tex;
                else if(texel.first == "self_illumination")
                    mEmitTex = tex;
            }
        }
    }

    /* Model */

    Model::Model(const UknString& name):
    mName(name) {

    }

    Model::~Model() {

    }

    const UknString& Model::getName() const {
        return mName;
    }

    RenderBufferPtr Model::getRenderBuffer() const {
        return mRenderBuffer;
    }

    void Model::onRenderBegin() {
        for(MeshPtr& mesh: mMeshes) {
            mesh->onRenderBegin();
        }
    }

    void Model::onRenderEnd() {
        for(MeshPtr& mesh: mMeshes) {
            mesh->onRenderEnd();
        }
    }

    const MeshPtr& Model::getMesh(size_t mid) const {
        return mMeshes[mid];
    }

    size_t Model::getMeshCount() const {
        return mMeshes.size();
    }

    const MaterialPtr& Model::getMaterial(size_t mid) const {
        return mMaterials[mid];
    }

    size_t Model::getMaterialCount() const {
        return mMaterials.size();
    }

    const TexturePtr& Model::getTexture(const std::string& name) {
        auto it = mTexturePool.find(name);
        if(it == mTexturePool.end()) {
            TexturePtr tex = Context::Instance().getGraphicFactory().load2DTexture(
                ResourceLoader::Instance().loadResource(string::StringToWString(name)),
                false);
            it = mTexturePool.insert(std::make_pair(name, tex)).first;
        }
        return it->second;
    }

    void Model::updateBoundingBox() {
        mBoundingBox = AABB3();
        for(MeshPtr& mesh: mMeshes) {
            mBoundingBox.extend(mesh->getBound());
        }
    }

    Box Model::getBound() const {
        return mBoundingBox;
    }


}