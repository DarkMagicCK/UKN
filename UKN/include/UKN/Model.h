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
    };

}