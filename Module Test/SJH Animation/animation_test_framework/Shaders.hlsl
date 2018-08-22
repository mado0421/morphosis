cbuffer cbPlayerInfo : register(b0)
{
	matrix		gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
	float3		gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	uint		gnMaterial : packoffset(c4);
};

#include "Light.hlsl"

struct VS_DIFFUSED_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_DIFFUSED_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_DIFFUSED_OUTPUT VSDiffused(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSDiffused(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);

	return(cColor);
}

#define _WITH_VERTEX_LIGHTING

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif
	return(output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
	float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
	return(cIllumination);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	//	nointerpolation float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
	VS_TEXTURED_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;
#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif
	return(output);
}

float4 PSTexturedLighting(VS_TEXTURED_LIGHTING_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
#ifdef _WITH_VERTEX_LIGHTING
	float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
	return(lerp(cColor, cIllumination, 0.5f));
}

struct WeightInfo
{
	int		idx;
	float	weight;
};

struct sVertex
{
	float3	pos;
	WeightInfo weights[4];

};
struct sUV
{
	float u, v;
};
StructuredBuffer<sVertex>sVertexBuffer : register(t6);
//StructuredBuffer<sUV>sUVBuffer : register(t7);
StructuredBuffer<float2>sUVBuffer : register(t7);

struct VS_INTPUT
{
	int vertexIdx : VERTEXIDX;
	int uvIdx : UVIDX;
	float3 normal : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;
};
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;
};

#define MAXBONE	60

struct Bone
{
	matrix toParent;
	matrix offset;

	matrix m;
	int parentIdx;
	float3 padding;
		// 배열 형태로 선언시 오류 새로 다음 줄에 만듦
};

cbuffer cbBones:register(b8) {
	Bone bone[MAXBONE];
};

VS_OUTPUT VSData(VS_INTPUT input, uint nVertexID : SV_VertexID)
{
	VS_OUTPUT output;
	float4 pos;
	sVertex v;
	//sUV tuv;
	float2 tuv;
	v = sVertexBuffer[input.vertexIdx];
	tuv = sUVBuffer[input.uvIdx];
	/*matrix iden = float4x4(float4(0.5, 0, 0, 0), float4(0, 0.5, 0, 0),
		float4(0, 0, 0.5, 0), float4(0, 0, 0, 1));*/
	/*matrix toRoot = float4x4(float4(1, 0, 0, 0), float4(0, 1, 0, 0),
		float4(0, 0, 1, 0), float4(0, 0, 0, 1));
	matrix offset = float4x4(float4(1, 0, 0, 0), float4(0, 1, 0, 0),
		float4(0, 0, 1, 0), float4(0, 0, 0, 1));*/



	pos = float4(v.pos, 1.0f);
	
	float4 res_pos = float4(0, 0, 0, 0);
	for (int i = 0; i < 4; ++i)
	{
		if (v.weights[i].weight > 0)
		{
			float weight = v.weights[i].weight;
			Bone b = bone[v.weights[i].idx];

			res_pos += weight * mul(mul(pos, b.offset), b.m);

			//res_pos += weight * mul(pos, b.offset);

			//res_pos += weight * mul(mul(b.offset, pos), b.m);

			//res_pos += weight * pos;
			//res_pos += weight * mul(pos,b.offset);
		}
	}
	pos = res_pos;
	
	output.positionW = (float3)mul(pos, gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

	//output.uv = float2(tuv.u, tuv.v);
	output.uv = tuv;

	//	뒤집힐때~~
	output.uv.y = 1 - output.uv.y;

	output.normal = input.normal;
	output.binormal = input.binormal;
	output.tangent = input.tangent;

	return(output);
}

float4 PSData(VS_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
	float4 cIllumination = Lighting(input.positionW, input.normal);
	
	return(lerp(cColor, cIllumination, 0.1f));
}


//struct mvertex
//{
//	float4 x;
//	float4 u;
//	float4 v;
//};
//
//StructuredBuffer<mvertex>vertics :register(t10);
//
//
//float4 PS1Player(VS_DIFFUSED_OUTPUT input) : SV_TARGET
//{
//	vertics[10].x;
//	return(input.color);
//}

//struct tOutput
//{
//	float4 position : SV_POSITION;
//	float4 color : COLOR;
//
//};
//
//struct tInput
//{
//	int input : VERTEXIDX;
//	int uvidx : UVIDX;
//	float3 normal : NORMAL;
//	float3 binoraml : BINORMAL;
//};
//tOutput tVSData(tInput input)
//{
//	tOutput output;
//	output.position = float4(0, 0, 0, 1);
//	output.color = float4(0, 0, 0, 1);
//	return(output);
//}
//
//float4 tPSData(tOutput input) : SV_TARGET
//{
//	//float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);
//	float4 cColor = float4(0,0,0,1);
//
//	return cColor;
//}