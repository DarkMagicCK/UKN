#include "UKN/CgHelper.h"
#include "UKN/GraphicFactory.h"
#include "UKN/Shader.h"
#include "UKN/Context.h"

#include "mist/Logger.h"

namespace ukn {

    namespace {
        static const char* _2d_vert_program = "const uniform float4x4 viewMatrix;\
                                               const uniform float4x4 projectionMatrix;\
                                               const uniform float4x4 worldMatrix;\
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
                                                output.position = mul(output.position, worldMatrix);\
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

    EffectPtr CreateCgEffet2D() {
        EffectPtr effect = ukn::Context::Instance().getGraphicFactory().createEffect();
        effect->setFragmentShader(effect->createShader(
            ukn::Resource::MakeResourcePtr(new ukn::MemoryStream((const uint8*)_2d_frag_program, 
                                                                 strlen(_2d_frag_program)), 
                                                                 L""),
                                           ukn::ShaderDesc(ST_FragmentShader,
                                           "FragmentProgram")));
        effect->setVertexShader(effect->createShader(
            ukn::Resource::MakeResourcePtr(new ukn::MemoryStream((const uint8*)_2d_vert_program, 
                                                                 strlen(_2d_vert_program)), 
                                                                 L""),
                                           ukn::ShaderDesc(ST_VertexShader,
                                           "VertexProgram")));
        effect->setVertexFormat(ukn::Vertex2D::Format());
        return effect;
    }


}