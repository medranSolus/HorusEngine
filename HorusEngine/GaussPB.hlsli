cbuffer GaussBuffer : register(b0)
{
	// Must not exceed coefficients size
	int cb_radius;
	// Should be 6 * sigma - 1, current sigma for best effect 1.3 (but with reduced render target sigma can be 2.6)
	float cb_coefficients[8];
}