#include "Header.hlsli"

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = float4(input.position, 1.0f);
	output.uv = input.uv;

	return(output);
}

//VS_ILLUMINATED_OUTPUT VSIlluminated(VS_ILLUMINATED_INPUT input)
//{
//	VS_ILLUMINATED_OUTPUT output;
//
//	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
//	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
//	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
//
//	return(output);
//}

VS_TEXTURED_ILLUMINATED_OUTPUT VSTexturedIlluminated(VS_TEXTURED_ILLUMINATED_INPUT input, uint nVertexID : SV_VertexID)
{
	VS_TEXTURED_ILLUMINATED_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return output;
}
