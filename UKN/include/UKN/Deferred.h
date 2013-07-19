#ifndef UKN_DEFERRED_H_
#define UKN_DEFERRED_H_

#include "UKN/PreDeclare.h"
#include "UKN/Shader.h"
#include "UKN/RenderTarget.h"
#include "UKN/RasterizerStateObject.h"
#include "UKN/BlendStateObject.h"

#include "mist/Factory.h"

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
    class UKN_API DeferredRenderer: Uncopyable, public enable_shared_from_this<DeferredRenderer> {
    public:
        /* if size = (0, 0), then will use screen size */
        DeferredRenderer(const float2& size = float2(0, 0));
        ~DeferredRenderer();

        const EffectPtr& getEffect() const;
        const CompositeRenderTargetPtr& getGBufferRT() const;
        const CompositeRenderTargetPtr& getLightMapRT() const;
        const CompositeRenderTargetPtr& getCompositeRT() const;

        RenderTargetPtr getColorTarget() const;
        RenderTargetPtr getDepthTarget() const;
        RenderTargetPtr getNormalTarget() const;
        RenderTargetPtr getLightTarget() const;
        
        TexturePtr getFinalTexture() const;

        float2 size() const;

        // render currrent scene, see Context::getSceneManager()
        void renderScene(SceneManager& scene);

    public:
        typedef mist::Factory<PostEffect> PostEffectFactory;
        typedef std::vector<std::pair<MistString, PostEffectPtr> > PostEffectQueue;
    
        template<typename T>
        void registerPostEffect(const MistString& name) {
            mPostEffectFactory.registerClassCtor<T>(name);
        }

        void deregisterPostEffect(const MistString& name);

        /* if after = "", then the posteffect is appended to the end,
            otherwise after a certain posteffect,
            post effects must be registered with DeferredRenderer::registerPostEffect before calling add 
            post effects intergrated into ukn are registered automatically, including
                SSAO,
                Fog,
                (to do)
         */
        bool addPostEffect(const MistString& name, const MistString& after = L"");
        void removePostEffect(const MistString& name);

        const PostEffectQueue& getPostEffects() const;
        PostEffectPtr getPostEffect(const MistString& name) const;

    public:
        void reloadShaders();

    private:
        PostEffectQueue mPostEffects;
        PostEffectFactory mPostEffectFactory;

    private:
        bool init();
        
        void prepare();
        void makeGBuffer(SceneManager& scene);
        void makeLightMap(SceneManager& scene);
        void makeFinal();

        float2 mSize;

        EffectPtr mEffect;
        EffectTechniquePtr mClearTechnique;
        EffectTechniquePtr mGBufferTechnique;
        EffectTechniquePtr mGBufferBumpMapTechnique;
        EffectTechniquePtr mDirectionalLightTechnique;
        EffectTechniquePtr mSpotLightTechnique;
        EffectTechniquePtr mPointLightTechnique;
        EffectTechniquePtr mCompositeTechnique;

        CompositeRenderTargetPtr mGBufferRT;
        CompositeRenderTargetPtr mLightMapRT;
        CompositeRenderTargetPtr mCompositeRT;

        RenderTargetPtr mLightMap;

        BlendStatePtr mLightMapBS;
        RasterizerStatePtr mWireframeRS;
    };

    typedef SharedPtr<DeferredRenderer> DeferredRendererPtr;

}


#endif // UKN_DEFERRED_H_