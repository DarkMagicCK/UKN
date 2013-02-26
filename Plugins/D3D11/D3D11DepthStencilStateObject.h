#include "UKN/DepthStencilStateObject.h"
#include "D3D11Preq.h"

namespace ukn {

    class D3D11GraphicDevice;

    class D3D11DepthStencilStateObject: public DepthStencilStateObject {
    public:
        D3D11DepthStencilStateObject(const DepthStencilStateDesc& desc, D3D11GraphicDevice* device);
        virtual ~D3D11DepthStencilStateObject();

        ID3D11DepthStencilState* getD3DDepthStencilState() const;

    private:
        COM<ID3D11DepthStencilState>::Ptr mState;
    };

}