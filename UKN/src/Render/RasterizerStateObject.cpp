#include "UKN/RasterizerStateObject.h"

namespace ukn {

    RasterizerStateObject::RasterizerStateObject(const RasterizerStateDesc& desc):
        mDesc(desc) {

    }

    RasterizerStateObject::~RasterizerStateObject() {

    }

    const RasterizerStateDesc&RasterizerStateObject::getDesc() const {
        return mDesc;
    }

}