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
        LS_Ambient,
    };

    class UKN_API LightSource {
    public:
        LightSource(LightSourceType type);
        virtual ~LightSource();

        virtual void update();

    public:
        LightSourceType type() const;
        
        const float3& getDirection() const;
        const float4& getColor() const;
        const float3& getPosition() const;
        float getIntensity() const;

        void setDirection(const float3& dir);
        void setColor(const float4& color);
        void setColor(const Color& color);
        void setPosition(const float3& pos);
        void setIntensity(float intensity);

        bool getCastShadows() const;
        void setCastShadows(bool flag);

        bool getEnabled() const;
        void setEnabled(bool flag);
        
        const RenderTargetPtr& getShadowMap() const;
        const TexturePtr& getAttenuationTexture() const;
        const RenderTargetPtr& getDSView() const;
        void setAttenuationTexture(const TexturePtr& attenuationTex);

        int32 getShadowMapResolution() const;

        const Matrix4& getWorldMatrix() const;
        
    public:
        virtual const CameraPtr& getCamera(uint32 index = 0) const;

    protected:
        LightSourceType mType;

        float3 mDirection;
        float4 mColor;
        float3 mPosition;
        float  mIntensity;
        bool   mCastShadows;
        bool   mEnabled;

        Matrix4 mWorldMat;

        RenderTargetPtr mShadowMap;
        RenderTargetPtr mDSView;
        int32 mShadowMapResolution;
        TexturePtr mAttenuationTexture;
    };

    typedef SharedPtr<LightSource> LightSourcePtr;

    class UKN_API DirectionalLight: public LightSource {
    public:
        DirectionalLight();
        DirectionalLight(const float3& dir, const float4& color, float intensity);
        ~DirectionalLight();
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

        float getNearPlane() const;
        float getFarPlane() const;
        float getFOV() const;
        float getDepthBias() const;

        void setDepthBias(float bias);
        void update();
        float lightAngleCos();

        const CameraPtr& getCamera(uint32 index) const;

    private:
        float mNearPlane;
        float mFarPlane;
        float mFOV;
        float mDepthBias;

        CameraPtr mCam;
    };

    typedef SharedPtr<SpotLight> SpotLightPtr;

} // namespace ukn


#endif // UKN_LIGHT_H_