#include "UtilsPS.hlsli"

cbuffer PixelBuffer : register(b1)
{
	float3 cb_specularColor;
	float cb_specularIntensity; // The bigger the brighter
	float cb_specularPower;     // The smaller the less focused in one point
#ifdef _TEX
#ifdef _TEX_NORMAL
	float cb_normalMapWeight;
#endif
#ifdef _TEX_SPEC
	bool cb_useSpecularPowerAlpha;
#endif
#else
	float4 cb_materialColor;
#endif
};

#ifdef _TEX
SamplerState splr : register(s0);
Texture2D tex : register(t0);
#ifdef _TEX_NORMAL
Texture2D normalMap : register(t1);
#endif
#ifdef _TEX_SPEC
Texture2D spec : register(t2);
#endif
#endif

struct PSOut
{
	float4 color : SV_TARGET0;
	float4 specular : SV_TARGET1; // RGB - color, A = 1.0f
	float4 position : SV_TARGET2; // RGB - position, A - specular intensity
	float4 normal : SV_TARGET3;   // RGB - normal, A - power
};

PSOut main(float3 viewPos : POSITION, float3 viewNormal : NORMAL//, float3 shadowPos : SHADOW_POSITION
#ifdef _TEX
	, float2 tc : TEXCOORD
#ifdef _TEX_NORMAL
	, float3 viewTan : TANGENT,
	float3 viewBitan : BITANGENT
#endif
#endif
)
{
	PSOut pso;
#ifdef _TEX
	pso.color = tex.Sample(splr, tc);
#else
	pso.color = cb_materialColor;
#endif
	clip(pso.color.a - 0.0039f);
	pso.position = float4(viewPos, cb_specularIntensity);

#ifdef _TEX_NORMAL
	pso.normal.rgb = lerp(viewNormal,
		GetMappedNormal(viewTan, viewBitan, viewNormal, tc, normalMap, splr), cb_normalMapWeight);
#else
	pso.normal.rgb = normalize(viewNormal);
#endif
	// Only for double sided objects
	//if (dot(viewNormal, viewPos) >= 0.0f)
	//	viewNormal *= -1.0f;

#ifdef _TEX_SPEC
	const float4 specularTex = spec.Sample(splr, tc);
	pso.specular = float4(specularTex.rgb, 1.0f);
	pso.normal.a = cb_useSpecularPowerAlpha ? GetSampledSpecularPower(specularTex) : cb_specularPower;
#else
	pso.specular = float4(cb_specularColor, 1.0f);
	pso.normal.a = cb_specularPower;
#endif
	return pso;
}