#ifndef UKN_SSAO_H_
#define UKN_SSAO_H_

#include "UKN/PreDeclare.h"
#include "UKN/PostEffect.h"

namespace ukn {

    /* SSAO(ScreenSpace Ambient Occulusion) Implementation  */
    class UKN_API SSAO: public PostEffect {
    public:
        SSAO(const float2& size = float2(0, 0));
        virtual ~SSAO();

        const EffectPtr& getEffect() const;
        const RenderTargetPtr& getSSAOTarget() const;
        const RenderTargetPtr& getSSAOBlurTarget() const;
        const RenderTargetPtr& getCompositeTarget() const;

        float2 size() const;

        float getSampleRadius() const;
        float getDistanceScale() const;

        void setSampleRadius(float r);
        void setDistanceScale(float r);

        const TexturePtr& getFinalTexture() const;

    public:
        // render currrent scene, see Context::getSceneManager()
        void render(const CompositeRenderTargetPtr& gbuffer, const TexturePtr& target);

    private:
        bool init();
        void makeSSAO(const CompositeRenderTargetPtr& gbuffer);
        void makeSSAOBlur(const CompositeRenderTargetPtr& gbuffer);
        void makeFinal(const TexturePtr& target);

        CompositeRenderTargetPtr mRT;
        RenderTargetPtr mSSAOTarget;
        RenderTargetPtr mSSAOBlurTarget;
        RenderTargetPtr mCompositeTarget;
        
        EffectPtr mEffect;
        EffectTechniquePtr mSSAOTechnique;
        EffectTechniquePtr mSSAOBlurTechnique;
        EffectTechniquePtr mCompositeTechnique;

        float2 mSize;
        float mSampleRadius;
        float mDistanceScale;

        TexturePtr mRandomNormalTex;
    };

    typedef SharedPtr<SSAO> SSAOPtr;

} // namespace ukn


#endif // UKN_SSAO_H_