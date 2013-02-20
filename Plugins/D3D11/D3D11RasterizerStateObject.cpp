#include "D3D11RasterizerStateObject.h"
#include "D3D11Debug.h"
#include "D3D11GraphicDevice.h"

namespace ukn {

    D3D11_CULL_MODE _rs_to_cullmode(RenderStateParam param) {
        switch(param) {
        case RSP_CullBack: return D3D11_CULL_BACK;
        case RSP_CullFront: return D3D11_CULL_FRONT;
        case RSP_CullNone: return D3D11_CULL_NONE;
        }
        return D3D11_CULL_BACK;
    }

    D3D11_FILL_MODE _rs_to_fillmode(RenderStateParam param) {
        switch(param) {
        case RSP_FillSolid: return D3D11_FILL_SOLID;
        case RSP_FillWireFrame: return D3D11_FILL_WIREFRAME;
        }
        return D3D11_FILL_SOLID;
    }

    bool _rs_to_cc(RenderStateParam param) {
        if(param == RSP_FrontFaceClockwise)
            return false;
        return true;
    }

    D3D11RasterizerStateObject::D3D11RasterizerStateObject(const RasterizerStateDesc& desc, ID3D11RasterizerState* state):
    RasterizerStateObject(desc),
    mRasterizerState(MakeCOMPtr(state)) {

    }

    D3D11RasterizerStateObject::D3D11RasterizerStateObject(const RasterizerStateDesc& desc, D3D11GraphicDevice* device):
        RasterizerStateObject(desc) {
        D3D11_RASTERIZER_DESC rasterizerDesc;

        rasterizerDesc.AntialiasedLineEnable = false;
        rasterizerDesc.CullMode = _rs_to_cullmode(desc.cullface);
        rasterizerDesc.DepthBias = 0;
        rasterizerDesc.DepthBiasClamp = 0.f;
        rasterizerDesc.DepthClipEnable = true;
        rasterizerDesc.FillMode = _rs_to_fillmode(desc.fillmode);
        rasterizerDesc.FrontCounterClockwise = _rs_to_cc(desc.frontface);
        rasterizerDesc.MultisampleEnable = false;
        rasterizerDesc.ScissorEnable = false;
        rasterizerDesc.SlopeScaledDepthBias = 0.f;

        ID3D11RasterizerState* rasterState;
        HRESULT result = device->getD3DDevice()->CreateRasterizerState(&rasterizerDesc, &rasterState);
        if(D3D11Debug::CHECK_RESULT(result, L"ID3D11Device->CreateRasterizerState")) {
            mRasterizerState = MakeCOMPtr(rasterState);
        }
    }

    D3D11RasterizerStateObject::~D3D11RasterizerStateObject() {
    
    }

    ID3D11RasterizerState* D3D11RasterizerStateObject::getD3D11RasterizerState() const {
        return mRasterizerState.get();
    }

}