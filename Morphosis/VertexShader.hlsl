#include "Header.hlsli"

VS_TEXTURED_ILLUMINATED_VERTEX_OUTPUT VSAnimated(VS_ANIMATED_VERTEX_INPUT input)
{
	VS_TEXTURED_ILLUMINATED_VERTEX_OUTPUT output;
	float3 weightedPos = 0;
	//float4x4 a = {
	//	0,0,-1,0,
	//	1,0,0,0,
	//	0,-1,0,0,
	//	0,0,0,1
	//};
	float4x4 a = {
	1,0,0,0,
	0,0,-1,0,
	0,1,0,0,
	0,0,0,1
	};
	float4x4 b = {
	0,0,1,0,
	0,-1,0,0,
	1,0,0,0,
	0,0,0,1
	};
	float4x4 c = {
	0,1,0,0,
	0,0,1,0,
	1,0,0,0,
	0,0,0,1
	};
	//float4x4 tmpM;
	//float4 tmp = float4(input.position, 1);
	//tmp.x *= -1;
	//tmp = mul(tmp, a);



	for (int i = 0; i < 4; ++i) {

		//float4 bonePos = mul(float4(input.position, 1), mul(AnimMatrix[input.boneIdx[i]], b));
		float4 bonePos = mul(float4(input.position, 1), AnimMatrix[input.boneIdx[i]]);
		//float4 bonePos = float4(input.position, 1);
		//float4x4 tmpM = mul(AnimMatrix[input.boneIdx[i]], b);
		//float4 bonePos = mul ( tmp, a);
		////bonePos = mul(bonePos, tmpM);
		//bonePos = mul(bonePos, AnimMatrix[input.boneIdx[i]]);
		//bonePos = mul(bonePos, b);
		//tmpM = mul(AnimMatrix[input.boneIdx[i]], a);
		//tmpM = mul(tmpM, b);
		//float4 bonePos = mul(tmp, tmpM);
		weightedPos += input.weight[i] * bonePos.xyz;
	}
	weightedPos = mul(float4(weightedPos, 1.0f), a);
	//float tmp = weightedPos.y;
	//weightedPos.y = weightedPos.z;
	//weightedPos.z = tmp;
	//weightedPos.x *= -1;

	output.positionW = (float3)mul(float4(weightedPos * 2.0, 1.0f), gmtxGameObject);
	//output.positionW = (float3)mul(float4(output.positionW, 1.0f), a);


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