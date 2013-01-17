
#ifndef Project_Unknown_D3D10Texture_h
#define Project_Unknown_D3D10Texture_h

#include "mist/Platform.h"

#include "UKN/Texture.h"

#include "D3D10Preq.h"

namespace ukn {

	class D3D10GraphicDevice;

	class D3D10Texture2D: public Texture {
	public:
		D3D10Texture2D(D3D10GraphicDevice* device);
		virtual ~D3D10Texture2D();

		/* directly created in shader resource view, getTexture = 0 */
		bool load(const ResourcePtr& rsrc, bool createMipmap);
		/* loaded into texture first, then create resource view */
        bool create(uint32 w, uint32 h, uint32 mipmas, ElementFormat format, const uint8* initialData);
        
        SharedPtr<uint8> readTextureData(uint8 level);
        void updateTextureData(void* data, int32 x, int32 y, uint32 width, uint32 height, uint8 level);
		
        uint32 getWidth(uint32 level = 0) const override;
        uint32 getHeight(uint32 level = 0) const override;
        
		/* uintPtr of texture resource view instead of texture */
        uintPtr getTextureId() const override;
        
		ID3D10Texture2D* getTexture() const;
		ID3D10ShaderResourceView* getTextureResourceView() const;

	private:
		ID3D10ShaderResourceView* mTextureResourceView;
		ID3D10Texture2D* mTexture;
		D3D10GraphicDevice* mDevice;
	};

} // namespace ukn

#endif