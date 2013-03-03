#include "UKN/Light.h"
#include "UKN/GraphicFactory.h"
#include "UKN/RenderTarget.h"
#include "UKN/Texture.h"

namespace ukn {

    LightSource::LightSource(LightSourceType type):
    mType(type),
    mDirection(float3(0, 0, 0)),
    mColor(float4(1, 1, 1, 1)),
    mPosition(float3(0, 0, 0)),
    mIntensity(0.f),
    mCastShadows(false),
    mEnabled(true),
    mShadowMapResolution(0) {
    }

    LightSource::~LightSource() {

    }

    void LightSource::update() {
    
    }

    LightSourceType LightSource::type() const {
        return mType;
    }
    
    const float3& LightSource::getPosition() const {
        return mPosition;
    }

    const float3& LightSource::getDirection() const {
        return mDirection;
    }

    const float4& LightSource::getColor() const {
        return mColor;
    }

    bool LightSource::getCastShadows() const {
        return mCastShadows;
    }

    void LightSource::setPosition(const float3& position) {
        mPosition = position;
    }

    void LightSource::setColor(const float4& color) {
        mColor = color;
    }

    void LightSource::setColor(const Color& color) {
        mColor = color.c;
    }

    int32 LightSource::getShadowMapResolution() const {
        return mShadowMapResolution;
    }
    
    void LightSource::setCastShadows(bool flag) {
        mCastShadows = flag;
        if(!mShadowMap && mShadowMapResolution > 0) {
            mShadowMap = new RenderTarget(mShadowMapResolution,
                                          mShadowMapResolution,
                                          1,
                                          EF_Float);
            if(!mShadowMap)
                log_error(L"LightSource::setCastShadows: error creating shadow map");
        
            mDSView = new RenderTarget(mShadowMapResolution,
                                       mShadowMapResolution,
                                       EF_D24S8);
        }
    }

     const CameraPtr& LightSource::getCamera(uint32 index) const {
         static CameraPtr cam;
         return cam;
     }

    const RenderTargetPtr& LightSource::getDSView() const {
        return mDSView;
    }

    void LightSource::setDirection(const float3& dir) {
        mDirection = dir;
    }
    
    float LightSource::getIntensity() const {
        return mIntensity;
    }
    
    void LightSource::setIntensity(float intensity) {
        mIntensity = intensity;
    }

    bool LightSource::getEnabled() const {
        return mEnabled;
    }

    void LightSource::setEnabled(bool flag) {
        mEnabled = flag;
    }
    
    const RenderTargetPtr& LightSource::getShadowMap() const {
        return mShadowMap;
    }

    const TexturePtr& LightSource::getAttenuationTexture() const {
        return mAttenuationTexture;
    }
    
    void LightSource::setAttenuationTexture(const TexturePtr& attenuationTex) {
        mAttenuationTexture = attenuationTex;
    }

    const Matrix4& LightSource::getWorldMatrix() const {
        return mWorldMat;
    }

    DirectionalLight::DirectionalLight():
    LightSource(LS_Directional) {

    }
    
    DirectionalLight::DirectionalLight(const float3& _dir, const float4& _color, float _intensity):
    LightSource(LS_Directional) {
        mDirection = _dir;
        mColor = _color;
        mIntensity = _intensity;
    }

    DirectionalLight::~DirectionalLight() {

    }

    SpotLight::SpotLight(const float3& position, const float3& direction,
                         const float4& color, float intensity,
                         bool castShadows, int shadowMapResolution,
                         const TexturePtr& attenuationTex):
    LightSource(LS_Spot),
    mNearPlane(0.1f),
    mFarPlane(100.f),
    mFOV(d_pi_2),
    mDepthBias(1.f / 2000.f),
    mCam(MakeSharedPtr<Camera>()){

        mPosition = position;
        mDirection = direction;
        mColor = color;
        mIntensity = intensity;
        mShadowMapResolution = shadowMapResolution;
        mAttenuationTexture = attenuationTex;
        
        this->setCastShadows(castShadows);
        
        mCam->setProjParams(mFOV, 1.0f, mNearPlane, mFarPlane);
        this->update();
    }

    SpotLight::SpotLight():
    LightSource(LS_Spot) {

    }

    SpotLight::~SpotLight() {

    }

    float SpotLight::getNearPlane() const {
        return mNearPlane;
    }

    float SpotLight::getFarPlane() const {
        return mFarPlane;
    }

    float SpotLight::getFOV() const {
        return mFOV;
    }

    float SpotLight::getDepthBias() const {
        return mDepthBias;
    }

    void SpotLight::setDepthBias(float bias) {
        mDepthBias = bias;
    }

    float SpotLight::lightAngleCos() {
        return cosf(mFOV);
    }

    void SpotLight::update() {
        float3 target = mPosition + mDirection;
        if(target.sqrLength() == 0) 
            target = -Vector3::Up();

        float3 up = math::cross(mDirection, Vector3::Up());
        if(up.sqrLength() == 0) 
            up = Vector3::Right();
        else
            up = Vector3::Up();

        mCam->setViewParams(mPosition, target, up);
        float radial = float(tanf(mFOV / 2.0f) * 2 * mFarPlane);

        Matrix4 scaling = Matrix4::ScaleMat(radial, radial, mFarPlane);
        Matrix4 translation = Matrix4::TransMat(mPosition[0], mPosition[1], mPosition[2]);
        Matrix4 inverseView = mCam->getViewMatrix().inverted();
        Matrix4 semiProduct = scaling * inverseView;

        Vector3 scale, trans, rot;
        semiProduct.decompose(trans, rot, scale);

        mWorldMat = scaling * Matrix4::RotMat(rot.x(), rot.y(), rot.z()) * translation;
    }

    const CameraPtr& SpotLight::getCamera(uint32 index) const {
        return mCam;
    }
} // namespace ukn