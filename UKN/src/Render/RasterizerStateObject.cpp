#include "UKN/RasterizerStateObject.h"

#include "UKN/GraphicFactory.h"
#include "UKN/Context.h"

namespace ukn {

    RasterizerStateObject::RasterizerStateObject(const RasterizerStateDesc& desc):
        mDesc(desc) {

    }

    RasterizerStateObject::~RasterizerStateObject() {

    }

    const RasterizerStateDesc&RasterizerStateObject::getDesc() const {
        return mDesc;
    }

    namespace {

        RasterizerStatePtr _g_clockwise;
        RasterizerStatePtr _g_counterclockwise;
        RasterizerStatePtr _g_none;

    }

    void RasterizerStateObject::InitializeBuildInStates() {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        
        RasterizerStateDesc desc;
        /* front = cw, cull = back */
        _g_counterclockwise = gf.createRasterizerStateObject(desc);

        /* front = ccw, cull = front */
        desc.frontface = RSP_FrontFaceCounterClockwise;
        _g_clockwise = gf.createRasterizerStateObject(desc);

        desc.cullface = RSP_CullNone;
        _g_none = gf.createRasterizerStateObject(desc);

    }

    RasterizerStatePtr RasterizerStateObject::CullClockwise() {
        return _g_clockwise;
    }

    RasterizerStatePtr RasterizerStateObject::CullCounterClockwise() {
        return _g_counterclockwise;
    }

    RasterizerStatePtr RasterizerStateObject::CullNone() {
        return _g_none;
    }

}