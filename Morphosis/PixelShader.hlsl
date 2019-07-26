#include "Header.hlsli"
#include "Light.hlsli"

float4 PSAnimated(ANIM_ILLUM_TEX_OUTPUT input) : SV_TARGET
{
	float3 N = normalize(input.normalW);
	float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);


	float3 normal		= mul(input.normalW, TBN);
	//float4 cColor		= gtxtTexture.Sample(gSamplerState, input.uv);
	//float4 cLightResult = TestLighting(input.positionW, normal);
	////return (lerp(cColor, cLightResult, 0.5f));

	//return cLightResult;


	float3 vLightDirection	= normalize(float3(1.0f, -1.0f, 0.0f));
	float3 vToLight			= -vLightDirection;
	float f = dot(vToLight, normal);
	return float4(f, f, f, 1.0f);


}

float4 PSDefaultShader(ILLUM_TEX_OUTPUT input) : SV_TARGET
{
	
	//float3 vLightDirection	= normalize(float3(1.0f, -1.0f, 0.0f));
	//float3 vToLight			= -vLightDirection;
	//float f = dot(vToLight, input.normalW);
	//return float4(f, f, f, 1.0f);
	//int i = step(f, 0.7f);


	//return float4(i, i, i, 1.0f);

	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	float4 cLightResult = TestLighting(input.positionW, input.normalW);
	return (lerp(cColor, cLightResult, 1.0f));

	//return cLightResult;

	//float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	//clip(cColor.a - 0.1f);
	//if (cColor.a < 0.1) discard; // 투명 넣기
	//return float4(1.0f, 0.0f, 0.0f, 0.0f);
	//return (cColor);
}

float4 PSDebug(VS_DEBUG_OUTPUT input) : SV_TARGET
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}