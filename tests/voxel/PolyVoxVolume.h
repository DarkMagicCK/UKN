#ifndef UKN_TEST_POLYVOX_VOLUMN_H_
#define UKN_TEST_POLYVOX_VOLUMN_H_

#include "UKN/UKN.h"

#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/SimpleVolume.h"

#ifdef MIST_DEBUG
#pragma comment(lib, "PolyVoxCored.lib")
#pragma comment(lib, "PolyVoxUtild.lib")
#else
#pragma comment(lib, "PolyVoxCore.lib")
#pragma comment(lib, "PolyVoxUtil.lib")
#endif

namespace ukn {

    namespace voxel {

        
        using namespace PolyVox;

        class SimplePolyvoxVolume: public Renderable {
        public:
            SimplePolyvoxVolume();
            ~SimplePolyvoxVolume();

        public:
            /* Renderable */
            const UknString& getName() const override;
        
            Box getBound() const override;
            RenderBufferPtr getRenderBuffer() const override;
        
            void initSphere();
            void initPlane(int thickness);

        private:
            void createBuffers(const SurfaceMesh<PositionMaterialNormal>& mesh);

            ukn::RenderBufferPtr mRenderBuffer;
        };

    }

}

#endif