#include "PolyVoxVolume.h"

#undef MessageBox
#include "mist/SysUtil.h"

namespace ukn {

    namespace voxel {
        
        void createSphereInVolume(SimpleVolume<uint8_t>& volData, float fRadius)
        {
	        //This vector hold the position of the center of the volume
	        Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

	        //This three-level for loop iterates over every voxel in the volume
	        for (int z = 0; z < volData.getDepth(); z++)
	        {
		        for (int y = 0; y < volData.getHeight(); y++)
		        {
			        for (int x = 0; x < volData.getWidth(); x++)
			        {
				        //Store our current position as a vector...
				        Vector3DFloat v3dCurrentPos(x,y,z);	
				        //And compute how far the current position is from the center of the volume
				        float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

				        uint8_t uVoxelValue = 0;

				        //If the current voxel is less than 'radius' units from the center then we make it solid.
				        if(fDistToCenter <= fRadius)
				        {
					        //Our new voxel value
					        uVoxelValue = 255;
				        }

				        //Wrte the voxel value into the volume	
				        volData.setVoxelAt(x, y, z, uVoxelValue);
			        }
		        }
	        }
        }

        std::vector<VertexUVNormal> tranform_pv_vertex_to_vertex_uv_normal(const std::vector<PositionMaterialNormal>& pv) {
            std::vector<VertexUVNormal> n;
            std::for_each(pv.begin(), pv.end(), [&](const PositionMaterialNormal& pvv) {
                VertexUVNormal vtx;
                vtx.normal = Vector3(pvv.getNormal().getX(),
                                     pvv.getNormal().getY(),
                                     pvv.getNormal().getZ());
                vtx.position = Vector3(pvv.getPosition().getX(),
                                       pvv.getPosition().getY(),
                                       pvv.getPosition().getZ());
                vtx.uv = Vector2(0, 0);
                n.push_back(vtx);
            });
            return n;
        }

        struct PVVertexMeshNormalVertex {
            static ukn::vertex_elements_type Format() {
                vertex_elements_type static_format = VertexElementsBuilder()
                                                        .add(VertexElement(VU_Position, EF_Float3, 0))
                                                        .add(VertexElement(VU_Normal, EF_Float3, 0))
                                                        .add(VertexElement(VU_UV/* for material? */, EF_Float, 0))
                                                        .to_vector();
                return static_format;
            }
        };

        SimplePolyvoxVolume::SimplePolyvoxVolume() {
            this->init();
        }

        SimplePolyvoxVolume::~SimplePolyvoxVolume() {

        }

        Box SimplePolyvoxVolume::getBound() const {
            return Box();
        }

        const UknString& SimplePolyvoxVolume::getName() const {
            static UknString name(L"SimplePolyvoxVolume");
            return name;
        }

        RenderBufferPtr SimplePolyvoxVolume::getRenderBuffer() const {
            return mRenderBuffer;
        }

        void SimplePolyvoxVolume::init() {
              //Create an empty volume and then place a sphere in it
	        SimpleVolume<uint8_t> volData(PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(63, 63, 63)));
	        createSphereInVolume(volData, 30);

	        //A mesh object to hold the result of surface extraction
	        SurfaceMesh<PositionMaterialNormal> mesh;

	        //Create a surface extractor. Comment out one of the following two lines to decide which type gets created.
	        CubicSurfaceExtractorWithNormals< SimpleVolume<uint8_t> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh);
	        //MarchingCubesSurfaceExtractor< SimpleVolume<uint8_t> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh);

	        //Execute the surface extractor.
	        surfaceExtractor.execute();

           std::vector<VertexUVNormal> vertices(tranform_pv_vertex_to_vertex_uv_normal(mesh.getVertices()));

            ukn::GraphicFactory& gf = Context::Instance().getGraphicFactory();
            ukn::GraphicBufferPtr vtxBuffer = gf.createVertexBuffer(GraphicBuffer::None,
                                                                    GraphicBuffer::Static,
                                                                    vertices.size(),
                                                                    &vertices[0],
                                                                    VertexUVNormal::Format());
            ukn::GraphicBufferPtr idxBuffer = gf.createIndexBuffer(GraphicBuffer::None,
                                                                   GraphicBuffer::Static,
                                                                   mesh.getIndices().size(),
                                                                   &mesh.getIndices()[0]);
            if(vtxBuffer && idxBuffer) {
                mRenderBuffer = gf.createRenderBuffer();
                mRenderBuffer->bindVertexStream(vtxBuffer, VertexUVNormal::Format());
                mRenderBuffer->bindIndexStream(idxBuffer);
                mRenderBuffer->useIndexStream(true);

            } else {
                log_error(L"Error building volume data");
            }
        }

    }

}