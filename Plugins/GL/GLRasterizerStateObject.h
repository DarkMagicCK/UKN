#ifndef __Project_Unknown__GLRasterizerStateObject__
#define __Project_Unknown__GLRasterizerStateObject__

#include "UKN/RasterizerStateObject.h"

namespace ukn {

    class GLRasterizerStateObject: public RasterizerStateObject {
    public:
        GLRasterizerStateObject(const RasterizerStateDesc& desc):
            RasterizerStateObject(desc) {
        }

        virtual ~GLRasterizerStateObject() { }
    };

}

#endif //__Project_Unknown__GLRasterizerStateObject__