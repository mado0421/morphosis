#include "Header.hlsli"
//
//float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
//{
//	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
//	return(cColor);
//}
//
//float4 PSTexturedIlluminated(VS_TEXTURED_ILLUMINATED_OUTPUT input) : SV_TARGET
//{
//	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
//	//clip(cColor.a - 0.1f);
//	//if (cColor.a < 0.1) discard; // 투명 넣기
//	return (cColor);
//}
//
//float4 PSModel(VS_MODEL_OUTPUT input) : SV_TARGET
//{
//	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
//
//	return cColor;
//}
//
//float4 PSDebug(VS_DEBUG_OUTPUT input) : SV_TARGET
//{
//	return float4(1.0f, 0.0f, 0.0f, 1.0f);
//}
//
//float4 PSUI(VS_UI_OUTPUT input) : SV_TARGET
//{
//	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
//	return(cColor);
//}

float4 PSTexturedIlluminated(VS_TEXTURED_ILLUMINATED_VERTEX_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	return (cColor);
}