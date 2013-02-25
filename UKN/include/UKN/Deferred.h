#ifndef UKN_DEFERRED_H_
#define UKN_DEFERRED_H_

#include "UKN/PreDeclare.h"

namespace ukn {

    /* deferred lighting implementation */
    /* 
        pass 0 = clear G buffer
        pass 1 = G buffer
        pass 2 = ligtings
        pass 3 = composite
        ...
    */
    /*
        ATT_Color0 = Color: RGBA8
        ATT_Color1 = Normal: RGBA8
        ATT_Color2 = Depth: R32
        ATT_DepthStencil = depth buffer for depth test: D16
    */
    class DeferredRenderer: Uncopyable {
    public:
        /* if size = (0, 0), then will use screen size */
        DeferredRenderer(const float2& size = float2(0, 0));
        ~DeferredRenderer();

        const EffectPtr& getEffect() const;
        const CompositeRenderTargetPtr& getGBufferRT() const;
        const CompositeRenderTargetPtr& getLightMapRT() const;
        const CompositeRenderTargetPtr& getCompositeRT() const;


        float2 size() const;

    private:
        bool init();

        float2 mSize;

        EffectPtr mEffect;
        EffectPassPtr mClearPass;
        EffectPassPtr mGBufferPass;
        EffectPassPtr mDirectLightPass;
        EffectPassPtr mPointLightPass;
        EffectPassPtr mSpotlightPass;
        EffectPassPtr mCompositePass;

        CompositeRenderTargetPtr mGBufferRT;
        CompositeRenderTargetPtr mLightMapRT;
        CompositeRenderTargetPtr mCompositeRT;

        BlendStatePtr mLightMapBS;
        RasterizerStatePtr mWireframeRS;

        RenderBufferPtr mPointLightGeometry;
        RenderBufferPtr mSpotLightGeometry;
    };

    typedef SharedPtr<DeferredRenderer> DeferredRendererPtr;


}


#endif // UKN_DEFERRED_H_