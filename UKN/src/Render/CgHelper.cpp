#include "UKN/CgHelper.h"
#include "UKN/GraphicFactory.h"
#include "UKN/Shader.h"
#include "UKN/Context.h"

#include "mist/Logger.h"

namespace ukn {

    namespace {
        static const char* _2d_vert_program = "const uniform float4x4 viewMatrix;\
                                               const uniform float4x4 projectionMatrix;\
                                               struct VertexOutputType {\
                                               float4 position: POSITION;\
                                               float2 texCoord: TEXCOORD0;\
                                               float4 color: COLOR;\
                                               };\
                                               VertexOutputType VertexProgram(in float2 texCoord: TEXCOORD0,\
                                                                              in float3 position: POSITION,\
                                                                              in float4 color: COLOR) {\
                                                VertexOutputType output;\
                                                output.position = float4(position, 1);\
                                                output.position = mul(output.position, viewMatrix);\
                                                output.position = mul(output.position, projectionMatrix);\
                                                output.texCoord = texCoord;\
                                                output.color = color;\
                                                return output;\
                                               }\0";

        static const char* _2d_frag_program = "struct VertexOutputType {\
                                               float4 position: POSITION;\
                                               float2 texCoord: TEXCOORD0;\
                                               float4 color: COLOR;\
                                               };\
                                               float4 FragmentProgram(VertexOutputType input,\
                                                uniform sampler2D tex: TEX): COLOR {\
                                                float4 texColor = tex2D(tex, input.texCoord);\
                                                return texColor * input.color;\
                                               }\0";

    }

    EffectPtr GetCgEffet2D() {
        static EffectPtr effect = ukn::Context::Instance().getGraphicFactory().createEffect();
        EffectTechniquePtr technique = effect->appendTechnique();
        EffectPassPtr pass0 = technique->appendPass();
        pass0->setFragmentShader(effect->createShader(
            ukn::Resource::MakeResourcePtr(MakeSharedPtr<ukn::MemoryStream>((const uint8*)_2d_frag_program, 
                                                                 strlen(_2d_frag_program)), 
                                                                 L""),
                                           ukn::ShaderDesc(ST_FragmentShader,
                                           "FragmentProgram")));
        pass0->setVertexShader(effect->createShader(
            ukn::Resource::MakeResourcePtr(MakeSharedPtr<ukn::MemoryStream>((const uint8*)_2d_vert_program, 
                                                                 strlen(_2d_vert_program)), 
                                                                 L""),
                                           ukn::ShaderDesc(ST_VertexShader,
                                           "VertexProgram")));
        return effect;
    }


    namespace {
        static const char* _pass_vert_program = "\
                                               struct VertexOutputType {\
                                               float4 position: POSITION;\
                                               float4 color: COLOR;\
                                               };\
                                               VertexOutputType VertexProgram(in float3 position: POSITION,\
                                                                              in float4 color: COLOR) {\
                                                VertexOutputType output;\
                                                output.position = float4(position, 1);\
                                                output.texCoord = texCoord;\
                                                output.color = color;\
                                                return output;\
                                               }\0";

        static const char* _pass_frag_program = "struct VertexOutputType {\
                                               float4 position: POSITION;\
                                               float4 color: COLOR;\
                                               };\
                                               float4 FragmentProgram(VertexOutputType input): COLOR {\
                                                return input.color;\
                                               }\0";
    }

    EffectPtr GetCgEffetPass() {
        static EffectPtr effect = ukn::Context::Instance().getGraphicFactory().createEffect();
        EffectTechniquePtr technique = effect->appendTechnique();
        EffectPassPtr pass0 = technique->appendPass();
        pass0->setFragmentShader(effect->createShader(
            ukn::Resource::MakeResourcePtr(MakeSharedPtr<ukn::MemoryStream>((const uint8*)_pass_frag_program, 
                                                                 strlen(_pass_frag_program)), 
                                                                 L""),
                                           ukn::ShaderDesc(ST_FragmentShader,
                                           "FragmentProgram")));
        pass0->setVertexShader(effect->createShader(
            ukn::Resource::MakeResourcePtr(MakeSharedPtr<ukn::MemoryStream>((const uint8*)_pass_vert_program, 
                                                                 strlen(_pass_vert_program)), 
                                                                 L""),
                                           ukn::ShaderDesc(ST_VertexShader,
                                           "VertexProgram")));
        return effect;
    }


}