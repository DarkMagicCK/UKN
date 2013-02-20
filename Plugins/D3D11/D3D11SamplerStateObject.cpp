#include "D3D11SamplerStateObject.h"

#include "D3D11GraphicDevice.h"
#include "D3D11Debug.h"

namespace ukn {

    D3D11_TEXTURE_ADDRESS_MODE _rs_to_tex_address(RenderStateParam param) {
        switch(param) {
        case RSP_TextureWrapClamp: return D3D11_TEXTURE_ADDRESS_CLAMP;
        case RSP_TextureWrapMirror: return D3D11_TEXTURE_ADDRESS_MIRROR;
        case RSP_TextureWrapClampToBorder: return D3D11_TEXTURE_ADDRESS_BORDER;
        case RSP_TextureWrapRepeat: return D3D11_TEXTURE_ADDRESS_WRAP;
        }
    }

    D3D11_FILTER _rs_to_filter(RenderStateParam param) {
        switch(param) {
        case RSP_FilterAnisotropic:             return D3D11_FILTER_ANISOTROPIC;
        case RSP_FilterMinLinearMagMipPoint:    return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        case RSP_FilterMinLinearMagPointMipLinear: return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        case RSP_FilterMinMagLinearMipPoint:    return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        case RSP_FilterMinMagMipLinear:         return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        case RSP_FilterMinMagPointMipLinear:    return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        case RSP_FilterMinMapMipPoint:          return D3D11_FILTER_MIN_MAG_MIP_POINT;
        case RSP_FilterMinPointMagLinearMipPoint: return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        case RSP_FilterMinPointMagMipLinear:    return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
        }
    }

    D3D11SamplerStateObject::D3D11SamplerStateObject(const SamplerStateDesc& desc, ID3D11SamplerState* state):
    SamplerStateObject(desc),
    mSamplerState(MakeCOMPtr(state)) {


    }

    D3D11SamplerStateObject::D3D11SamplerStateObject(const SamplerStateDesc& desc, D3D11GraphicDevice* device):
    SamplerStateObject(desc) {
        D3D11_SAMPLER_DESC d3ddesc;
        ZeroMemory(&d3ddesc, 0);

        d3ddesc.AddressU = _rs_to_tex_address( desc.address_u );
        d3ddesc.AddressV = _rs_to_tex_address( desc.address_v );
        d3ddesc.AddressW = _rs_to_tex_address( desc.address_w );

        d3ddesc.MaxAnisotropy = desc.max_anisotropy;
        d3ddesc.Filter = _rs_to_filter( desc.filter );

        // default values, currently not mapped in ukn
        d3ddesc.MinLOD = -FLT_MAX;
        d3ddesc.MaxLOD = FLT_MAX;
        d3ddesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        
        ID3D11SamplerState* samplerState;
        if(D3D11Debug::CHECK_RESULT(device->getD3DDevice()->CreateSamplerState(&d3ddesc, &samplerState))) {
            mSamplerState = MakeCOMPtr(samplerState);
        } else {
            log_error(L"D3D11SamplerStateObject: error creating sampler state");
        }
    }

    D3D11SamplerStateObject::~D3D11SamplerStateObject() {
    
    }

    ID3D11SamplerState* D3D11SamplerStateObject::getD3DSamplerState() const {
        return mSamplerState.get();
    }

}