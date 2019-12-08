cbuffer TransformConstatBuffer
{
    matrix modelView;
    matrix modelViewProjection;
};

struct VSOut
{
    float4 col : Color;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float4 color : Color)
{
	VSOut vout;
	vout.col = color;
    vout.pos = mul(modelViewProjection, float4(pos, 1.0f));
	return vout;
}