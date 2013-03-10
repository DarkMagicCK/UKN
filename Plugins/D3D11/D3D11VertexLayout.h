#ifndef UKN_D3D11_VERTEX_LAYOUT_H_
#define UKN_D3D11_VERTEX_LAYOUT_H_

#include "D3D11Preq.h"
#include "UKN/Vertex.h"

namespace ukn {

    class D3D11GraphicDevice;

    class D3D11VertexLayout {
    public:
        D3D11VertexLayout(const vertex_elements_type& format, 
                            void* code, 
                            uint32 signature,
                            D3D11GraphicDevice* device,
                            uint32 instance_start,
                            uint32 instance_end);
        ~D3D11VertexLayout();

        ID3D11InputLayout* getD3DLayout() const;

        bool operator==(uint32 sig) const;

    private:
        uint32 mSignature;
        COM<ID3D11InputLayout>::Ptr mLayout;
    };

    typedef SharedPtr<D3D11VertexLayout> D3D11VertexLayoutPtr;

} // namespace ukn


#endif // UKN_D3D11_VERTEX_LAYOUT_H_