#include "LightUtilsPS.hlsli"
#include "SamplersPS.hlsli"
#include "PointLightPB.hlsli"
#include "HDRGammaPB.hlsli"
#include "CameraPB.hlsli"
#include "BiasPB.hlsli"

Texture2D colorTex    : register(t4); // RGB - color, A = 0.0f: solid; 0.5f: light source; 1.0f: normal
Texture2D normalTex   : register(t5); // RG - normal
Texture2D specularTex : register(t6); // RGB - color, A - power

TextureCube shadowMap : register(t7);
Texture2D depthMap    : register(t8);

struct PSOut
{
	float4 color : SV_TARGET0;
	float4 specular : SV_TARGET1;
};

PSOut main(float3 texPos : TEX_POSITION)
{
	PSOut pso;

	const float2 tc = float2(0.5f, -0.5f) * (texPos.xy / texPos.z) + 0.5f;
	const float3 position = GetWorldPosition(tc, depthMap.Sample(splr_PW, tc).x, cb_inverseViewProjection);

	float3 directionToLight = cb_lightPos - position;
	const float lightDistance = length(directionToLight);
	const float3 lightColor = DeleteGammaCorr(cb_lightColor) * (cb_lightIntensity / GetAttenuation(cb_atteuationLinear, cb_attenuationQuad, lightDistance));
	directionToLight /= lightDistance;

	const float isSolid = colorTex.Sample(splr_PW, tc).a;
	[branch]
	if (isSolid == 0.0f)
	{
		const float3 shadowColor = DeleteGammaCorr(cb_shadowColor);

		// Shadow test (cb_mapSize from BiasPB is bound implicitly from Shadow Mapping Pass since it has to be run always before Lighting Pass)
		const float shadowLevel = GetShadowLevel(normalize(cb_cameraPos - position), lightDistance, directionToLight, splr_AW, shadowMap, cb_mapSize);
		if (shadowLevel != 0.0f)
		{
			const float3 normal = DecodeNormal(normalTex.Sample(splr_PW, tc).rg);
			pso.color = float4(lerp(shadowColor, GetDiffuse(lightColor, directionToLight, normal), shadowLevel), 0.0f);

			if (shadowLevel > 0.98f)
			{
				const float4 specularData = specularTex.Sample(splr_PW, tc);
				pso.specular = float4(GetSpecular(cb_cameraPos, directionToLight, position, normal,
					pso.color.rgb * specularData.rgb, specularData.a), 0.0f);
			}
			else
				pso.specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			pso.color = float4(-shadowColor, 0.0f);
			pso.specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
		}
	}
	else
	{
		pso.color = float4(lightColor, 0.0f);
		pso.specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	return pso;
}