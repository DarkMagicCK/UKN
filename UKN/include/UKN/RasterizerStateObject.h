#ifndef RASTERIZER_STATE_OBJECT_H_
#define RASTERIZER_STATE_OBJECT_H_

#include "UKN/PreDeclare.h"
#include "UKN/GraphicSettings.h"

namespace ukn {
 
    // opengl introduced sampler objects in 3.3
    // so the behavior of sampler objects is simulated in OGL Plugin
    //  for each FrameBuffer
    struct UKN_API RasterizerStateDesc {
        RenderStateParam cullface;  /* front / back / none */
        RenderStateParam frontface; /* CW / CCW */
        RenderStateParam fillmode;  /* solid, wireframe */
        
        // line smooth / multisample / depth bias ?
        RasterizerStateDesc():
        cullface(RSP_CullBack),
        frontface(RSP_FrontFaceClockwise),
        fillmode(RSP_FillSolid) { }
    };
    
    class UKN_API RasterizerStateObject {
    public:
        RasterizerStateObject(const RasterizerStateDesc& desc);
      
        // force base
        virtual ~RasterizerStateObject() = 0;
        
        const RasterizerStateDesc& getDesc() const;
        
    private:
        RasterizerStateDesc mDesc;
    };
    
}

#endif // RASTERIZER_STATE_OBJECT_H_