#include "UKN/Model.h"
#include "UKN/Context.h"
#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"
#include "UKN/RenderBuffer.h"

#include "mist/Resource.h"
#include "mist/FileUtil.h"
#include "mist/ConfigParser.h"
#include "mist/Convert.h"

namespace ukn {

    using namespace mist;

#define MAX(a, b) (a > b) ? (a) : (b)
#define MIN(a, b) (a < b) ? (a) : (b)

    inline void _get_sphere_vertex(double r, double a, double b, VertexUVNormal& vertex) {
        double sina = sin(a);
        vertex.position = Vector3((float)(r * sina * cos(b)),
                                  (float)(r * sina * sin(b)),
                                  (float)(r * cos(a)));

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
                                           (real)math::degree_to_radius(a),
                                           (real)math::degree_to_radius(b),
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

            Vertex2D vertices[6 * 6];
            
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
    
    ModelLoader::ModelDataPtr ModelLoader::LoadFromPly(const mist::ResourcePtr& file, bool calculate_normal) {
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
                    
                    vertex_elements_type format;

                    VertexComponent() {
                        memset(vertex_flag, 0, sizeof(vertex_flag));
                    }
                    
                    void add(Component comp) {
                        components.push_back(comp);
                        vertex_flag[comp] = true;

                        switch(comp) {
                        case Z:
                            format.push_back(VertexElement(VU_Position, EF_Float3, 0));
                            break;
                        case NZ:
                            format.push_back(VertexElement(VU_Normal, EF_Float3, 0));
                            break;
                        case B:
                            format.push_back(VertexElement(VU_Diffuse, EF_RGBA8, 0));
                            break;
                        case V:
                            format.push_back(VertexElement(VU_UV, EF_Float2, 0));
                            break;
                        }
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
                
                /* begin data */
                std::vector<float3> position;
                std::vector<float3> normal;
                std::vector<float3> color;
                std::vector<float2> uv;

                position.resize(vertexCount);
                normal.resize(vertexCount);
                if(vertex_components.hasColor())
                    color.resize(vertexCount);
                uv.resize(vertexCount);
                for(uint32 i=0; i<vertexCount; ++i) {
                    for(const VertexComponent::Component& c: vertex_components) {
                        switch(c) {
                            case VertexComponent::X: position[i][0] = reader.readFloat(); break;
                            case VertexComponent::Y: position[i][1] = reader.readFloat(); break;
                            case VertexComponent::Z: position[i][2] = reader.readFloat(); break;
                            case VertexComponent::NX: normal[i][0] = reader.readFloat(); break;
                            case VertexComponent::NY: normal[i][1] = reader.readFloat(); break;
                            case VertexComponent::NZ: normal[i][2] = reader.readFloat(); break;
                            case VertexComponent::R: color[i][0] = reader.readFloat(); break;
                            case VertexComponent::G: color[i][1] = reader.readFloat(); break;
                            case VertexComponent::B: color[i][2] = reader.readFloat(); break;
                            case VertexComponent::U: uv[i][0] = reader.readFloat(); break;
                            case VertexComponent::V: uv[i][1] = reader.readFloat(); break;
                        }
                    }
                }
              
                if(!vertex_components.hasUV()) {
                    std::fill(uv.begin(), uv.end(), float2(0, 0));
                }
                
                ModelData* data = new ModelData();

                std::vector<float3> n_position;
                std::vector<float2> n_uv;
                std::vector<float3> n_color;

                std::vector<uint32>& index_data = data->index_data;
                for(uint32 i=0; i<indexCount; ++i) {
                    uint32 count = reader.readInt32();
                    for(uint32 j=0; j<count; ++j) {
                        /* triangle strip */
                        if(j >= 3) {
                            size_t size = index_data.size();
                            index_data.push_back(index_data[size-2]);
                            index_data.push_back(index_data[size-1]);

                            if(calculate_normal && !vertex_components.hasNormal()) {
                                n_position.push_back(position[index_data[size-2]]);
                                n_position.push_back(position[index_data[size-1]]);
                                
                                n_uv.push_back(uv[index_data[size-2]]);
                                n_uv.push_back(uv[index_data[size-1]]);
                                
                                if(vertex_components.hasColor()) {
                                    n_color.push_back(color[index_data[size-2]]);
                                    n_color.push_back(color[index_data[size-1]]);
                                }
                            }
                        }
                        uint32 index = reader.readInt32();
                        index_data.push_back(index);

                        if(calculate_normal && !vertex_components.hasNormal()) {
                            n_position.push_back(position[index]);
                            n_uv.push_back(uv[index]);

                            if(vertex_components.hasColor())
                                n_color.push_back(color[index]);
                        }
                    }
                }
                
                data->vertex_data.push_back(std::vector<uint8>());
                std::vector<uint8>& vert_data = data->vertex_data.back();

                /* ply sometimes does not contain normal data, calculate manually here */
                if(calculate_normal && !vertex_components.hasNormal()) {
                    normal.resize(n_position.size());

                    // calculate normals
                    ukn::uint32 index = 0;
                    for(size_t i=0; i<index_data.size() / 3; ++i) {
                        ukn::float3& p1 = n_position[index];
                        ukn::float3& p2 = n_position[index+1];
                        ukn::float3& p3 = n_position[index+2];

                        ukn::float3 u = p2 - p1;
                        ukn::float3 v = p3 - p1;
                        ukn::float3 n = math::cross(u, v);

                        normal[index] = normal[index+1] = normal[index+2] = n;
                        index += 3;
                    }
                }

                bool new_data = (calculate_normal & !vertex_components.hasNormal());
                
                std::vector<float3>& position_source = new_data ? n_position: position;
                std::vector<float3>& normal_source = normal;
                std::vector<float2>& uv_source = new_data ? n_uv: uv;
                std::vector<float3>& color_source = new_data ? n_color: color;
                
                data->vertex_format = vertex_components.format;
                if(new_data) 
                    data->vertex_format = VertexUVNormal::Format();


             //   vert_data.resize(GetVertexElementsTotalSize(vertex_components.format) * position_source.size());
                for(size_t i=0; i<position_source.size(); ++i) {
                    
                    for(VertexElement& element: data->vertex_format) {
                        switch(element.usage) {
                        case VU_Position:
                            vert_data.insert(vert_data.end(),
                                                (uint8*)position_source[i].value,
                                                (uint8*)(position_source[i].value + 3));
                            break;
                        case VU_Normal:
                            vert_data.insert(vert_data.end(),
                                                (uint8*)normal_source[i].value,
                                                (uint8*)(normal_source[i].value + 3));
                            break;
                        case VU_UV:
                            vert_data.insert(vert_data.end(),
                                                (uint8*)uv_source[i].value,
                                                (uint8*)(uv_source[i].value + 2));
                            break;
                        case VU_Diffuse:
                            float3& rgb = color_source[i];
                            uint32 c = COLOR_RGBA(rgb[0] / 255.f,
                                                  rgb[1] / 255.f,
                                                  rgb[2] / 255.f,
                                                  1);
                            vert_data.insert(vert_data.end(),
                                             (uint8*)&c,
                                             ((uint8*)&c) + 4);
                            break;
                        }
                    }
                }

                ModelData::MeshData mesh;
                mesh.num_indices = index_data.size();
                mesh.start_index = 0;
                mesh.num_vertices = position_source.size();
                mesh.start_vertex = 0;
                mesh.material_id = 0;
                mesh.name = file->getName();
                data->meshes.push_back(mesh);

                data->vertex_count = (uint32)position_source.size();

                return SharedPtr<ModelData>(data);
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

    ModelLoader::ModelDataPtr ModelLoader::LoadFromMeshML(const mist::ResourcePtr& file) {
        // load from xml
        ConfigParserPtr xml = ConfigParser::MakeParser(file);
        if(xml) {

            if(xml->toNode(L"model")) {
                xml->toFirstChild();
                ModelData* data = new ModelData;;

                do {
                    MistString node = xml->getCurrentNodeName();

                    if(node == L"materials_chunk") {
                        // load materials
                        xml->toFirstChild();

                        do {

                            MistString matNodeName = xml->getCurrentNodeName();
                            if(matNodeName == L"material") {

                                Material* mat = new Material();
                                mat->ambient = Convert::ToFloat3(xml->getString(L"ambient"));
                                mat->diffuse = Convert::ToFloat3(xml->getString(L"diffuse"));
                                mat->specular = Convert::ToFloat3(xml->getString(L"specular"));
                                mat->emit = Convert::ToFloat3(xml->getString(L"emit"));
                                mat->opacity = (float)Convert::ToDouble(xml->getString(L"opacity"));
                                mat->specular = Convert::ToFloat3(xml->getString(L"specular"));
                                mat->specular_power = (float)Convert::ToDouble(xml->getString(L"specular_level"));
                                mat->shininess = (float)Convert::ToDouble(xml->getString(L"shininess"));

                                data->materials.push_back(SharedPtr<Material>(mat));

                            }

                        } while(xml->toNextChild());

                        xml->toParent();
                    
                    } else if(node == L"meshes_chunk") {

                        xml->toFirstChild();

                        do {

                            MistString meshNodeName = xml->getCurrentNodeName();
                            if(meshNodeName == L"mesh") {

                                ModelData::MeshData mesh;
                                mesh.name = xml->getString(L"name");
                                mesh.material_id = Convert::ToInt32(xml->getString(L"mtl_id"));

                                xml->toFirstChild();

                                do {

                                    MistString meshDataNodeName = xml->getCurrentNodeName();
                                    if(meshDataNodeName == L"vertices_chunk") {

                                        xml->toFirstChild();

                                        do {

                                            MistString vertexNodeName = xml->getCurrentNodeName();

                                            if(vertexNodeName == L"pos_bb") {
                                                mesh.bounding_box = Box(
                                                    Convert::ToFloat3(xml->getString(L"min")),
                                                    Convert::ToFloat3(xml->getString(L"max"))
                                                );
                                            } else if(vertexNodeName == L"tc_bb") {
                                                // continue
                                            } else if(vertexNodeName == L"vertex") {

                                                

                                            }

                                        } while(xml->toNextChild());

                                        xml->toParent();

                                    }

                                } while(xml->toNextChild());

                                xml->toParent();
                            }

                        } while(xml->toNextChild());

                        xml->toParent();
                    }

                } while(xml->toNextChild());
               
            }

        } else {
            log_error(L"error loading xml data");
        }
        return ModelDataPtr();
    }

    ModelLoader::ModelDataPtr ModelLoader::LoadFromCollada(const mist::ResourcePtr& file) {
        return ModelDataPtr();
    }

    ModelPtr ModelLoader::LoadModel(const UknString& name, uint32 access_hint) {
        if(name.find(L".ply") != UknString::npos) {
            ModelDataPtr data = ModelLoader::LoadFromPly(
                ResourceLoader::Instance().loadResource(name), 
                true
            );
            if(data) {
                ModelPtr model = MakeSharedPtr<Model>(Path::GetFileNameWithoutExtension(name));
                
                ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
     
                ukn::GraphicBufferPtr vertexBuffer = gf.createVertexBuffer(ukn::GraphicBuffer::None,
                                                                           ukn::GraphicBuffer::Static,
                                                                           data->vertex_count,
                                                                           &data->vertex_data[0][0],
                                                                           data->vertex_format);
                ukn::GraphicBufferPtr indexBuffer = gf.createIndexBuffer(ukn::GraphicBuffer::None,
                                                                         ukn::GraphicBuffer::Static,
                                                                         data->index_data.size(),
                                                                         &data->index_data[0]);
                ukn::RenderBufferPtr buffer = gf.createRenderBuffer();
                buffer->bindVertexStream(vertexBuffer, ukn::VertexUVNormal::Format());
                buffer->bindIndexStream(indexBuffer);
                buffer->setRenderMode(ukn::RM_Triangle);
                
                model->setRenderBuffer(buffer);

                MeshPtr mesh = MakeSharedPtr<Mesh>(model, L"main");
                mesh->setRenderBuffer(buffer);

                MaterialPtr default_mat = MakeSharedPtr<Material>();
                default_mat->ambient = float3(1, 1, 1);
                default_mat->diffuse = float3(1, 1, 1);
                default_mat->emit = float3(0, 0, 0);
                default_mat->opacity = 1.f;
                default_mat->shininess = 10.f;
                default_mat->specular = float3(0, 0, 0);
                default_mat->specular_power = 16.f;
                default_mat->textures.push_back(std::make_pair("diffuse",
                                                               "white"));

                model->addMaterial(default_mat);
                model->addMesh(mesh);

                uint32 white(0xFFFFFFFF);
                model->mTexturePool.insert(std::make_pair("white",
                                                          gf.create2DTexture(1, 1, 0, EF_RGBA8, (uint8*)&white)));

                mesh->buildInfo();

                return model;

            } else {
                log_error(L"error loading model data");
            }
        }
        

        return ModelPtr();
    }

    ModelPtr ModelLoader::CreateModel(const ModelDataPtr& data) {
        return ModelPtr();
    }

    bool ModelLoader::SaveModel(const ModelPtr& model, const UknString& save_to, const std::string& mesh_name) {
        RenderablePtr rp = model;
        return false;
    }

    bool ModelLoader::SaveModel(const ModelDataPtr& model, const UknString& save_to, const std::string& mesh_name) {
        return false;
    }
    
    /* Mesh */

    Mesh::Mesh(const ModelPtr& model, const UknString& name):
    mName(name),
    mMaterialId(0),
    mModel(model),
    mVertexStartIndex(0),
    mIndexStartIndex(0),
    mNumVertices(0),
    mNumIndices(0) {

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
        mRenderBuffer->setVertexStartIndex(mVertexStartIndex);
        mRenderBuffer->setVertexCount(mNumVertices);
        mRenderBuffer->setIndexStartIndex(mIndexStartIndex);
        mRenderBuffer->setIndexCount(mNumIndices);
      
        return mRenderBuffer;
    }

    int32 Mesh::getMaterialId() const {
        return mMaterialId;
    }

    void Mesh::setMaterailId(int32 mid) {
        mMaterialId = mid;
    }

    uint32 Mesh::getNumVertices() const {
        return mNumVertices;
    }

    void Mesh::setNumVertices(uint32 num) {
        mNumVertices= num;
    }

    uint32 Mesh::getNumTriangles() const {
        return mNumIndices / 3;
    }

    void Mesh::setNumTriangles(uint32 num) {
        mNumIndices = num * 3;
    }

    uint32 Mesh::getNumIndicies() const {
        return mNumIndices;
    }

    void Mesh::setNumIndicies(uint32 num) {
        mNumIndices = num;
    }

    uint32 Mesh::getVertexStartIndex() const {
        return mVertexStartIndex;
    }

    void Mesh::setVertexStartIndex(uint32 index) {
        mVertexStartIndex = index;
    }

    uint32 Mesh::getIndexStartIndex() const {
        return mIndexStartIndex;
    }

    void Mesh::setIndexStartIndex(uint32 index) {
        mIndexStartIndex = index;
    }
        
    void Mesh::setIndexStream(const GraphicBufferPtr& index_stream) {
        if(mRenderBuffer)
            mRenderBuffer->bindIndexStream(index_stream);
        else {
            mRenderBuffer = ukn::Context::Instance().getGraphicFactory().createRenderBuffer();
        }
    }

    void Mesh::setVertexStream(const GraphicBufferPtr& vtx_stream, const vertex_elements_type& format) {
        if(mRenderBuffer) 
           mRenderBuffer->bindVertexStream(vtx_stream, format);
        else {
            mRenderBuffer = ukn::Context::Instance().getGraphicFactory().createRenderBuffer();
        }
    }

    void Mesh::setRenderBuffer(const RenderBufferPtr& buffer) {
        mRenderBuffer = buffer;
        this->setNumIndicies(mRenderBuffer->getIndexCount());
        this->setNumVertices(mRenderBuffer->getVertexCount());
    }

    void Mesh::buildInfo() {
        ModelPtr model = mModel.lock();

        /* retrieve textures from material */
        if(mMaterialId < model->getMaterialCount()) {
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

    void Model::addMesh(const MeshPtr& mesh) {
        mMeshes.push_back(mesh);
    }

    void Model::addMaterial(const MaterialPtr& mat) {
        mMaterials.push_back(mat);
    }

    void Model::setRenderBuffer(const RenderBufferPtr& buffer) {
        mRenderBuffer = buffer;
    }

}