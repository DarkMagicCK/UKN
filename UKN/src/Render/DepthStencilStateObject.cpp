#include "UKN/DepthStencilStateObject.h"

#include "UKN/GraphicFactory.h"
#include "UKN/Context.h"

namespace ukn {

    DepthStencilStateObject::DepthStencilStateObject(const DepthStencilStateDesc& desc):
    mDesc(desc) { 

    }

    DepthStencilStateObject::~DepthStencilStateObject() {

    }

    const DepthStencilStateDesc& DepthStencilStateObject::getDesc() const {
        return mDesc;
    }

    namespace {

        DepthStencilStatePtr _g_default;
        DepthStencilStatePtr _g_depthread;
        DepthStencilStatePtr _g_none;

    }

    void DepthStencilStateObject::InitializeBuildInStates() {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        
        DepthStencilStateDesc desc;

        _g_default = gf.createDepthStencilStateObject(desc);

        desc.depth_write_mask = false;
        _g_depthread = gf.createDepthStencilStateObject(desc);

        desc.depth_enable = false;
        _g_none = gf.createDepthStencilStateObject(desc);
    }

    DepthStencilStatePtr DepthStencilStateObject::Default() {
        return _g_default;
    }

    DepthStencilStatePtr DepthStencilStateObject::DepthRead() {
        return _g_depthread;
    }

    DepthStencilStatePtr DepthStencilStateObject::None() {
        return _g_none;
    }

}