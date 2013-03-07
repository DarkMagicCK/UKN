#include "D3D11RenderBuffer.h"
#include "D3D11VertexLayout.h"
#include "D3D11GraphicDevice.h"
#include "D3D11Shader.h"
#include "CgShader.h"

#include "ukn/RenderBuffer.h"

namespace ukn {

	D3D11RenderBuffer::D3D11RenderBuffer(D3D11GraphicDevice* device):
	mDevice(device) {
	}

	D3D11RenderBuffer::~D3D11RenderBuffer() {

	}

    
    ID3D11InputLayout* D3D11RenderBuffer::inputLayout(uint8* code, uint32 signature) {
        for(D3D11VertexLayoutPtr& layout: mLayouts) {
            if(*layout == signature) {
                return layout->getD3DLayout();
            }
        }
        vertex_elements_type format;
        for(StreamObject& vo: mVertexStreams) {
            format.insert(format.end(), vo.format.begin(), vo.format.end());
        }
        uint32 instance_start = 0, instance_end = 0;
        if(mInstanceStream.stream)  {
            instance_start = format.size();
            instance_end = instance_start + mInstanceStream.format.size();
            format.insert(format.end(), mInstanceStream.format.begin(), mInstanceStream.format.end());
        }

        D3D11VertexLayoutPtr layout = MakeSharedPtr<D3D11VertexLayout>(format, 
                                                                        code, 
                                                                        signature, 
                                                                        mDevice,
                                                                        instance_start,
                                                                        instance_end);
        mLayouts.push_back(layout);
        return layout->getD3DLayout();
    }


}