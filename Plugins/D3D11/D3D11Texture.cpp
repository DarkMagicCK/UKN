#include "D3D11Texture.h"

#include "D3D11GraphicDevice.h"
#include "D3D11GraphicFactory.h"

namespace ukn {

    D3D11Texture::D3D11Texture(TextureType type, 
                               uint32 sample_count, 
                               uint32 sample_quality, 
                               uint32 access_hint, 
                               D3D11GraphicDevice* device):
    Texture(type, sample_count, sample_quality, access_hint),
    mDevice(device) {

    }

    D3D11Texture::~D3D11Texture() {

    }

    uint32 D3D11Texture::width(uint32 level) const {
        return 0;
    }

    uint32 D3D11Texture::height(uint32 level) const {
        return 0;
    }

    uint32 D3D11Texture::depth(uint32 level) const {
        return 0;
    }

    void D3D11Texture::generateMipmaps() {
        if(mShaderResourceView && mDevice) 
            mDevice->getD3DDeviceContext()->GenerateMips(mShaderResourceView.get());
    }

    uintPtr D3D11Texture::getTextureId() const {
        return 0;
    }

    ID3D11ShaderResourceView* D3D11Texture::getShaderResourceView() const {
        return mShaderResourceView.get();
    }
}