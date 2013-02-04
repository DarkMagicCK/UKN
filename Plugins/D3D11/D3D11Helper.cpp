#include "D3D11Helper.h"
#include "mist/Logger.h"

namespace ukn {

    ID3D11InputLayout* D3D11ShaderUtilities::CreateLayout(ID3D11Device* device, const void* signature, SIZE_T size, const ukn::vertex_elements_type& format) {
        D3D11_INPUT_ELEMENT_DESC* layoutDesc = new D3D11_INPUT_ELEMENT_DESC[format.size()];

        uint32 offset = 0;
        uint32 index = 0;
        for(vertex_elements_type::const_iterator it = format.begin(),
            end = format.end();
            it != end;
            ++it) {
            layoutDesc[index].SemanticName = VertexUsageToSemanticName(it->usage);
            layoutDesc[index].SemanticIndex = 0;
            layoutDesc[index].Format = ElementFormatToDxGIFormat(it->format);
            layoutDesc[index].InputSlot = 0;
            layoutDesc[index].AlignedByteOffset = offset;
            layoutDesc[index].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            layoutDesc[index].InstanceDataStepRate = 0;
            index += 1;
            offset += it->size();
        }
       
        ID3D11InputLayout* layout = 0;

        HRESULT result = device->CreateInputLayout(
            layoutDesc,
            format.size(),
            signature,
            size,
            &layout);
        delete []layoutDesc;
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