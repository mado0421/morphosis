#include "Header.hlsli"

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	clip(cColor.a - 0.1f);

	return(cColor);
}

float4 PSTexturedIlluminated(VS_TEXTURED_ILLUMINATED_OUTPUT input) : SV_TARGET
{

	//float4 cColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
//	input.normalW = normalize(input.normalW);
	//	float4 cIllumination = Lighting(input.positionW, input.normalW);

	//	return (lerp(cColor, cIllumination, 0.9f));
	return (cColor);
}

float4 PSModel(VS_MODEL_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);

	return cColor;
}