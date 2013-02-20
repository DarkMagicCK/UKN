#ifndef UKN_D3D11RasterizerStateObject_H_
#define UKN_D3D11RasterizerStateObject_H_

#include "UKN/RasterizerStateObject.h"
#include "D3D11Preq.h"

namespace ukn {

    class D3D11GraphicDevice;

    class D3D11RasterizerStateObject: public RasterizerStateObject {
    public:
        D3D11RasterizerStateObject(const RasterizerStateDesc& desc, ID3D11RasterizerState* state);
        D3D11RasterizerStateObject(const RasterizerStateDesc& desc, D3D11GraphicDevice* device);
        virtual ~D3D11RasterizerStateObject();
        
        ID3D11RasterizerState* getD3D11RasterizerState() const;

    private:
        COM<ID3D11RasterizerState>::Ptr mRasterizerState;
    };
}

#endif // UKN_D3D11RasterizerStateObject_H_