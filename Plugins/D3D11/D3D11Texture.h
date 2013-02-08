
#ifndef Project_Unknown_D3D11Texture_h
#define Project_Unknown_D3D11Texture_h

#include "mist/Platform.h"

#include "UKN/Texture.h"

#include "D3D11Preq.h"

namespace ukn {

	class D3D11GraphicDevice;

    class D3D11Texture: public Texture {
    public:
        D3D11Texture(TextureType type, uint32 sample_count, uint32 sample_quality, uint32 access_hint, D3D11GraphicDevice* device);
        virtual ~D3D11Texture();

        virtual uint32 width(uint32 level = 0) const override;
        virtual uint32 height(uint32 level = 0) const override;
        virtual uint32 depth(uint32 level = 0) const override;

        virtual void generateMipmaps() override;
        virtual uintPtr getTextureId() const override;
        
        /* may be null if bindFlag & TB_Texture = 0 */
        ID3D11ShaderResourceView* getShaderResourceView() const;

    protected:
        COM<ID3D11ShaderResourceView>::Ptr mShaderResourceView;
        D3D11GraphicDevice* mDevice;
    };

	class D3D11Texture2D: public D3D11Texture {
	public:
		D3D11Texture2D(D3D11GraphicDevice* device);
		virtual ~D3D11Texture2D();

		/* directly created in shader resource view, getTexture = 0 */
		bool load(const ResourcePtr& rsrc, bool createMipmap);
		/* loaded into texture first, then create resource view */
        bool create(uint32 w, uint32 h, uint32 mipmas, ElementFormat format, const uint8* initialData, uint32 bindFlag);
        
        uint32 width(uint32 level = 0) const override;
        uint32 height(uint32 level = 0) const override;
        uint32 depth(uint32 level = 0) const override;
        
        void generateMipmaps() override;

		/* uintPtr of ID3D11Texture2D */
        uintPtr getTextureId() const override;

        void* map(uint32 level = 0);
        void unmap();
        
		ID3D11Texture2D* getTexture() const;

	private:
		COM<ID3D11Texture2D>::Ptr mTexture;
	};

    class D3D11TextureCube: public D3D11Texture {
    public:
        D3D11TextureCube(D3D11GraphicDevice* device);
        virtual ~D3D11TextureCube();

        bool load(const ResourcePtr& rsrc, bool createMipmap);
        // 6 faces
        bool create(uint32 w, uint32 h, uint32 mipmaps, ElementFormat format, const uint8* initialData[6], uint32 bindFlag);

    };

} // namespace ukn

#endif