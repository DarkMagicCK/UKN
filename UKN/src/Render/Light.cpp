#include "UKN/Light.h"
#include "UKN/GraphicFactory.h"
#include "UKN/RenderTarget.h"
#include "UKN/Texture.h"

namespace ukn {

    LightSource::LightSource(LightSourceType type):
    mType(type) {
    
    }

    LightSource::~LightSource() {

    }

    void LightSource::update() {
    
    }

    LightSourceType LightSource::type() const {
        return mType;
    }

    DirectionalLight::DirectionalLight():
    LightSource(LS_Directional),
    mDir(float3(0, 0, 0)),
    mColor(float4(1, 1, 1, 1)),
    mIntensity(0.f) {

    }

    DirectionalLight::DirectionalLight(const float3& _dir, const float4& _color, float _intensity):
    LightSource(LS_Directional),
    mDir(_dir),
    mColor(_color),
    mIntensity(_intensity) {

    }

    DirectionalLight::DirectionalLight(const DirectionalLight& rhs):
    LightSource(LS_Directional),
    mDir(rhs.mDir),
    mColor(rhs.mColor),
    mIntensity(rhs.mIntensity) {

    }

    DirectionalLight::~DirectionalLight() {

    }

    DirectionalLight& DirectionalLight::operator=(const DirectionalLight& rhs) {
        if(this != &rhs) {
            mDir = rhs.mDir;
            mColor = rhs.mColor;
            mIntensity = rhs.mIntensity;
        }
        return *this;
    }

    const float3& DirectionalLight::getDirection() const {
        return mDir;
    }

    const float4& DirectionalLight::getColor() const {
        return mColor;
    }

    float DirectionalLight::getIntensity() const {
        return mIntensity;
    }

    DirectionalLight& DirectionalLight::setDirection(const float3& dir) {
        mDir = dir;
        return *this;
    }

    DirectionalLight& DirectionalLight::setColor(const float4& color) {
        mColor = color;
        return *this;
    }

    DirectionalLight& DirectionalLight::setIntensity(float intensity) {
        mIntensity = intensity;
        return *this;
    }

    SpotLight::SpotLight(const float3& position, const float3& direction,
                         const float4& color, float intensity,
                         bool castShadows, int shadowMapResolution,
                         const TexturePtr& attenuationTex):
    LightSource(LS_Spot),
    mPosition(position),
    mDirection(direction),
    mColor(color),
    mIntensity(intensity),
    mCastShadows(castShadows),
    mShadowMapResolution(shadowMapResolution),
    mAttenuationTexture(attenuationTex),
    mNearPlane(0.1f),
    mFarPlane(1000.f),
    mFOV(d_pi_2),
    mDepthBias(1.f / 2000.f) {

        mShadowMap = new RenderTarget(mShadowMapResolution,
                                      mShadowMapResolution,
                                      EF_D24S8);

        this->update();

    }

    SpotLight::SpotLight():
    LightSource(LS_Spot) {

    }

    SpotLight::~SpotLight() {

    }

    const float3& SpotLight::getPosition() const {
        return mPosition;
    }

    const float3& SpotLight::getDirection() const {
        return mDirection;
    }

    const float4& SpotLight::getColor() const {
        return mColor;
    }

    float SpotLight::getIntensity() const {
        return mIntensity;
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

    bool SpotLight::getCastShadows() const {
        return mCastShadows;
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

    SpotLight& SpotLight::setPosition(const float3& position) {
        mPosition = position;
        return *this;
    }

    SpotLight& SpotLight::setColor(const float4& color) {
        mColor = color;
        return *this;
    }

    SpotLight& SpotLight::setColor(const Color& color) {
        mColor = color.c;
        return *this;
    }

    SpotLight& SpotLight::setIntensity(float intensity) {
        mIntensity = intensity;
        return *this;
    }

    SpotLight& SpotLight::setCastShadows(bool flag) {
        mCastShadows = flag;
        return *this;
    }

    SpotLight& SpotLight::setDepthBias(float bias) {
        mDepthBias = bias;
        return *this;
    }

    SpotLight& SpotLight::setAttenuationTexture(const TexturePtr& attenuationTex) {
        mAttenuationTexture = attenuationTex;
        return *this;
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