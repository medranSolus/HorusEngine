cbuffer LightConstantBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightPos;
    float diffuseIntensity;
    float atteuationConst;
    float atteuationLinear;
    float attenuationQuad;
}

Texture2D tex;
Texture2D spec;
SamplerState splr;

float4 main(float3 cameraPos : POSITION, float3 normal : NORMAL, float2 tc : TEXCOORD) : SV_Target
{
	// Vertex to light data
    const float3 vertexToLight = lightPos - cameraPos;
    const float distanceToLight = length(vertexToLight);
    const float3 directionToLight = vertexToLight / distanceToLight;
    
	// Diffuse attenuation
    // http://wiki.ogre3d.org/-Point+Light+Attenuation
    float attenuation = atteuationConst + atteuationLinear * distanceToLight + attenuationQuad * (distanceToLight * distanceToLight);
	
    const float4 diffuseColorScaled = diffuseColor * diffuseIntensity / attenuation;
	// Diffuse intensity
    const float4 diffuse = diffuseColorScaled * max(0.0f, dot(directionToLight, normal));
    
    // Specular intensity based on angle between viewing vector and reflection vector
    const float3 reflection = normal * dot(vertexToLight, normal) * 2.0f - vertexToLight;
    const float4 specularTex = spec.Sample(splr, tc);
    
    // https://gamedev.stackexchange.com/questions/74879/specular-map-what-about-the-specular-reflections-highlight-size
    const float specularPower = pow(2.0f, specularTex.a * 13.0f);
    const float4 specular = diffuseColorScaled * pow(max(0.0f, dot(normalize(-reflection), normalize(cameraPos))), specularPower);
    
    return saturate((diffuse + ambientColor) * tex.Sample(splr, tc).bgra + specular * specularTex.rgba);
}