#include "UKN/SamplerStateObject.h"

namespace ukn {

    SamplerStateObject::SamplerStateObject(const SamplerStateDesc& desc):
        mDesc(desc) {

    }

    SamplerStateObject::~SamplerStateObject() {

    }

    const SamplerStateDesc& SamplerStateObject::getDesc() const {
        return mDesc;
    }

}