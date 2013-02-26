#ifndef UKN_DEFERRED_H_
#define UKN_DEFERRED_H_

#include "UKN/PreDeclare.h"
#include "UKN/Shader.h"
#include "UKN/RenderTarget.h"

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
    class UKN_API DeferredRenderer: Uncopyable {
    public:
        /* if size = (0, 0), then will use screen size */
        DeferredRenderer(const float2& size = float2(0, 0));
        ~DeferredRenderer();

        const EffectPtr& getEffect() const;
        const CompositeRenderTargetPtr& getGBufferRT() const;
        const CompositeRenderTargetPtr& getLightMapRT() const;
        const CompositeRenderTargetPtr& getCompositeRT() const;

        float2 size() const;

        void begin(const LightManagerPtr& lights);

        void renderBuffer(const RenderBufferPtr& buffer, const TexturePtr& tex, const Matrix4& worldMat);

        void startRenderBuffer(const Matrix4& worldMat, const TexturePtr& tex);
        void endRenderBuffer();

        void end();

    private:
        bool init();
        void makeLightMap(const LightManagerPtr& lights);
        void composite();

        bool mBegan;

        float2 mSize;

        EffectPtr mEffect;
        EffectPassPtr mClearPass;
        EffectPassPtr mGBufferPass;
        EffectPassPtr mDirectionalLightPass;
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

        LightManagerPtr mLights;
    };

    typedef SharedPtr<DeferredRenderer> DeferredRendererPtr;


}


#endif // UKN_DEFERRED_H_