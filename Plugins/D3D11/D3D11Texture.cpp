#include "D3D11Texture.h"

#include "D3D11GraphicDevice.h"
#include "D3D11Convert.h"

#include "mist/Logger.h"

namespace ukn {

    D3D11Texture2D::D3D11Texture2D(D3D11GraphicDevice* device):
        Texture(TT_Texture2D),
        mTextureResourceView(0),
        mTexture(0),
        mDevice(device) {

    }

    D3D11Texture2D::~D3D11Texture2D() {
        if(mTexture) {
            mTexture->Release();
        }
        if(mTextureResourceView) {
            mTextureResourceView->Release();
        }
    }

    bool D3D11Texture2D::load(const ResourcePtr& resource, bool createMipmap) {
        if(!resource->getResourceStream())
            return false;

        StreamPtr memStream = resource->getResourceStream()->readIntoMemory();

        if(memStream) {
            MemoryStream* memData = static_cast<MemoryStream*>(memStream.get());

            if(mTexture) {
                mTexture->Release();
                mTexture = 0;
            }
            if(mTextureResourceView) {
                mTextureResourceView->Release();
            }

            if(D3D11Debug::CHECK_RESULT(D3DX11CreateTextureFromMemory(mDevice->getD3DDevice(),
                memData->data(),
                memData->size(),
                0,
                0,
                (ID3D11Resource**)&mTexture,
                0))) {
                    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

                    D3D11_TEXTURE2D_DESC desc;
                    mTexture->GetDesc(&desc);

                    ZeroMemory(&srvDesc, sizeof(srvDesc));
                    srvDesc.Format = desc.Format;
                    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                    srvDesc.Texture2D.MipLevels = desc.MipLevels;
                    srvDesc.Texture1D.MostDetailedMip = 0;

                    if(!D3D11Debug::CHECK_RESULT(mDevice->getD3DDevice()->CreateShaderResourceView(mTexture, &srvDesc, &mTextureResourceView))) {
                        mTexture->Release();
                        mTexture = 0;

                        log_error("error creating shader source view for texture");

                        return false;
                    }
                    return true;
            }
        }
        
        return false;
    }

    bool D3D11Texture2D::create(uint32 w, uint32 h, uint32 mipmaps, ElementFormat format, const uint8* initialData) {
        D3D11_TEXTURE2D_DESC desc;

        ZeroMemory(&desc, sizeof(desc));
        desc.Width = w;
        desc.Height = h;
        desc.MipLevels = mipmaps == 0 ? 1: mipmaps;
        desc.ArraySize = 1;
        desc.Format = ElementFormatToDxGIFormat(format);
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

        if(mTexture) {
            mTexture->Release();
            mTexture = 0;
        }
        if(mTextureResourceView) {
            mTextureResourceView->Release();
            mTextureResourceView = 0;
        }

        uint8* texData;
        if(initialData == 0) {
            texData = mist_malloc_t(uint8, w*h*GetElementSize(format));
            memset(texData, 0xFF, w*h*GetElementSize(format));
        } else {
            texData = const_cast<uint8*>(initialData);
        }

        D3D11_SUBRESOURCE_DATA textureData;
        textureData.pSysMem = texData;
        textureData.SysMemPitch = w * GetElementSize(format);

        if(!D3D11Debug::CHECK_RESULT(mDevice->getD3DDevice()->CreateTexture2D(&desc, &textureData, &mTexture))) {
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture1D.MostDetailedMip = desc.MipLevels - 1;

        if(!D3D11Debug::CHECK_RESULT(mDevice->getD3DDevice()->CreateShaderResourceView(mTexture, &srvDesc, &mTextureResourceView))) {
            mTexture->Release();
            mTexture = 0;

            log_error("error creating shader source view for texture");

            return false;
        }
        return true;
    }

    void* D3D11Texture2D::map(uint32 level) {
#if defined(UKN_D3D10)
        D3D11_MAPPED_TEXTURE2D mappedTex;
        if(mTexture) {
            if(FAILED(mTexture->Map(D3D11CalcSubresource(0, 0, 1),
                D3D11_MAP_WRITE_DISCARD,
                0,
                &mappedTex))) {
                    log_error("error mapping texture2d data");
                    return SharedPtr<uint8>();
            }
            uint32 size = this->getWidth() * this->getHeight() * GetElementSize(this->getFormat());
            uint8* texData = new uint8[size];
            memcpy(texData, mappedTex.pData, size);

            mTexture->Unmap(D3D11CalcSubresource(0, 0, 1));
            return SharedPtr<uint8>(texData);
        }
        return SharedPtr<uint8>();
#endif
        D3D11_MAPPED_SUBRESOURCE mappedTex;
        if(FAILED(mDevice->getD3DDeviceContext()->Map(mTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedTex))) {
            log_error("error mapping texture2d data");
            return SharedPtr<uint8>();
        }
        return mappedTex.pData;
    }

    void D3D11Texture2D::unmap() {
        if(mTexture) {
            /*uint8* inPixel = (uint8*)data;
            uint8* pixels = (uint8*)mappedTex.pData;
            for(uint32 row = y; row < y + height; ++row) {
            uint32 inStart = (row - y) * width;
            uint32 rowStart = row * mappedTex.RowPitch;
            for(uint32 col = x; col < x + width; ++col) {
            uint32 colStart = col * 4;
            pixels[rowStart + colStart + 0] = inPixel[inStart + colStart + 0];
            pixels[rowStart + colStart + 1] = inPixel[inStart + colStart + 1];
            pixels[rowStart + colStart + 2] = inPixel[inStart + colStart + 2];
            pixels[rowStart + colStart + 3] = inPixel[inStart + colStart + 3];
            }
            }*/
#if defined(UKN_D3D10)	
            mTexture->Unmap(D3D11CalcSubresource(0, 0, level));
#else
            mDevice->getD3DDeviceContext()->Unmap(mTexture, 0);
#endif
        }
    }

    uint32 D3D11Texture2D::getWidth(uint32 level) const {
        D3D11_TEXTURE2D_DESC desc;
        if(mTexture) {
            mTexture->GetDesc(&desc);
            return desc.Width / (1U << (level));
        }
    }

    uint32 D3D11Texture2D::getHeight(uint32 level) const {
        D3D11_TEXTURE2D_DESC desc;
        if(mTexture) {
            mTexture->GetDesc(&desc);
            return desc.Height / (1U << (level));
        }
    }

    uintPtr D3D11Texture2D::getTextureId() const {
        return (uintPtr)mTexture;
    }

    ID3D11Texture2D* D3D11Texture2D::getTexture() const {
        return mTexture;
    }

    ID3D11ShaderResourceView* D3D11Texture2D::getTextureResourceView() const {
        return mTextureResourceView;
    }

} // namespace ukn