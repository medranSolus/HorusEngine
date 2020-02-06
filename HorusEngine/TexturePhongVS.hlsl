cbuffer TransformConstatBuffer
{
	matrix transformView;
	matrix scalingTransformView;
	matrix scalingTransformViewProjection;
};

struct VSOut
{
	float3 cameraPos : POSITION;
	float3 normal : NORMAL;
	float2 tc : TEXCOORD;
	float4 pos : SV_Position;
};

VSOut main(float3 pos : POSITION, float3 normal : NORMAL, float2 tc : TEXCOORD)
{
	VSOut vso;
	vso.cameraPos = (float3) mul(float4(pos, 1.0f), scalingTransformView);
	vso.normal = mul(normal, (float3x3) transformView);
	vso.tc = tc;
	vso.pos = mul(float4(pos, 1.0f), scalingTransformViewProjection);
	return vso;
}