#include "D3D11Helper.h"
#include "mist/Logger.h"

namespace ukn {

    ID3D11InputLayout* D3D11ShaderUtilities::CreateLayout(ID3D11Device* device, const void* signature, SIZE_T size, const ukn::VertexFormat& format) {
        D3D11_INPUT_ELEMENT_DESC layoutDesc[5];

        UINT numElements = 0;

        if(format.checkFormat(VF_XYZ)) {
            layoutDesc[numElements].SemanticName = "POSITION";
            layoutDesc[numElements].SemanticIndex = 0;
            layoutDesc[numElements].Format = DXGI_FORMAT_R32G32B32_FLOAT;
            layoutDesc[numElements].InputSlot = 0;
            layoutDesc[numElements].AlignedByteOffset = format.offsetXYZ();
            layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            layoutDesc[numElements].InstanceDataStepRate = 0;
            numElements += 1;
        }
        if(format.checkFormat(VF_Color0)) {
            layoutDesc[numElements].SemanticName = "COLOR";
            layoutDesc[numElements].SemanticIndex = 0;
            layoutDesc[numElements].Format = DXGI_FORMAT_R8G8B8A8_UINT;
            layoutDesc[numElements].InputSlot = 0;
            layoutDesc[numElements].AlignedByteOffset = format.offsetColor0();
            layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            layoutDesc[numElements].InstanceDataStepRate = 0;
            numElements += 1;
        }
        if(format.checkFormat(VF_UV)) {
            layoutDesc[numElements].SemanticName = "TEXCOORD";
            layoutDesc[numElements].SemanticIndex = 0;
            layoutDesc[numElements].Format = DXGI_FORMAT_R32G32_FLOAT;
            layoutDesc[numElements].InputSlot = 0;
            layoutDesc[numElements].AlignedByteOffset = format.offsetUV();
            layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            layoutDesc[numElements].InstanceDataStepRate = 0;
            numElements += 1;
        }
        if(format.checkFormat(VF_Color1)) {
            layoutDesc[numElements].SemanticName = "COLOR1";
            layoutDesc[numElements].SemanticIndex = 0;
            layoutDesc[numElements].Format = DXGI_FORMAT_R8G8B8A8_UINT;
            layoutDesc[numElements].InputSlot = 0;
            layoutDesc[numElements].AlignedByteOffset = format.offsetColor1();
            layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            layoutDesc[numElements].InstanceDataStepRate = 0;
            numElements += 1;
        }
        if(format.checkFormat(VF_Normal)) {
            layoutDesc[numElements].SemanticName = "NORMAL";
            layoutDesc[numElements].SemanticIndex = 0;
            layoutDesc[numElements].Format = DXGI_FORMAT_R32G32B32_FLOAT;
            layoutDesc[numElements].InputSlot = 0;
            layoutDesc[numElements].AlignedByteOffset = format.offsetNormal();
            layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            layoutDesc[numElements].InstanceDataStepRate = 0;
            numElements += 1;
        }

        ID3D11InputLayout* layout = 0;

        HRESULT result = device->CreateInputLayout(layoutDesc,
            numElements,
            signature,
            size,
            &layout);
        if(!D3D11Debug::CHECK_RESULT(result)) {
            return 0;
        }
        return layout;
    }
    void D3D11ShaderUtilities::LogError(ID3D10Blob* error) {
        if(error) {
            std::string error_mssg((char*)error->GetBufferPointer(), (char*)error->GetBufferPointer() + error->GetBufferSize());
            log_error(error_mssg);
        }
    }
}