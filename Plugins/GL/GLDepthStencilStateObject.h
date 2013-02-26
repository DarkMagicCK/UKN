#ifndef UKN_GL_DEPTH_STENCIL_STATE_OBJECT_H_
#define UKN_GL_DEPTH_STENCIL_STATE_OBJECT_H_

#include "UKN/DepthStencilStateObject.h"

namespace ukn {

    class GLDepthStencilStateObject: public DepthStencilStateObject {
    public:
        GLDepthStencilStateObject(const DepthStencilStateDesc& desc):
            DepthStencilStateObject(desc) {
        };

        virtual ~GLDepthStencilStateObject() {

        }

    };

}

#endif