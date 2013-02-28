#include "D3D11Texture.h"

#include "D3D11GraphicDevice.h"
#include "D3D11Convert.h"

#include "mist/Logger.h"
#include <D3DX11.h>

namespace ukn {

    D3D11Texture2D::D3D11Texture2D(D3D11GraphicDevice* device):
    D3D11Texture(TT_Texture2D, 1, 0, 0, device),
    mTexture(0) {

    }

    D3D11Texture2D::~D3D11Texture2D() {

    }

    bool D3D11Texture2D::load(const ResourcePtr& resource, bool createMipmap) {
        if(!resource->getResourceStream())
            return false;

        StreamPtr memStream = resource->getResourceStream()->readIntoMemory();

        if(memStream) {
            MemoryStream* memData = static_cast<MemoryStream*>(memStream.get());

            ID3D11Texture2D* texture;
            if(D3D11Debug::CHECK_RESULT(D3DX11CreateTextureFromMemory(mDevice->getD3DDevice(),
                memData->data(),
                memData->size(),
                0,
                0,
                (ID3D11Resource**)&texture,
                0))) {
                    mTexture = MakeCOMPtr(texture);

                    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

                    D3D11_TEXTURE2D_DESC desc;
                    mTexture->GetDesc(&desc);

                    ZeroMemory(&srvDesc, sizeof(srvDesc));
                    srvDesc.Format = desc.Format;
                    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                    srvDesc.Texture2D.MipLevels = desc.MipLevels;
                    srvDesc.Texture1D.MostDetailedMip = 0;

                    mFormat = DxGIFormatToElementFormat( desc.Format );
                    mNumMipmaps = 1;

                    ID3D11ShaderResourceView* shaderResourceView;
                    if(!D3D11Debug::CHECK_RESULT(mDevice->getD3DDevice()->CreateShaderResourceView(mTexture.get(), &srvDesc, &shaderResourceView))) {
                        mTexture.assign(0);

                        log_error(L"D3D11Texture2D::load: error creating shader source view for texture");

                        return false;
                    }
                    mShaderResourceView = MakeCOMPtr(shaderResourceView);
                    mTexture->GetDesc(&mDesc);
                    return true;
            }
        }

        return false;
    }

    bool D3D11Texture2D::create(uint32 w, uint32 h, uint32 mipmaps, ElementFormat format, const uint8* initialData, uint32 flag) {
        D3D11_TEXTURE2D_DESC desc;

        ZeroMemory(&desc, sizeof(desc));
        desc.Width = w;
        desc.Height = h;
        desc.MipLevels = mipmaps == 0 ? 1 : mipmaps;
        desc.ArraySize = 1;
        desc.Format = ElementFormatToDxGIFormat(format);
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.CPUAccessFlags = 0;
        
        DWORD bindFlags = 0;
        if(flag & TB_DepthStencil) {
            bindFlags |= D3D11_BIND_DEPTH_STENCIL;
        }
        if(flag & TB_RenderTarget) {
            bindFlags |= D3D11_BIND_RENDER_TARGET;
        }
        if(flag & TB_Texture) {
            bindFlags |= D3D11_BIND_SHADER_RESOURCE;
        }
        desc.BindFlags = bindFlags;

        ID3D11Texture2D* texture;
        if(initialData == 0) {
            if(!D3D11Debug::CHECK_RESULT(mDevice->getD3DDevice()->CreateTexture2D(&desc, 0, &texture))) {
                return false;
            }
        } else {
            D3D11_SUBRESOURCE_DATA textureData;

            textureData.pSysMem = initialData;
            textureData.SysMemPitch = w * GetElementSize(format);

            if(!D3D11Debug::CHECK_RESULT(mDevice->getD3DDevice()->CreateTexture2D(&desc, &textureData, &texture))) {
                return false;
            }
        }
        mTexture = MakeCOMPtr(texture);

        desc.Usage = D3D11_USAGE_STAGING;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

        ID3D11Texture2D* stagingTexture;
        mDevice->getD3DDevice()->CreateTexture2D(&desc, 0, &stagingTexture);
        if(stagingTexture)
            mStagingTexture = MakeCOMPtr(stagingTexture);

        if(bindFlags & D3D11_BIND_SHADER_RESOURCE) {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

            ZeroMemory(&srvDesc, sizeof(srvDesc));
            srvDesc.Format = desc.Format;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = desc.MipLevels;
            srvDesc.Texture1D.MostDetailedMip = desc.MipLevels - 1;

            ID3D11ShaderResourceView* shaderResourceView;

            if(!D3D11Debug::CHECK_RESULT(
                mDevice->getD3DDevice()->CreateShaderResourceView(mTexture.get(), 
                                                                  &srvDesc, 
                                                                  &shaderResourceView))) {
                
                mTexture.reset(0);
                log_error("error creating shader source view for texture");

                return false;
            }
            mShaderResourceView = MakeCOMPtr(shaderResourceView);
        }

        mTexture->GetDesc(&mDesc);
        mFormat = format;
        mNumMipmaps = mipmaps;
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
            uint32 size = this->width() * this->height() * GetElementSize(this->getFormat());
            uint8* texData = new uint8[size];
            memcpy(texData, mappedTex.pData, size);

            mTexture->Unmap(D3D11CalcSubresource(0, 0, 1));
            return SharedPtr<uint8>(texData);
        }
        return SharedPtr<uint8>();
#endif
        if(mStagingTexture) {
            mDevice->getD3DDeviceContext()->CopyResource(mStagingTexture.get(), mTexture.get());
            D3D11_MAPPED_SUBRESOURCE mappedTex;

      //      uint32 size = this->width() * this->height() * GetElementSize(this->format());
       //     uint8* texData = mist_malloc_t(uint8, size);
        
            if(FAILED(mDevice->getD3DDeviceContext()->Map(mStagingTexture.get(), 
                                                            0, 
                                                            D3D11_MAP_WRITE, 
                                                            0, 
                                                            &mappedTex))) {
                log_error("error mapping texture2d data");
                return 0;
            }
       //     memcpy(texData, mappedTex.pData, size);
      //      mTexData = texData;
        
      //      mDevice->getD3DDeviceContext()->Unmap(mTexture.get(), 0);
            return mappedTex.pData;
        }
        return 0;
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
            
#else
            if(mStagingTexture) {
                mDevice->getD3DDeviceContext()->Unmap(mStagingTexture.get(), 0);
                mDevice->getD3DDeviceContext()->CopyResource(mTexture.get(), 
                                                             mStagingTexture.get());
                /*
                mDevice->getD3DDeviceContext()->UpdateSubresource(mTexture.get(),
                                                                  0,
                                                                  0,
                                                                  mTexData,
                                                                  this->width() * GetElementSize(this->format()),
                                                                  0);*/
            //mist_free(mTexData);
            }
#endif
        }
    }

    void D3D11Texture2D::updateData(const mist::Rectangle& rect, const uint8* data, uint32 level) {
        if(mTexture) {
            D3D11_BOX box;
            box.top = rect.y(); box.left = rect.x();
            box.bottom = rect.bottom(); box.right = rect.right();
            mDevice->getD3DDeviceContext()->UpdateSubresource(mTexture.get(),
                                                              level,
                                                              &box,
                                                              data,
                                                              rect.width() * GetElementSize(this->format()),
                                                              0);
        }
    }

    uint32 D3D11Texture2D::width(uint32 level) const {
        if(mTexture) {
            if(level == 0 || level == 1)
                return mDesc.Width;
            return mDesc.Width / (1U << (level-1));
        }
    }

    uint32 D3D11Texture2D::height(uint32 level) const {
        if(mTexture) {
            if(level == 0 || level == 1)
                return mDesc.Height;
           
            return mDesc.Height / (1U << (level-1));
        }
    }

    uint32 D3D11Texture2D::depth(uint32 level) const {
        return 0;
    }

    void D3D11Texture2D::generateMipmaps() {

    }

    uintPtr D3D11Texture2D::getTextureId() const {
        return (uintPtr)mTexture.get();
    }

    ID3D11Texture2D* D3D11Texture2D::getTexture() const {
        return mTexture.get();
    }

} // namespace ukn