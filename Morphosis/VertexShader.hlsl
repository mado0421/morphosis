#include "Header.hlsli"

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input, uint nVertexID : SV_VertexID)
{
	VS_TEXTURED_OUTPUT output;

	output.position = float4(input.position, 1.0f);
	output.uv = input.uv;

	//if (nVertexID == 0)			{ output.position	= float4(	0.0f,	0.5f,	0.5f,	1.0f); output.uv = float2(0.5f, 0.0f); }
	//else if (nVertexID == 1)	{ output.position	= float4(	0.5f,	-0.5f,	0.5f,	1.0f); output.uv = float2(1.0f, 1.0f); }
	//else if (nVertexID == 2)	{ output.position	= float4(-	0.5f,	-0.5f,	0.5f,	1.0f); output.uv = float2(0.0f, 1.0f); }

	return(output);
}

VS_TEXTURED_ILLUMINATED_OUTPUT VSTexturedIlluminated(VS_TEXTURED_ILLUMINATED_INPUT input, uint nVertexID : SV_VertexID)
{
	VS_TEXTURED_ILLUMINATED_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	//if (nVertexID == 0) { output.position = float4(0.0f, 0.5f, 0.5f, 1.0f); output.uv = float2(0.5f, 0.0f); }
	//else if (nVertexID == 1) { output.position = float4(0.5f, -0.5f, 0.5f, 1.0f); output.uv = float2(1.0f, 1.0f); }
	//else if (nVertexID == 2) { output.position = float4(-0.5f, -0.5f, 0.5f, 1.0f); output.uv = float2(0.0f, 1.0f); }

	return output;
}

VS_MODEL_OUTPUT VSModel(VS_MODEL_INPUT input, uint nVertexID : SV_VertexID)
{
	VS_MODEL_OUTPUT output;

	float4 pos;
	sVertex v;
	sUV tuv;
	v = sVertexBuffer[input.vertexIdx];
	tuv = sUVBuffer[input.uvIdx];

	pos = float4(v.pos, 1.0f);
	output.positionW = (float3)mul(pos, gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = float2(tuv.u, tuv.v);
	output.normal = input.normal;
	output.binormal = input.binormal;
	output.tangent = input.tangent;

	return(output);
}

VS_DEBUG_OUTPUT VSDebug(VS_DEBUG_INPUT input)
{
	VS_DEBUG_OUTPUT output;

	float3 positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(positionW, 1.0f), gmtxView), gmtxProjection);

	return(output);
}

VS_UI_OUTPUT VSUI(VS_UI_INPUT input)
{
	VS_UI_OUTPUT output;

	output.position = float4(input.position, 1.0f);
	output.uv = input.uv;

	return(output);
}

VS_TEXTURED_ILLUMINATED_OUTPUT VSAnimated(VS_ANIMATED_INPUT input)
{
	VS_TEXTURED_ILLUMINATED_OUTPUT output;

	float3 weightedPos = 0;
	for (int i = 0; i < 4; ++i) {
		int idx = boneIdx[i];
		float4 bonePos = mul(BoneBuffer[idx], float4(input.position, 1));
		weightedPos += weight[i] * bonePos.xyz;
	}
	output.positionW = (float3)mul(float4(weightedPos, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return output;
}