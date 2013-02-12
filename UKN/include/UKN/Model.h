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
        static ukn::RenderBufferPtr LoadFromCSV(const mist::ResourcePtr& file, const vertex_elements_type& elements);
    };

}

#endif