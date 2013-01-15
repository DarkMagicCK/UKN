#include "ukn/Shader.h"
#include "mist/Resource.h"

#include "D3D10Preq.h"

namespace ukn {

	class D3D10GraphicDevice;

	class D3D10Effect: public Shader {
	public:
		D3D10Effect(D3D10GraphicDevice* device);
		virtual ~D3D10Effect();

		bool initialize(const ukn::VertexFormat& format, const mist::Resource& content);

		virtual void bind() override;
		virtual void unbind() override;

	private:
		void logError(ID3D10Blob* error);

	private:
		ID3D10Effect* mEffect;
		ID3D10InputLayout* mLayout;

		D3D10GraphicDevice* mDevice;
	};

}