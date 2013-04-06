#include "UKN/Light.h"
#include "UKN/GraphicFactory.h"
#include "UKN/RenderTarget.h"
#include "UKN/Texture.h"
#include "UKN/GraphicDevice.h"
#include "UKN/FrameBuffer.h"
#include "UKN/Camera.h"
#include "UKN/Context.h"

namespace ukn {

    LightSource::LightSource(LightSourceType type):
    mType(type),
    mDirection(float3(0, 0, 0)),
    mColor(float4(1, 1, 1, 1)),
    mPosition(float3(0, 0, 0)),
    mIntensity(0.f),
    mDepthBias(1.f / 2000.f),
    mCastShadows(false),
    mEnabled(true),
    mShadowMapResolution(0) {

    }

    LightSource::~LightSource() {

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

    void LightSource::setDepthBias(float bias) {
        mDepthBias = bias;
    }

    int32 LightSource::getShadowMapResolution() const {
        return mShadowMapResolution;
    }
    
    void LightSource::setCastShadows(bool flag) {
        mCastShadows = flag;
    }

     const CameraPtr& LightSource::getCamera(uint32 index) const {
         static CameraPtr cam;
         return cam;
     }

    const RenderTargetPtr& LightSource::getDSView() const {
        return mDSView;
    }

    void LightSource::setDirection(const float3& dir) {
        mDirection = dir.normalize();
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

    float LightSource::getDepthBias() const {
        return mDepthBias;
    }

    DirectionalLight::DirectionalLight():
    LightSource(LS_Directional) {

    }
    
    DirectionalLight::DirectionalLight(const float3& _dir, const float4& _color, 
                                       float _intensity, 
                                       bool castShadows, int shadowMapResolution):
    LightSource(LS_Directional),
    mCamera(MakeSharedPtr<Camera>()){
        mDirection = _dir;
        mColor = _color;
        mIntensity = _intensity;
        mShadowMapResolution = shadowMapResolution;
        
        this->setCastShadows(castShadows);
        this->updateCamera();
    }

    DirectionalLight::~DirectionalLight() {

    }

    void DirectionalLight::updateCamera() {
        const GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        const CameraPtr& cp = gd.getCurrFrameBuffer()->getViewport().camera;
        
        float farPlane = 1000.f;
        float nearPlane = 0.1f;
        if(cp) {
            farPlane = cp->getFarPlane();
            nearPlane = cp->getNearPlane();
        }

        mCamera->setProjParamsOrtho(-50, 50, -50, 50, nearPlane, farPlane);
        
        float3 target = mPosition + mDirection;
        if(target.sqrLength() == 0) 
            target = -Vector3::Up();

        float3 up = math::cross(mDirection, Vector3::Up());
        if(up.sqrLength() == 0) 
            up = Vector3::Right();
        else
            up = Vector3::Up();

        mCamera->setViewParams(mPosition, target, up);
    }

    const CameraPtr& DirectionalLight::getCamera(uint32 index) const {
        return mCamera;
    }

    void DirectionalLight::setCastShadows(bool flag) {
        LightSource::setCastShadows(flag);

        if(flag && !mShadowMap && mShadowMapResolution > 0) {
            mShadowMap = new RenderTarget(mShadowMapResolution,
                                          mShadowMapResolution,
                                          1,
                                          EF_RGBA8);
            if(!mShadowMap)
                log_error(L"LightSource::setCastShadows: error creating shadow map");
        
            mDSView = new RenderTarget(mShadowMapResolution,
                                       mShadowMapResolution,
                                       EF_D24S8);
        }
    }

    SpotLight::SpotLight(const float3& position, const float3& direction,
                         const float4& color, float intensity,
                         const TexturePtr& attenuationTex,
                         bool castShadows, int shadowMapResolution):
    LightSource(LS_Spot),
    mFOV(pi_2),
    mCam(MakeSharedPtr<Camera>()){

        mPosition = position;
        setDirection(direction);
        mColor = color;
        mIntensity = intensity;
        mShadowMapResolution = shadowMapResolution;
        mAttenuationTexture = attenuationTex;
        
        this->setCastShadows(castShadows);
        this->updateCamera();
    }

    SpotLight::SpotLight():
    LightSource(LS_Spot) {

    }

    SpotLight::~SpotLight() {

    }

    float SpotLight::getFOV() const {
        return mFOV;
    }

    float SpotLight::lightAngleCos() {
        return cosf(mFOV);
    }

    void SpotLight::updateCamera() {
        float3 target = mPosition + mDirection;
        if(target.sqrLength() == 0) 
            target = -Vector3::Up();

        float3 up = math::cross(mDirection, Vector3::Up());
        if(up.sqrLength() == 0) 
            up = Vector3::Right();
        else
            up = Vector3::Up();

        mCam->setViewParams(mPosition, target, up);

        const GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        const CameraPtr& cp = gd.getCurrFrameBuffer()->getViewport().camera;

        float farPlane = 1000.f;
        float nearPlane = 0.f;
        if(cp) {
            farPlane = cp->getFarPlane();
            nearPlane = cp->getNearPlane();
        }

        mCam->setProjParams(mFOV, 1.0f, nearPlane, farPlane);
        float radial = float(tanf(mFOV / 2.0f) * 2 * farPlane);

        Matrix4 scaling = Matrix4::ScaleMat(radial, radial, farPlane);
        Matrix4 translation = Matrix4::TransMat(mPosition[0], mPosition[1], mPosition[2]);
        Matrix4 inverseView = mCam->getViewMatrix().inverted();
        Matrix4 semiProduct = scaling * inverseView;

        Vector3 scale, trans, rot;
        semiProduct.decompose(trans, rot, scale);

        mWorldMat = scaling * Matrix4::RotMat(rot.x(), rot.y(), rot.z()) * translation;
    }

    void DirectionalLight::setPosition(const float3& pos) {
        LightSource::setPosition(pos);
        this->updateCamera();
    }

    void DirectionalLight::setDirection(const float3& dir) {
        LightSource::setDirection(dir);
        this->updateCamera();
    }

    const CameraPtr& SpotLight::getCamera(uint32 index) const {
        return mCam;
    }

    void SpotLight::setCastShadows(bool flag) {
        LightSource::setCastShadows(flag);

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

    PointLight::PointLight():
    LightSource(LS_Point) {

    }

    PointLight::PointLight(const float3& position, float radius, 
                           const float4& color, float intensity,
                           bool castShadows, int shadowMapResolution):
    LightSource(LS_Point) {
        for(int i=0; i<6; ++i) {
            mCamera[i] = MakeSharedPtr<Camera>();
        }
        mPosition = position;
        mRadius = radius;
        mColor = color;
        mIntensity = intensity;
    
        mShadowMapResolution = shadowMapResolution;
        this->setCastShadows(castShadows);

        this->updateCamera();
    }

    PointLight::~PointLight() {

    }

    float PointLight::getRadius() const {
        return mRadius;
    }

    void PointLight::setRadius(float r) {
        mRadius = r;

        this->updateCamera();
    }

    void PointLight::setPosition(const float3& pos) {
        LightSource::setPosition(pos);

        this->updateCamera();
    }

    void PointLight::updateCamera() {
        // foreword, backword, left, right, down, up
        mCamera[0]->setViewParams(this->getPosition(),
                                  this->getPosition() + Vector3::Forward(),
                                  Vector3::Up());
        mCamera[1]->setViewParams(this->getPosition(),
                                  this->getPosition() + Vector3::Backward(),
                                  Vector3::Up());
        mCamera[2]->setViewParams(this->getPosition(),
                                  this->getPosition() + Vector3::Left(),
                                  Vector3::Up());
        mCamera[3]->setViewParams(this->getPosition(),
                                  this->getPosition() + Vector3::Right(),
                                  Vector3::Up());
        mCamera[4]->setViewParams(this->getPosition(),
                                  this->getPosition() + Vector3::Down(),
                                  Vector3::Forward());
        mCamera[5]->setViewParams(this->getPosition(),
                                  this->getPosition() + Vector3::Up(),
                                  Vector3::Backward());
        mWorldMat = Matrix4::ScaleMat(mRadius, mRadius, mRadius).translate(mPosition[0], mPosition[1], mPosition[2]);
        
        for(int i=0; i<6; ++i)
            mCamera[i]->setProjParams(pi_2,
                                      1.0f,
                                      1.0f,
                                      mRadius);

    }

    const CameraPtr& PointLight::getCamera(uint32 index) const {
        return mCamera[index];
    }

} // namespace ukn