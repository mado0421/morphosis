SamplerState gSamplerState					: register(s0);
cbuffer cbCameraInfo						: register(b0)
{
	matrix		gmtxView			: packoffset(c0);
	matrix		gmtxProjection		: packoffset(c4);
	float3		gvCameraPosition	: packoffset(c8);
};
cbuffer cbGameObjectInfo					: register(b1)
{
	matrix		gmtxGameObject		: packoffset(c0);
	uint		gnMaterial			: packoffset(c4);
};
Texture2D gtxtTexture						: register(t2);
cbuffer cbAnimMatrixInfo					: register(b3)
{
	matrix		AnimMatrix[64]		: packoffset(c0);
};
//struct Matrix
//{
//	matrix AnimMatrix[20];
//};
//
//StructuredBuffer<Matrix>AnimMatrixBuffer	: register(t3);

struct VS_ANIMATED_VERTEX_INPUT
{
	float3	position	: POSITION;
	float2	uv			: TEXCOORD0;
	float4	weight		: BLENDWEIGHT0;
	int4	boneIdx		: BLENDINDICES0;
};

struct VS_TEXTURED_ILLUMINATED_VERTEX_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float2 uv : TEXCOORD;
	matrix test0 : ASDASD;
	matrix test1 : AS;
	matrix test2 : ASDAS;
};

//StructuredBuffer<sVertex>sVertexBuffer : register(t5);
//StructuredBuffer<sUV>sUVBuffer : register(t6);
//cbuffer cbCameraInfo : register(b0)
//{
//	matrix		gmtxView : packoffset(c0);
//	matrix		gmtxProjection : packoffset(c4);
//	float3		gvCameraPosition : packoffset(c8);
//};
//
//cbuffer cbGameObjectInfo : register(b1)
//{
//	matrix		gmtxGameObject : packoffset(c0);
//	uint		gnMaterial : packoffset(c4);
//};
//
//cbuffer cbGameUIInfo : register(b7)
//{
//	matrix		gmtxGameUI : packoffset(c0);
//	float4		gf4Rect : packoffset(c4);
//	uint		gnUIMaterial : packoffset(c8);
//};
//
//struct WeightInfo
//{
//	int		idx;
//	float	weight;
//};
//
//struct sVertex
//{
//	float3	pos;
//	WeightInfo weights[4];
//
//};
//struct sUV
//{
//	float u, v;
//};
//
//Texture2D gtxtTexture : register(t4);
//SamplerState gSamplerState : register(s0);
//StructuredBuffer<sVertex>sVertexBuffer : register(t5);
//StructuredBuffer<sUV>sUVBuffer : register(t6);
//
//#define EPSILON	1.0e-10f
//bool isAlmostSame(float a, float b)
//{
//	if (a - b > EPSILON) return false;
//	if (b - a > EPSILON) return false;
//	return true;
//}
//
//struct VS_TEXTURED_INPUT
//{
//	float3 position : POSITION;
//	float2 uv : TEXCOORD;
//};
//struct VS_TEXTURED_OUTPUT
//{
//	float4 position : SV_POSITION;
//	float2 uv : TEXCOORD;
//};
//
//struct VS_TEXTURED_ILLUMINATED_INPUT
//{
//	float3 position : POSITION;
//	float3 normal : NORMAL;
//	float2 uv : TEXCOORD;
//};
//
//struct VS_TEXTURED_ILLUMINATED_OUTPUT
//{
//	float4 position : SV_POSITION;
//	float3 positionW : POSITION;
//	float3 normalW : NORMAL;
//	float2 uv : TEXCOORD;
//};
//
//struct VS_MODEL_INPUT
//{
//	int vertexIdx : VERTEXIDX;
//	int uvIdx : UVIDX;
//	float3 normal : NORMAL;
//	float3 binormal : BINORMAL;
//	float3 tangent : TANGENT;
//};
//struct VS_MODEL_OUTPUT
//{
//	float4 position : SV_POSITION;
//	float3 positionW : POSITION;
//	float2 uv : TEXCOORD;
//	float3 normal : NORMAL;
//	float3 binormal : BINORMAL;
//	float3 tangent : TANGENT;
//};
//
//struct VS_DEBUG_INPUT
//{
//	float3 position : POSITION;
//};
//struct VS_DEBUG_OUTPUT
//{
//	float4 position : SV_POSITION;
//};
//
//struct VS_UI_INPUT
//{
//	float3 position : POSITION;
//	float2 uv : TEXCOORD;
//};
//struct VS_UI_OUTPUT
//{
//	float4 position : SV_POSITION;
//	float2 uv : TEXCOORD;
//};
//
//
//
//struct BoneInfo
//{
//	matrix animMatrix;
//};
//StructuredBuffer<BoneInfo>BoneBuffer : register(t8);
//struct VS_ANIMATED_INPUT
//{
//	float3 position : POSITION;
//	float2 uv : TEXCOORD0;
//	float4 weight : TEXCOORD1;
//	int4 boneIdx : TEXCOORD2;
//};
//
//
