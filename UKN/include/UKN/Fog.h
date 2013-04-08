#ifndef PROJECT_UNKNOWN_FOG_H_
#define PROJECT_UNKNOWN_FOG_H_

#include "UKN/PreDeclare.h"
#include "UKN/PostEffect.h"

#include "mist/Color.h"

namespace ukn {

    class Fog: public PostEffect {
    public:
        Fog();
        virtual ~Fog();

        enum FogType {
            Linear,
            Exponential,
            Exponential2,
        };

    public:
        void render(const TexturePtr& color,
                    const TexturePtr& normal,
                    const TexturePtr& depth,
                    const TexturePtr& target) override;
        bool init(float2 size) override;
        const TexturePtr& getFinalTexture() const override;

    private:
        mist::Color mColor;
        float mStart, mEnd;
        FogType mType;
        
        RenderTargetPtr mTarget;
        
        EffectPtr mEffect;
        EffectTechniquePtr mFogTechnique;
    };

    typedef SharedPtr<Fog> FogPtr;
}

#endif