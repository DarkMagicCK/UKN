#ifndef UKN_OVR_POST_RPOCESS_H_
#define UKN_OVR_POST_PROCESS_H_

#pragma once

#include "UKN/PostEffect.h"

namespace ukn {

    class OVRPostEffect: public ukn::PostEffect {
    public:
        OVRPostEffect();
        virtual ~OVRPostEffect();

    public:
        // ukn::PostEffect
        void render(const TexturePtr& color,
                    const TexturePtr& normap,
                    const TexturePtr& depth) override;
        bool init(float2 size) override;
        
        TexturePtr getFinalTexture() const override;

    private:
        CompositeRenderTargetPtr mRT;
        RenderTargetPtr mTarget;

        EffectPtr mEffect;
    };

}

#endif