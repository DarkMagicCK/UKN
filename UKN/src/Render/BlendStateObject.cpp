#include "UKN/BlendStateObject.h"

namespace ukn {

    BlendStateObject::BlendStateObject(const BlendStateDesc& desc):
        mDesc(desc) {
    }

    BlendStateObject::~BlendStateObject() {
    
    }

    const BlendStateDesc& BlendStateObject::getDesc() const { 
        return mDesc;
    }

}