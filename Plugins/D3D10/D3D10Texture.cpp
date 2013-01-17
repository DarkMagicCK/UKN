#include "D3D10Texture.h"

#include "D3D10GraphicDevice.h"
#include "D3D10Convert.h"

#include "mist/Logger.h"

namespace ukn {

	D3D10Texture2D::D3D10Texture2D(D3D10GraphicDevice* device):
	Texture(TT_Texture2D),
	mTextureResourceView(0),
	mTexture(0),
	mDevice(device) {

	}

	D3D10Texture2D::~D3D10Texture2D() {
		if(mTexture) {
			mTexture->Release();
		}
	}

	bool D3D10Texture2D::load(const ResourcePtr& resource, bool createMipmap) {
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

			if(FAILED(D3DX10CreateShaderResourceViewFromMemory(mDevice->getD3DDevice(),
															   memData->data(),
															   memData->size(),
															   0,
															   0,
															   &mTextureResourceView,
															   0))) {
				mTexture = 0;
				return false;
			}
			return true;
		}
		return false;
	}

	bool D3D10Texture2D::create(uint32 w, uint32 h, uint32 mipmaps, ElementFormat format, const uint8* initialData) {
		D3D10_TEXTURE2D_DESC desc;

		ZeroMemory(&desc, sizeof(desc));
		desc.Width = w;
		desc.Height = h;
		desc.MipLevels = mipmaps;
		desc.ArraySize = 1;
		desc.Format = ElementFormatToDxGIFormat(format);
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D10_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		
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

		D3D10_SUBRESOURCE_DATA textureData;
		textureData.pSysMem = texData;

		if(FAILED(mDevice->getD3DDevice()->CreateTexture2D(&desc, &textureData, &mTexture))) {
			return false;
		}

		D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;

		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture1D.MostDetailedMip = desc.MipLevels - 1;

		if(FAILED(mDevice->getD3DDevice()->CreateShaderResourceView(mTexture, &srvDesc, &mTextureResourceView))) {
			mTexture->Release();
			mTexture = 0;

			log_error("error creating shader source view for texture");

			return false;
		}
		return true;
	}

	SharedPtr<uint8> D3D10Texture2D::readTextureData(uint8 level) {
		D3D10_MAPPED_TEXTURE2D mappedTex;
		if(mTexture) {
			if(FAILED(mTexture->Map(D3D10CalcSubresource(0, 0, 1),
									D3D10_MAP_WRITE_DISCARD,
									0,
									&mappedTex))) {
				log_error("error mapping texture2d data");
				return SharedPtr<uint8>();
			}
			uint32 size = this->getWidth() * this->getHeight() * GetElementSize(this->getFormat());
			uint8* texData = new uint8[size];
			memcpy(texData, mappedTex.pData, size);

			mTexture->Unmap(D3D10CalcSubresource(0, 0, 1));
			return SharedPtr<uint8>(texData);
		}
		return SharedPtr<uint8>();
	}

	void D3D10Texture2D::updateTextureData(void* data, int32 x, int32 y, uint32 width, uint32 height, uint8 level) {
		D3D10_MAPPED_TEXTURE2D mappedTex;
		if(mTexture) {
			if(FAILED(mTexture->Map(D3D10CalcSubresource(0, 0, level),
									D3D10_MAP_WRITE_DISCARD,
									0,
									&mappedTex))) {
				log_error("error mapping texture2d data");
			}
			
			uint8* inPixel = (uint8*)data;
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
			}

			mTexture->Unmap(D3D10CalcSubresource(0, 0, level));
		}
	}

	uint32 D3D10Texture2D::getWidth(uint32 level) const {
		D3D10_TEXTURE2D_DESC desc;
		if(mTexture) {
			mTexture->GetDesc(&desc);
			return desc.Width / (1U << (level - 1));
		}
	}

	uint32 D3D10Texture2D::getHeight(uint32 level) const {
		D3D10_TEXTURE2D_DESC desc;
		if(mTexture) {
			mTexture->GetDesc(&desc);
			return desc.Height / (1U << (level - 1));
		}
	}

	uintPtr D3D10Texture2D::getTextureId() const {
		return (uintPtr)mTextureResourceView;
	}

	ID3D10Texture2D* D3D10Texture2D::getTexture() const {
		return mTexture;
	}

	ID3D10ShaderResourceView* D3D10Texture2D::getTextureResourceView() const {
		return mTextureResourceView;
	}

} // namespace ukn