#include "Header.hlsli"
#include "Light.hlsli"

ANIM_ILLUM_TEX_OUTPUT VSAnimated(ANIM_ILLUM_TEX_INPUT input)
{
	ANIM_ILLUM_TEX_OUTPUT output;

	float weights[4]	= { input.weight[0], input.weight[1], input.weight[2], input.weight[3] };
	float3 posL			= 0;
	float3 normalL		= 0;
	float3 tangentL		= 0;

	for (int i = 0; i < 4; ++i) {
		posL		+= weights[i] * mul(float4(input.position, 1.0f), AnimMatrix[input.boneIdx[i]]).xyz;
		normalL		+= weights[i] * mul(input.normal, (float3x3)AnimMatrix[input.boneIdx[i]]);
		tangentL	+= weights[i] * mul(input.tangent, (float3x3)AnimMatrix[input.boneIdx[i]]);
	}
	posL.x *= -1;
	normalL.x *= -1;
	tangentL.x *= -1;

	output.positionW	= (float3)mul(float4(posL, 1.0f), gmtxGameObject);
	output.normalW		= mul(normalL, (float3x3)gmtxGameObject);
	output.tangentW		= mul(tangentL, (float3x3)gmtxGameObject);
	output.position		= mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv			= input.uv;

	return output;
}

ILLUM_TEX_OUTPUT VSDefaultShader(ILLUM_TEX_INPUT input)
{
	ILLUM_TEX_OUTPUT output;

	output.normalW		= normalize(mul(normalize(input.normal), (float3x3)gmtxGameObject));
	output.positionW	= (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position		= mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv			= input.uv;

	return output;
}

VS_DEBUG_OUTPUT VSDebug(VS_DEBUG_INPUT input)
{
	VS_DEBUG_OUTPUT output;

	float3 positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);

	return(output);
}