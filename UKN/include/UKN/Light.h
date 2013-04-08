#ifndef UKN_LIGHT_H_
#define UKN_LIGHT_H_

#include "UKN/PreDeclare.h"

#include "mist/Color.h"
#include "mist/MathUtil.h"

#include "UKN/RenderTarget.h"

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

    public:
        LightSourceType type() const;
        
        const float3& getDirection() const;
        const float4& getColor() const;
        const float3& getPosition() const;
        float getIntensity() const;
        float getDepthBias() const;

        virtual void setDirection(const float3& dir);
        virtual void setColor(const float4& color);
        virtual void setColor(const Color& color);
        virtual void setPosition(const float3& pos);
        virtual void setIntensity(float intensity);
        virtual void setCastShadows(bool flag);
        virtual void setDepthBias(float bias);

        bool getCastShadows() const;
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
        float  mDepthBias;
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
        DirectionalLight(const float3& dir,
                         const float4& color, float intensity,
                         bool castShadows = false, int shadowMapResolution = 256);
        virtual ~DirectionalLight();

    public:
        const CameraPtr& getCamera(uint32 index) const override;
        void setCastShadows(bool flag) override;

        void setPosition(const float3& pos) override;
        void setDirection(const float3& dir) override;

    private:
        void updateCamera();

        CameraPtr mCamera;
    };

    typedef SharedPtr<DirectionalLight> DirectionalLightPtr;

    class UKN_API SpotLight: public LightSource {
    public:
        SpotLight();
        SpotLight(const float3& position, const float3& direction,
                  const float4& color, float intensity,
                  const TexturePtr& attenuationTex,
                  bool castShadows = false, int shadowMapResolution = 256);
        virtual ~SpotLight();

        float getFOV() const;
        float lightAngleCos();

    public:
        const CameraPtr& getCamera(uint32 index) const override;
        
        void setCastShadows(bool flag) override;

    private:
        void updateCamera();

        float mFOV;
        CameraPtr mCam;
    };

    typedef SharedPtr<SpotLight> SpotLightPtr;

    class UKN_API PointLight: public LightSource {
    public:
        PointLight();
        PointLight(const float3& position, float radius,
                   const float4& color, float intensity,
                   bool castShadows = false, int shadowMapResolution = 256);
        virtual ~PointLight();

        float getRadius() const;
        void setRadius(float r);

    public:
        const CameraPtr& getCamera(uint32 index) const override;
        
        void setPosition(const float3& pos);

    private:
        void updateCamera();

        CameraPtr mCamera[6];
        float mRadius;
    };

    typedef SharedPtr<PointLight> PointLightPtr;

} // namespace ukn


#endif // UKN_LIGHT_H_