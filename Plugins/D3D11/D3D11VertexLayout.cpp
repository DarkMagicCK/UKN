#include "D3D11VertexLayout.h"
#include "D3D11Helper.h"
#include "D3D11GraphicDevice.h"

namespace ukn {
    
    D3D11VertexLayout::D3D11VertexLayout(const vertex_elements_type& format, 
                                        uint8* code, 
                                        uint32 signature, 
                                        D3D11GraphicDevice* device,
                                        uint32 instance_start,
                                        uint32 instance_end):
    mSignature(0) {
        ID3D11InputLayout* layout = D3D11ShaderUtilities::CreateLayout(
                                        device->getD3DDevice(),
                                        code,
                                        signature,
                                        format,
                                        instance_start,
                                        instance_end);
        if(layout) {
            mLayout = MakeCOMPtr(layout);
            mSignature = signature;
        }
    }

    D3D11VertexLayout::~D3D11VertexLayout() {

    }

    ID3D11InputLayout* D3D11VertexLayout::getD3DLayout() const {
        return mLayout.get();
    }

    bool D3D11VertexLayout::operator==(uint32 sig) const {
        return this->mSignature == sig && this->mSignature != 0;
    }

}