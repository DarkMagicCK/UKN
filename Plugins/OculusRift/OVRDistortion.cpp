#include "OVRDistortion.h"

#include "UKN/Context.h"

namespace ukn {

    namespace {
        static const char* _distortion_vert_program = "\
        struct VertexOutputType {\
            float4 position: POSITION;\
            float2 texCoord: TEXCOORD0;\
        };\
        const uniform float4x4 viewMat; \
        const uniform float4x4 texMat; \
        VertexOutputType VertexProgram(in float2 texCoord: TEXCOORD0, \
                                       in float3 position: POSITION,\
                                       in float4 color: COLOR) {\
            VertexOutputType output;\
            output.position = mul(viewMat, float4(position, 1));\
            output.texCoord = mul(texMat, float4(texCoord, 0, 1));\
            return output;\
        }\0";

        static const char* _distortion_frag_program = "\
        struct VertexOutputType {\
            float4 position: POSITION;\
            float2 texCoord: TEXCOORD0;\
        };\
        float2 lensCenter; \
        float2 screenCenter; \
        float2 scale; \
        float2 scaleIn; \
        float4 HmdWrapParam; \
        \
        float2 HmdWrap(float2 in01) { \
            float2 theta = (in01 - lensCenter) * scaleIn; \
            float rSq = theta.x * theta.x + theta.y * theta.y; \
            float2 rvector = theta * (HmdWrapParam.x + HmdWrapParam.y * rSq + \
                                      HmdWrapParam.z * rSq * rSq + \
                                      HmdWrapParam.w * rSq * rSq * rSq); \
            return lensCenter + scale * rvector; \
        }\
        \
        float4 FragmentProgram(VertexOutputType input, \
                               uniform sampler2D tex: TEX): COLOR {\
            float2 tc = HmdWrap(input.texCoord); \
            if (any(clamp(tc, screenCenter - float2(0.25, 0.5), \
                              screenCenter + float2(0.25, 0.5)) - tc)) \
                return 0; \
            return tex2D(tex, tc); \
        }\0";
    }

    EffectPtr GetOVRDistortionEffect() {
        static EffectPtr effect;
        if(!effect) {
            effect = ukn::Context::Instance().getGraphicFactory().createEffect();
            EffectTechniquePtr technique = effect->appendTechnique();
            EffectPassPtr pass0 = technique->appendPass();
            pass0->setFragmentShader(effect->createShader(
                ukn::Resource::MakeResourcePtr(MakeSharedPtr<ukn::MemoryStream>((const uint8*)_distortion_frag_program, 
                                                                                strlen(_distortion_frag_program)), 
                                                                                L""),
                                                ukn::ShaderDesc(ST_FragmentShader,
                                                "FragmentProgram")));
            pass0->setVertexShader(effect->createShader(
                ukn::Resource::MakeResourcePtr(MakeSharedPtr<ukn::MemoryStream>((const uint8*)_distortion_vert_program, 
                                                                                strlen(_distortion_vert_program)), 
                                                                                L""),
                                                ukn::ShaderDesc(ST_VertexShader,
                                                "VertexProgram")));
        }
        return effect;
    }

}