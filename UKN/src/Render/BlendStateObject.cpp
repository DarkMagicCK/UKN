#include "UKN/BlendStateObject.h"

#include "UKN/GraphicFactory.h"
#include "UKN/Context.h"

namespace ukn {

    BlendStateObject::BlendStateObject(const BlendStateDesc& desc):
        mDesc(desc) {
    }

    BlendStateObject::~BlendStateObject() {
    
    }

    const BlendStateDesc& BlendStateObject::getDesc() const { 
        return mDesc;
    }

    namespace {

        BlendStatePtr _g_opaque;
        BlendStatePtr _g_addtive;
        BlendStatePtr _g_alphablend;
        BlendStatePtr _g_nonpremultiplied;
        BlendStatePtr _g_blendoff;
    }

    void BlendStateObject::InitializeBuildInStates() {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        
        BlendStateDesc desc;

        desc.blend_state.enabled = false;
        _g_blendoff = gf.createBlendStateObject(desc);

        desc.blend_state.enabled = true;
        desc.blend_state.src = RSP_BlendFuncOne;
        desc.blend_state.src_alpha = RSP_BlendFuncOne;
        desc.blend_state.dst = RSP_BlendFuncZero;
        desc.blend_state.dst_alpha = RSP_BlendFuncZero;
        _g_opaque = gf.createBlendStateObject(desc);

        desc.blend_state.src = RSP_BlendFuncSrcAlpha;
        desc.blend_state.src_alpha = RSP_BlendFuncSrcAlpha;
        desc.blend_state.dst = RSP_BlendFuncOne;
        desc.blend_state.dst_alpha = RSP_BlendFuncOne;
        _g_addtive = gf.createBlendStateObject(desc);

        desc.blend_state.src = RSP_BlendFuncOne;
        desc.blend_state.src_alpha = RSP_BlendFuncOne;
        desc.blend_state.dst = RSP_BlendFuncOneMinusSrcAlpha;
        desc.blend_state.dst_alpha = RSP_BlendFuncOneMinusSrcAlpha;
        _g_alphablend = gf.createBlendStateObject(desc);

        desc.blend_state.src = RSP_BlendFuncSrcAlpha;
        desc.blend_state.src_alpha = RSP_BlendFuncSrcAlpha;
        desc.blend_state.dst = RSP_BlendFuncOneMinusSrcAlpha;
        desc.blend_state.dst_alpha = RSP_BlendFuncOneMinusSrcAlpha;
        _g_nonpremultiplied = gf.createBlendStateObject(desc);
    }

    BlendStatePtr BlendStateObject::Opaque() {
        return _g_opaque;
    }

    BlendStatePtr BlendStateObject::Addtive() {
        return _g_addtive;
    }

    BlendStatePtr BlendStateObject::AlphaBlend() {
        return _g_alphablend;
    }

    BlendStatePtr BlendStateObject::NonPreMultiplied() {
        return _g_nonpremultiplied;
    }

    BlendStatePtr BlendStateObject::BlendOff() {
        return _g_blendoff;
    }

}