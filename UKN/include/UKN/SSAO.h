#ifndef UKN_SSAO_H_
#define UKN_SSAO_H_

#include "UKN/PreDeclare.h"
#include "UKN/PostEffect.h"

namespace ukn {

    /* SSAO(ScreenSpace Ambient Occulusion) Implementation  */
    class UKN_API SSAO: public PostEffect {
    public:
        SSAO();
        virtual ~SSAO();

        const EffectPtr& getEffect() const;
        const RenderTargetPtr& getSSAOTarget() const;
        const RenderTargetPtr& getSSAOBlurTarget() const;
        const RenderTargetPtr& getCompositeTarget() const;

        float getSampleRadius() const;
        float getDistanceScale() const;

        void setSampleRadius(float r);
        void setDistanceScale(float r);

    public:
        // render currrent scene, see Context::getSceneManager()
        void render(const TexturePtr& color,
                    const TexturePtr& normap,
                    const TexturePtr& depth) override;
        bool init(float2 size) override;
        TexturePtr getFinalTexture() const override;

    private:
        void makeSSAO(const TexturePtr& color,
                      const TexturePtr& normap,
                      const TexturePtr& depth);
        void makeSSAOBlur(const TexturePtr& color,
                          const TexturePtr& normap,
                          const TexturePtr& depth);
        void makeFinal(const TexturePtr& color);

        CompositeRenderTargetPtr mRT;
        RenderTargetPtr mSSAOTarget;
        RenderTargetPtr mSSAOBlurTarget;
        RenderTargetPtr mCompositeTarget;
        
        EffectPtr mEffect;
        EffectTechniquePtr mSSAOTechnique;
        EffectTechniquePtr mSSAOBlurTechnique;
        EffectTechniquePtr mCompositeTechnique;

        float mSampleRadius;
        float mDistanceScale;
        float2 mSize;

        TexturePtr mRandomNormalTex;
    };

    typedef SharedPtr<SSAO> SSAOPtr;

} // namespace ukn


#endif // UKN_SSAO_H_