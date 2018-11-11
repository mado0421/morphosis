#define NUM_BONES 128

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
cbuffer cbAnimationInfo						: register(b3)
{
	matrix		gmtxInterpolatedMatrix[NUM_BONES]: packoffset(c0);
};

//struct Matrix
//{
//	matrix AnimMatrix;
//};
//StructuredBuffer<Matrix>AnimMatrixBuffer	: register(t3);

struct VS_ANIMATED_VERTEX_INPUT
{
	float3	position	: POSITION;
	float2	uv			: TEXCOORD0;
	float4	weight		: BLENDWEIGHT0;
	//float	weight1		: BLENDWEIGHT1;
	//float	weight2		: BLENDWEIGHT2;
	//float	weight3		: BLENDWEIGHT3;
	int4	boneIdx		: BLENDINDICES0;
	//int		boneIdx1	: BLENDINDICES1;
	//int		boneIdx2	: BLENDINDICES2;
	//int		boneIdx3	: BLENDINDICES3;
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