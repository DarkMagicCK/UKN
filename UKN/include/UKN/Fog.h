#ifndef PROJECT_UNKNOWN_FOG_H_
#define PROJECT_UNKNOWN_FOG_H_

#include "UKN/PreDeclare.h"
#include "UKN/PostEffect.h"

#include "mist/Color.h"

namespace ukn {

    class UKN_API Fog: public PostEffect {
    public:
        Fog();
        virtual ~Fog();

        enum FogType {
            Linear = 1,
            Exponential,
            Exponential2,
        };

    public:
        void render(const TexturePtr& color,
                    const TexturePtr& normal,
                    const TexturePtr& depth) override;
        bool init(float2 size) override;
        TexturePtr getFinalTexture() const override;

    public:
        float getStart() const;
        float getEnd() const;
        float getDensity() const;
        FogType getType() const;
        Color getColor() const;

        void setStart(float start);
        void setEnd(float end);
        void setDensity(float density);
        void setType(FogType type);
        void setColor(const Color& color);

        void reloadShaders();

    private:
        mist::Color mColor;
        float mStart;
        float mEnd;
        float mDensity;
        FogType mType;
        
        CompositeRenderTargetPtr mRT;
        RenderTargetPtr mFogTarget;
        
        EffectPtr mEffect;
        EffectTechniquePtr mFogTechnique;
    };

    typedef SharedPtr<Fog> FogPtr;
}

#endif