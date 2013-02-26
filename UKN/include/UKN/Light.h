#ifndef UKN_LIGHT_H_
#define UKN_LIGHT_H_

#include "UKN/PreDeclare.h"

#include "mist/MathUtil.h"

namespace ukn {

    class UKN_API DirectionalLight {
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

} // namespace ukn


#endif // UKN_LIGHT_H_