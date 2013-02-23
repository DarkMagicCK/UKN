#ifndef PROJECT_UNKNOWN_MODEL_H_
#define PROJECT_UNKNOWN_MODEL_H_

#include "mist/Platform.h"
#include "mist/MathUtil.h"
#include "mist/Util.h"
#include "mist/Ptr.h"

#include "UKN/Vertex.h"
#include "UKN/PreDeclare.h"

namespace ukn {

    class UKN_API ModelLoader {
    public:
        static ukn::RenderBufferPtr BuildFromSphere(const mist::Sphere& sphere, uint32 slices);
        static ukn::RenderBufferPtr BuildFromBox(const mist::Box& box);
        
        /* simple format loaders
            to do with more complicated loaders, such as Scene, Lights & Animations
         */
        
        struct ModelData {
            Array<float3> position;
            Array<float3> normal;
            Array<float2> uv;
            Array<float3> color;
            Array<uint32> indices;
        };
        
        typedef SharedPtr<ModelData> ModelDataPtr;
        
        /* stanford polygon format, will only read components specified in elements */
        static ModelDataPtr LoadFromPly(const mist::ResourcePtr& file);
        /* wavefront obj format */
        static ModelDataPtr LoadFromObj(const mist::ResourcePtr& file);
        /* plane space separated file, components specified by elements */
        static ModelDataPtr LoadFromPlaneFile(const mist::ResourcePtr& file);
        
    };

}

#endif