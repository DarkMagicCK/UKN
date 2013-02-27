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
    mEnabled(true) {
    
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
    
    void LightSource::setCastShadows(bool flag) {
        mCastShadows = flag;
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
    mShadowMapResolution(shadowMapResolution),
    mAttenuationTexture(attenuationTex),
    mNearPlane(0.1f),
    mFarPlane(1000.f),
    mFOV(d_pi_2),
    mDepthBias(1.f / 2000.f) {

        mShadowMap = new RenderTarget(mShadowMapResolution,
                                      mShadowMapResolution,
                                      EF_D24S8);

        mPosition = position;
        mDirection = direction;
        mColor = color;
        mIntensity = intensity;
        mCastShadows = castShadows;
        
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

    const Matrix4& SpotLight::getWorldMat() const {
        return mWorldMat;
    }

    const Matrix4& SpotLight::getViewMat() const {
        return mViewMat;
    }

    const Matrix4& SpotLight::getProjMat() const {
        return mProjectionMat;
    }

    const RenderTargetPtr& SpotLight::getShadowMap() const {
        return mShadowMap;
    }

    const TexturePtr& SpotLight::getAttenuationTexture() const {
        return mAttenuationTexture;
    }
    
    void SpotLight::setDepthBias(float bias) {
        mDepthBias = bias;
    }

    void SpotLight::setAttenuationTexture(const TexturePtr& attenuationTex) {
        mAttenuationTexture = attenuationTex;
    }

    float SpotLight::lightAngleCos() {
        return cosf(mFOV);
    }

    void SpotLight::update() {
        float3 target = mPosition + mDirection;
        if(target == float3()) 
            target = -Vector3::Up();

        float3 up = math::cross(mDirection, Vector3::Up());
        if(up == float3()) 
            up = Vector3::Right();
        else
            up = Vector3::Up();

        mViewMat = Matrix4::LookAtMatLH(mPosition, target, up);
        float radial = float(tanf(mFOV / 2.0) * 2 * mFarPlane);

        Matrix4 scaling = Matrix4::ScaleMat(radial, radial, mFarPlane);
        Matrix4 translation = Matrix4::TransMat(mPosition[0], mPosition[1], mPosition[2]);
        Matrix4 inverseView = mViewMat.inverted();
        Matrix4 semiProduct = scaling * inverseView;

        Vector3 scale, trans, rot;
        semiProduct.decompose(trans, rot, scale);

        mWorldMat = scaling * Matrix4::RotMat(rot.x(), rot.y(), rot.z()) * translation;
    }

} // namespace ukn