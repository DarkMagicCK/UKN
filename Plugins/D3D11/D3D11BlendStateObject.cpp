#include "D3D11BlendStateObject.h"

#include "D3D11GraphicDevice.h"
#include "D3D11Debug.h"

namespace ukn {

    D3D11_BLEND_OP _rs_to_blendop(RenderStateParam param) {
        switch(param) {
        case RSP_BlendOpAdd:            return D3D11_BLEND_OP_ADD;
        case RSP_BlendOpSubtract:       return D3D11_BLEND_OP_SUBTRACT;
        case RSP_BlendOpMax:            return D3D11_BLEND_OP_MAX;
        case RSP_BlendOpMin:            return D3D11_BLEND_OP_MIN;
        case RSP_BlendOpRevSubtract:    return D3D11_BLEND_OP_REV_SUBTRACT;
        }
    }

    D3D11_BLEND _rs_to_blend(RenderStateParam param) {
        switch(param) {
        case RSP_BlendFuncDstAlpha:         return D3D11_BLEND_DEST_ALPHA;
        case RSP_BlendFuncDstColor:         return D3D11_BLEND_DEST_COLOR;
        case RSP_BlendFuncOne:              return D3D11_BLEND_ONE;
        case RSP_BlendFuncOneMinusDstAlpha: return D3D11_BLEND_INV_DEST_ALPHA;
        case RSP_BlendFuncOneMinusDstColor: return D3D11_BLEND_INV_DEST_COLOR;
        case RSP_BlendFuncOneMinusSrcAlpha: return D3D11_BLEND_INV_SRC_ALPHA;
        case RSP_BlendFuncOneMinusSrcColor: return D3D11_BLEND_INV_DEST_ALPHA;
        case RSP_BlendFuncSrcAlpha:         return D3D11_BLEND_SRC_ALPHA;
        case RSP_BlendFuncSrcColor:         return D3D11_BLEND_SRC_COLOR;
        case RSP_BlendFuncZero:             return D3D11_BLEND_ZERO;
           
        }
    }

    D3D11BlendStateObject::D3D11BlendStateObject(const BlendStateDesc& desc, D3D11GraphicDevice* device):
    BlendStateObject(desc) {
        D3D11_BLEND_DESC d3ddesc;
        d3ddesc.AlphaToCoverageEnable = desc.alpha_to_converage;
        d3ddesc.IndependentBlendEnable = FALSE;
        d3ddesc.RenderTarget[0].BlendEnable = desc.blend_state.enabled;
        d3ddesc.RenderTarget[0].BlendOp = _rs_to_blendop( desc.blend_state.op );
        d3ddesc.RenderTarget[0].BlendOpAlpha = _rs_to_blendop( desc.blend_state.op_alpha );
        d3ddesc.RenderTarget[0].DestBlend = _rs_to_blend( desc.blend_state.dst );
        d3ddesc.RenderTarget[0].SrcBlend = _rs_to_blend ( desc.blend_state.src );
        d3ddesc.RenderTarget[0].DestBlendAlpha = _rs_to_blend( desc.blend_state.dst_alpha );
        d3ddesc.RenderTarget[0].SrcBlendAlpha = _rs_to_blend( desc.blend_state.src_alpha );
        d3ddesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        ID3D11BlendState* blendState;
        if(D3D11Debug::CHECK_RESULT(device->getD3DDevice()->CreateBlendState(&d3ddesc, &blendState))) {
            mBlendState = MakeCOMPtr(blendState);
        } else {
            log_error(L"D3D11BlendState: error creating blend state");
        }

    }

    D3D11BlendStateObject::~D3D11BlendStateObject() {

    }

    ID3D11BlendState* D3D11BlendStateObject::getD3DBlendState() const {
        return mBlendState.get();
    }

}