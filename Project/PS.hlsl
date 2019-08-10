#include "HLSLHeader.hlsli"
#include "Lighting.hlsli"

float4 PSAnimModel(VS_ANIM_OUTPUT input) : SV_TARGET{
	input.normalW = normalize(input.normalW);

	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	float4 cLightResult = TestLighting(input.positionW, input.normalW);
	return (lerp(cColor, cLightResult, 0.5f));
}

float4 PSModel(VS_MODEL_OUTPUT input) : SV_TARGET{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	float4 cLightResult = TestLighting(input.positionW, input.normalW);
	return (lerp(cColor, cLightResult, 0.5f));
}

float4 PSDefaultUI(VS_UI_OUTPUT input) : SV_TARGET{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	if (cColor.a < 0.9) discard;
	return cColor;
}
