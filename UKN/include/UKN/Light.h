#ifndef UKN_LIGHT_H_
#define UKN_LIGHT_H_

#include "UKN/PreDeclare.h"

#include "mist/Color.h"
#include "mist/MathUtil.h"

namespace ukn {

    enum LightSourceType {
        LS_Directional,
        LS_Spot,
        LS_Point,
    };

    class UKN_API LightSource {
    public:
        LightSource(LightSourceType type);
        virtual ~LightSource();

        virtual void update();

        LightSourceType type() const;

    private:
        LightSourceType mType;
    };

    typedef SharedPtr<LightSource> LightSourcePtr;

    class UKN_API DirectionalLight: public LightSource {
    public:
        DirectionalLight();
        DirectionalLight(const float3& dir, const float4& color, float intensity);
        DirectionalLight(const DirectionalLight& rhs);
        ~DirectionalLight();

        DirectionalLight& operator=(const DirectionalLight& rhs);

        const float3& getDirection() const;
        const float4& getColor() const;
        float getIntensity() const;

        DirectionalLight& setDirection(const float3& dir);
        DirectionalLight& setColor(const float4& color);
        DirectionalLight& setIntensity(float intensity);

    private:
        float3 mDir;
        float4 mColor;
        float mIntensity;
    };

    typedef SharedPtr<DirectionalLight> DirectionalLightPtr;

    class UKN_API SpotLight: public LightSource {
    public:
        SpotLight();
        SpotLight(const float3& position, const float3& direction,
                  const float4& color, float intensity,
                  bool castShadows, int shadowMapResolution,
                  const TexturePtr& attenuationTex);
        ~SpotLight();

        const float3& getPosition() const;
        const float3& getDirection() const;
        const float4& getColor() const;
        float getIntensity() const;
        float getNearPlane() const;
        float getFarPlane() const;
        float getFOV() const;
        bool getCastShadows() const;
        float getDepthBias() const;
        const Matrix4& getWorldMat() const;
        const Matrix4& getViewMat() const;
        const Matrix4& getProjMat() const;
        const RenderTargetPtr& getShadowMap() const;
        const TexturePtr& getAttenuationTexture() const;

        SpotLight& setPosition(const float3& position);
        SpotLight& setColor(const float4& color);
        SpotLight& setColor(const Color& color);
        SpotLight& setIntensity(float intensity);
        SpotLight& setCastShadows(bool flag);
        SpotLight& setDepthBias(float bias);
        SpotLight& setAttenuationTexture(const TexturePtr& attenuationTex);

        void update();
        float lightAngleCos();

    private:
        float3 mPosition;
        float3 mDirection;
        float4 mColor;
        float mIntensity;
        float mNearPlane;
        float mFarPlane;
        float mFOV;
        bool mCastShadows;
        int32 mShadowMapResolution;
        float mDepthBias;
        Matrix4 mWorldMat;
        Matrix4 mViewMat;
        Matrix4 mProjectionMat;

        RenderTargetPtr mShadowMap;
        TexturePtr mAttenuationTexture;
    };

    typedef SharedPtr<SpotLight> SpotLightPtr;

} // namespace ukn


#endif // UKN_LIGHT_H_