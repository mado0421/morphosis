#include "Header.hlsli"
#include "Light.hlsli"

ANIM_ILLUM_TEX_OUTPUT VSAnimated(ANIM_ILLUM_TEX_INPUT input)
{
	ANIM_ILLUM_TEX_OUTPUT output;

	float3 weightedPos	= 0;
	float3 normalDir	= 0;
	float3 tangentDir	= 0;

	for (int i = 0; i < 4; ++i) {
		float4 bonePos		= mul(float4(input.position, 1),	AnimMatrix[input.boneIdx[i]]);
		float4 boneNormal	= mul(float4(input.normal, 1),		AnimMatrix[input.boneIdx[i]]);
		float4 boneTangent	= mul(float4(input.tangent, 1),		AnimMatrix[input.boneIdx[i]]);
		weightedPos += input.weight[i] * bonePos.xyz;
		normalDir	+= input.weight[i] * boneNormal.xyz;
		tangentDir	+= input.weight[i] * boneTangent.xyz;
	}

	output.normal		= input.normal;
	output.normalW		= (float3)mul(float4(normalDir, 1.0f),		gmtxGameObject);
	output.tangentW		= (float3)mul(float4(tangentDir, 1.0f),		gmtxGameObject);
	output.positionW	= (float3)mul(float4(weightedPos, 1.0f),	gmtxGameObject);
	output.position		= mul(mul(float4(output.positionW, 1.0f),	gmtxView), gmtxProjection);
	output.uv			= input.uv;

	return output;
}

ILLUM_TEX_OUTPUT VSDefaultShader(ILLUM_TEX_INPUT input)
{
	ILLUM_TEX_OUTPUT output;

	input.position.x *= -1;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return output;
}

VS_DEBUG_OUTPUT VSDebug(VS_DEBUG_INPUT input)
{
	VS_DEBUG_OUTPUT output;

	float3 positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);

	return(output);
}