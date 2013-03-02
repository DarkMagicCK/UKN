#ifndef PROJECT_UNKNOWN_MODEL_H_
#define PROJECT_UNKNOWN_MODEL_H_

#include "mist/Platform.h"
#include "mist/MathUtil.h"
#include "mist/Util.h"
#include "mist/Ptr.h"

#include "UKN/Vertex.h"
#include "UKN/PreDeclare.h"
#include "UKN/Renderable.h"

namespace ukn {

    class UKN_API ModelLoader {
    public:
        static ukn::RenderBufferPtr BuildFromSphere(const mist::Sphere& sphere, uint32 slices);
        static ukn::RenderBufferPtr BuildFromBox(const mist::Box& box);
        
        /* simple format loaders
            to do with more complicated loaders, such as Scene, Lights & Animations
         */
        struct ModelData {
            std::vector<std::vector<uint8> > vertex_data;
            uint32 vertex_count;

            std::vector<uint32> index_data;
            vertex_format vertex_format;

            std::vector<MaterialPtr> materials;

            struct MeshData {
                uint32 num_vertices;
                uint32 start_vertex;
                uint32 num_indices;
                uint32 start_index;

                uint32 material_id;
                Box bounding_box;
                UknString name;
            };
            std::vector<MeshData> meshes;
        };
        
        typedef SharedPtr<ModelData> ModelDataPtr;
        
        /* stanford polygon format */
        static ModelDataPtr LoadFromPly(const mist::ResourcePtr& file, bool calculate_normal);
        /* wavefront obj format */
        static ModelDataPtr LoadFromObj(const mist::ResourcePtr& file);
        /* plane space separated file */
        static ModelDataPtr LoadFromPlaneFile(const mist::ResourcePtr& file);
        
        /* ukn model format, to do */
        static ModelDataPtr LoadFromUknModel(const mist::ResourcePtr& file, bool is_bin);

        static ModelPtr LoadModel(const UknString& name,
                                  uint32 access_hint);
        static ModelPtr CreateModel(const ModelDataPtr& data);
        static bool     SaveModel(const ModelPtr& model, 
                                  const UknString& save_to, 
                                  const std::string& mesh_name);
        static bool     SaveModel(const ModelDataPtr& data,
                                  const UknString& save_to,
                                  const std::string& mesh_name);

    };

    class Model;

    class UKN_API Mesh: public Renderable {
    public:
        Mesh(const ModelPtr& model, const UknString& name);
        virtual ~Mesh();
    
    public:
        /* IRenderable */
        virtual const UknString& getName() const;
        
        virtual Box getBound() const;
        virtual RenderBufferPtr getRenderBuffer() const;

    public:
        int32 getMaterialId() const;
        void setMaterailId(int32 mid);
        
        uint32 getVertexStartIndex() const;
        void setVertexStartIndex(uint32 index);

        uint32 getNumVertices() const;
        void setNumVertices(uint32 num);

        uint32 getNumTriangles() const;
        void setNumTriangles(uint32 num);

        uint32 getNumIndicies() const;
        void setNumIndicies(uint32 num);

        uint32 getIndexStartIndex() const;
        void setIndexStartIndex(uint32 index);

    public:
        void setIndexStream(const GraphicBufferPtr& index_steam);
        void setVertexStream(const GraphicBufferPtr& vtx_stream, const vertex_elements_type& format);

    protected:
        friend class ModelLoader;

        void buildInfo();
        void setRenderBuffer(const RenderBufferPtr& buffer);

        UknString mName;
        Box mBoundingBox;
        int32 mMaterialId;

        uint32 mVertexStartIndex;
        uint32 mIndexStartIndex;
        uint32 mNumVertices;
        uint32 mNumIndices;

        RenderBufferPtr mRenderBuffer;

        WeakPtr<Model> mModel; /* parent model */
    };

    class UKN_API Model: public Renderable {
    public:
        Model(const UknString& name);
        virtual ~Model();

    public:
       /* IRenderable */
        const UknString& getName() const override;
        
        Box getBound() const;
        RenderBufferPtr getRenderBuffer() const override;
        
        void onRenderBegin() override;
        void onRenderEnd() override;

    public:
        const MeshPtr& getMesh(size_t mid) const;
        size_t getMeshCount() const;
        
        const MaterialPtr& getMaterial(size_t mid) const;
        size_t getMaterialCount() const;

        const TexturePtr& getTexture(const std::string& name);

    protected:
        friend class ModelLoader;

        void updateBoundingBox();
        void addMesh(const MeshPtr& mesh);
        void addMaterial(const MaterialPtr& mat);
        void setRenderBuffer(const RenderBufferPtr& buffer);

        UknString mName;
        Box mBoundingBox;

        RenderBufferPtr mRenderBuffer;

        typedef std::vector<MeshPtr> MeshVec;
        MeshVec mMeshes;

        typedef std::vector<MaterialPtr> MaterialVec;
        MaterialVec mMaterials;

        std::unordered_map<std::string, TexturePtr> mTexturePool;
    };

}

#endif