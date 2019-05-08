#include "Header.hlsli"

VS_TEXTURED_ILLUMINATED_VERTEX_OUTPUT VSAnimated(VS_ANIMATED_VERTEX_INPUT input)
{
	VS_TEXTURED_ILLUMINATED_VERTEX_OUTPUT output;
	float3 weightedPos = 0;
	float4x4 a = {
	1,0,0,0,
	0,0,-1,0,
	0,1,0,0,
	0,0,0,1
	};

	for (int i = 0; i < 4; ++i) {
		//float4 bonePos = mul(float4(input.position, 1), mul(AnimMatrix[input.boneIdx[i]], b));
		float4 bonePos = mul(float4(input.position, 1), AnimMatrix[input.boneIdx[i]]);
		weightedPos += input.weight[i] * bonePos.xyz;
	}
	weightedPos = mul(float4(weightedPos, 1.0f), a);


	output.positionW = (float3)mul(float4(weightedPos * 2.0, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return output;
}

ILLUM_TEX_OUTPUT VSDefaultShader(ILLUM_TEX_INPUT input)
{
	ILLUM_TEX_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return output;
}