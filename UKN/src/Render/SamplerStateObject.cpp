#include "UKN/SamplerStateObject.h"

#include "UKN/GraphicFactory.h"
#include "UKN/Context.h"

namespace ukn {

    SamplerStateObject::SamplerStateObject(const SamplerStateDesc& desc):
        mDesc(desc) {

    }

    SamplerStateObject::~SamplerStateObject() {

    }

    const SamplerStateDesc& SamplerStateObject::getDesc() const {
        return mDesc;
    }

    namespace {

        SamplerStatePtr _g_linearclamp;
        SamplerStatePtr _g_linearwrap;
        SamplerStatePtr _g_pointclamp;
        SamplerStatePtr _g_pointwrap;

    }

    void SamplerStateObject::InitializeBuildInStates() {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        
        SamplerStateDesc desc;
        _g_linearclamp = gf.createSamplerStateObject(desc);

        desc.filter = RSP_FilterMinMagMipPoint;
        _g_pointclamp = gf.createSamplerStateObject(desc);

        desc.filter = RSP_FilterMinMagMipLinear;
        desc.address_u = desc.address_v = desc.address_w = RSP_TextureWrapWrap;
        _g_linearwrap = gf.createSamplerStateObject(desc);

        desc.filter = RSP_FilterMinMagMipPoint;
        _g_pointwrap = gf.createSamplerStateObject(desc);

    }

    SamplerStatePtr SamplerStateObject::LinearClamp() {
        return _g_linearclamp;
    }

    SamplerStatePtr SamplerStateObject::LinearWrap() {
        return _g_linearwrap;
    }

    SamplerStatePtr SamplerStateObject::PointClamp() {
        return _g_pointclamp;
    }

    SamplerStatePtr SamplerStateObject::PointWrap() {
        return _g_pointwrap;
    }

}