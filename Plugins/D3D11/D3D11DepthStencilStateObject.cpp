#include "D3D11DepthStencilStateObject.h"
#include "D3D11GraphicDevice.h"

namespace ukn {

    D3D11_COMPARISON_FUNC _rs_to_comp_func(RenderStateParam param) {
        switch(param) {
        case RSP_CompAlways:    return D3D11_COMPARISON_ALWAYS;
        case RSP_CompEqual:     return D3D11_COMPARISON_EQUAL;
        case RSP_CompGEqual:    return D3D11_COMPARISON_GREATER_EQUAL;
        case RSP_CompGreater:   return D3D11_COMPARISON_GREATER;
        case RSP_CompLEqual:    return D3D11_COMPARISON_LESS_EQUAL;
        case RSP_CompLess:      return D3D11_COMPARISON_LESS;
        case RSP_CompNever:     return D3D11_COMPARISON_NEVER;
        case RSP_CompNotEqual:  return D3D11_COMPARISON_NOT_EQUAL;
        }
        return D3D11_COMPARISON_LESS;
    }

    D3D11_STENCIL_OP _rs_to_stencil_op(RenderStateParam param) {
        switch(param) {
        case RSP_StencilOpZero:     return D3D11_STENCIL_OP_ZERO;
        case RSP_StencilOpKeep:     return D3D11_STENCIL_OP_KEEP;
        case RSP_StencilOpReplace:  return D3D11_STENCIL_OP_REPLACE;
        case RSP_StencilOpIncr:     return D3D11_STENCIL_OP_INCR_SAT;
        case RSP_StencilOpDecr:     return D3D11_STENCIL_OP_INCR_SAT;
        case RSP_StencilOpInvert:   return D3D11_STENCIL_OP_INVERT;
        case RSP_StencilOpIncrWrap: return D3D11_STENCIL_OP_INCR;
        case RSP_StencilOpDecrWrap: return D3D11_STENCIL_OP_DECR;
        }
        return D3D11_STENCIL_OP_INCR;
    }

    D3D11DepthStencilStateObject::D3D11DepthStencilStateObject(const DepthStencilStateDesc& desc, D3D11GraphicDevice* device):
    DepthStencilStateObject(desc) {
        
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
        depthStencilDesc.DepthEnable = desc.depth_enable;
        depthStencilDesc.DepthWriteMask = desc.depth_write_mask ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilDesc.DepthFunc = _rs_to_comp_func(desc.depth_func);
        depthStencilDesc.StencilEnable = desc.stencil_enable;
        depthStencilDesc.StencilReadMask = 0xFF;
        depthStencilDesc.StencilWriteMask = desc.stencil_write_mask;

        depthStencilDesc.FrontFace.StencilFailOp =      _rs_to_stencil_op(desc.front.stencil_fail_op);
        depthStencilDesc.FrontFace.StencilDepthFailOp = _rs_to_stencil_op(desc.front.depth_fail_op);
        depthStencilDesc.FrontFace.StencilPassOp =      _rs_to_stencil_op(desc.front.pass_op);
        depthStencilDesc.FrontFace.StencilFunc =        _rs_to_comp_func(desc.front.func);

        depthStencilDesc.BackFace.StencilFailOp =       _rs_to_stencil_op(desc.front.stencil_fail_op);
        depthStencilDesc.BackFace.StencilDepthFailOp =  _rs_to_stencil_op(desc.back.depth_fail_op);
        depthStencilDesc.BackFace.StencilPassOp =       _rs_to_stencil_op(desc.back.pass_op);
        depthStencilDesc.BackFace.StencilFunc =         _rs_to_comp_func(desc.back.func);

        ID3D11DepthStencilState* state;
        HRESULT result = device->getD3DDevice()->CreateDepthStencilState(&depthStencilDesc, &state);
        if(FAILED(result))
            log_error(L"D3D11DepthStencilState: Error create depth stencil state");
        else
            mState = MakeCOMPtr(state);
    }

    D3D11DepthStencilStateObject::~D3D11DepthStencilStateObject() {

    }

    ID3D11DepthStencilState* D3D11DepthStencilStateObject::getD3DDepthStencilState() const {
        return mState.get();
    }

}