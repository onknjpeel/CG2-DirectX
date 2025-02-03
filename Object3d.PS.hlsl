#include "Object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

struct Camera
{
    float32_t3 worldPosition;
};

struct PointLight
{
    float32_t4 color;
    float32_t3 position;
    float intensity;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    

    float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
    float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
    
    float NDotH = dot(normalize(input.normal), halfVector);
    float specularPow = pow(saturate(NDotH), gMaterial.shininess);
    
    //float RdotE = dot(reflectLight, toEye);
    //float specularPow = pow(saturate(RdotE), gMaterial.shininess);
    
    float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
    
    float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
    
    float32_t3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
    

    float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
    
    float32_t3 reflectLightPosLight = reflect(pointLightDirection, normalize(input.normal));
    float32_t3 halfVectorPosLight = normalize(-pointLightDirection + toEye);
    
    float cosPosLight = saturate(dot(normalize(input.normal), -pointLightDirection));
    
    float32_t3 diffusePosLight = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity;
    
    float32_t3 specularPosLight = gPointLight.color.rgb * gPointLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
    
    float NDotHPosLight = dot(normalize(input.normal), halfVectorPosLight);
    float specularPowPosLight = pow(saturate(NDotHPosLight), gMaterial.shininess);
    
    PixelShaderOutput output;
    
    output.color.rgb = diffuse + specularPow + diffusePosLight + specularPowPosLight;
    
    output.color.a = gMaterial.color.a * textureColor.a;

    return output;
}