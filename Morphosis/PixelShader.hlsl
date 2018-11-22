#include "Header.hlsli"


float4 PSAnimated(VS_TEXTURED_ILLUMINATED_VERTEX_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	return (cColor);
}

float4 PSDefaultShader(ILLUM_TEX_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	//clip(cColor.a - 0.1f);
	//if (cColor.a < 0.1) discard; // ���� �ֱ�
	return (cColor);
}