#include "UKN/Light.h"


namespace ukn {

    DirectionalLight::DirectionalLight():
    mDir(float3(0, 0, 0)),
    mColor(float4(1, 1, 1, 1)),
    mIntensity(0.f) {

    }

    DirectionalLight::DirectionalLight(const float3& _dir, const float4& _color, float _intensity):
    mDir(_dir),
    mColor(_color),
    mIntensity(_intensity) {

    }

    DirectionalLight::DirectionalLight(const DirectionalLight& rhs):
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


} // namespace ukn