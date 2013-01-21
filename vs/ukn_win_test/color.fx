matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;

Texture2D tex;

float4 diffuseColor;
float4 ambientColor;
float3 lightDirection;

float3 cameraPosition;
float4 specularColor;
float specularPower = 16.f;

SamplerState SampleType {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VertexInputType {
	float4 position: POSITION;
    float3 normal: NORMAL;
	float2 texCoord: TEXCOORD;
};

struct PixelInputType {
	float4 position: SV_POSITION;
	float3 normal: NORMAL;
	float2 texCoord: TEXCOORD;
    float3 viewDirection: TEXCOORD1;
};

PixelInputType MyVertexShader(VertexInputType input) {
	PixelInputType output;
    float4 worldPosition;

	input.position.w = 1.0f;
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
   	output.position = mul(output.position, projectionMatrix);

	output.texCoord = input.texCoord;
	output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    worldPosition = mul(input.position, worldMatrix);
    output.viewDirection = normalize(cameraPosition.xyz - worldPosition.xyz);
   
	return output;
}

float4 MyPixelShader(PixelInputType input): SV_Target {
    float4 texColor;
    float3 nlightDir;
    float lightIntensity;
    float4 color;
    float3 reflection;
    float4 specular;
  
    texColor = tex.Sample(SampleType, input.texCoord);
    texColor = float4(1.f, 0.f, 0.f, 1.f);
    color = ambientColor;
    specular = float4(0.f, 0.f, 0.f, 0.f);

    nlightDir = -lightDirection;
    lightIntensity = saturate(dot(input.normal, nlightDir));
    if(lightIntensity > 0.0f) {
        color += (diffuseColor * lightIntensity);

        color = saturate(color);
        reflection = normalize(2 * lightIntensity * input.normal - nlightDir);
        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
    }
    color = color * texColor;
    color = saturate(color + specular);

    return color;
}

technique10 TextureTechnique {
	pass pass0 {
		SetVertexShader(CompileShader(vs_4_0, MyVertexShader()));
		SetPixelShader(CompileShader(ps_4_0, MyPixelShader()));
		SetGeometryShader(NULL);
	}
}