#ifndef UKN_D3D11SamplerStateObject_H_
#define UKN_D3D11SamplerStateObject_H_

#include "UKN/SamplerStateObject.h"

#include "D3D11Preq.h"

namespace ukn {
    
    class D3D11GraphicDevice;

    class D3D11SamplerStateObject: public SamplerStateObject {
    public:
        D3D11SamplerStateObject(const SamplerStateDesc& desc, ID3D11SamplerState* state);
        D3D11SamplerStateObject(const SamplerStateDesc& desc, D3D11GraphicDevice* device);
        virtual ~D3D11SamplerStateObject();

        ID3D11SamplerState* getD3DSamplerState() const;

    private:
        COM<ID3D11SamplerState>::Ptr mSamplerState;
    };

}

#endif