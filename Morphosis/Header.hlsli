SamplerState gSamplerState				: register(s0);
cbuffer cbCameraInfo					: register(b0)
{
	matrix		gmtxView				: packoffset(c0);
	matrix		gmtxProjection			: packoffset(c4);
	float3		gvCameraPosition		: packoffset(c8);
};
cbuffer cbGameObjectInfo				: register(b1)
{
	matrix		gmtxGameObject			: packoffset(c0);
	uint		gnMaterial				: packoffset(c4);
};
Texture2D gtxtTexture					: register(t2);
cbuffer cbAnimMatrixInfo				: register(b3)
{
	matrix		AnimMatrix[64]			: packoffset(c0);
};
cbuffer cbMaterialInfo					: register(b4)
{
	matrix		Material[64]			: packoffset(c0);
};
cbuffer cbLightlInfo					: register(b5)
{
	matrix		Light[64]				: packoffset(c0);
};




struct ANIM_ILLUM_TEX_INPUT
{
	float3	position	: POSITION;
	float3	normal		: NORMAL;
	float3	tangent		: TANGENT;
	float2	uv			: TEXCOORD0;
	float4	weight		: BLENDWEIGHT0;
	int4	boneIdx		: BLENDINDICES0;
};

struct ANIM_ILLUM_TEX_OUTPUT
{
	float4 position		: SV_POSITION;
	float3 positionW	: POSITION;
	float3 normal		: NORMAL0;
	float3 normalW		: NORMAL1;
	float3 tangentW		: TANGENT;
	float2 uv			: TEXCOORD;
};

struct ILLUM_TEX_INPUT
{
	float3 position		: POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
};

struct ILLUM_TEX_OUTPUT
{
	float4 position		: SV_POSITION;
	float3 positionW	: POSITION;
	float3 normalW		: NORMAL;
	float2 uv			: TEXCOORD;
};

struct VS_DEBUG_INPUT
{
	float3 position : POSITION;
};
struct VS_DEBUG_OUTPUT
{
	float4 position : SV_POSITION;
};