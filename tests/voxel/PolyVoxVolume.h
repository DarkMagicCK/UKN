#ifndef UKN_TEST_POLYVOX_VOLUMN_H_
#define UKN_TEST_POLYVOX_VOLUMN_H_

#include "UKN/UKN.h"


#ifdef MIST_DEBUG
#pragma comment(lib, "PolyVoxCored.lib")
#pragma comment(lib, "PolyVoxUtild.lib")
#else
#pragma comment(lib, "PolyVoxCore.lib")
#pragma comment(lib, "PolyVoxUtil.lib")
#endif

namespace ukn {

    namespace voxel {


        class SimplePolyvoxVolume: public Renderable {
        public:
            SimplePolyvoxVolume();
            ~SimplePolyvoxVolume();

        public:
            /* Renderable */
            const UknString& getName() const override;
        
            Box getBound() const override;
            RenderBufferPtr getRenderBuffer() const override;
        
        private:
            void init();

            ukn::RenderBufferPtr mRenderBuffer;
        };

    }

}

#endif