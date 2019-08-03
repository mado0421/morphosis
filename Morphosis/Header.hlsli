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
	//matrix		gmtxGameObjectNoTrans	: packoffset(c4);
	//uint		gnMaterial				: packoffset(c4);
};
Texture2D gtxtTexture					: register(t2);
cbuffer cbAnimMatrixInfo				: register(b3)
{
	matrix		AnimMatrix[64]			: packoffset(c0);
};
cbuffer cbUIInfo						: register(b4)
{
	matrix		gmtxUI					: packoffset(c0);
	float2		gf2Size					: packoffset(c4);

};
//cbuffer cbMaterialInfo					: register(b4)
//{
//	matrix		Material[64]			: packoffset(c0);
//};
//cbuffer cbLightlInfo					: register(b5)
//{
//	matrix		Light[64]				: packoffset(c0);
//};

struct VS_ANIM_INPUT {
	float3	position	: POSITION;
	float3	normal		: NORMAL;
	float3	tangent		: TANGENT;
	float2	uv			: TEXCOORD0;
	float4	weight		: BLENDWEIGHT0;
	int4	boneIdx		: BLENDINDICES0;
};
struct VS_ANIM_OUTPUT {
	float4 position		: SV_POSITION;
	float3 positionW	: POSITION;
	float3 normalW		: NORMAL0;
	float3 tangentW		: TANGENT;
	float2 uv			: TEXCOORD;
};

struct VS_MODEL_INPUT {
	float3 position		: POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
};
struct VS_MODEL_OUTPUT {
	float4 position		: SV_POSITION;
	float3 positionW	: POSITION;
	float3 normalW		: NORMAL;
	float2 uv			: TEXCOORD;
};

struct VS_UI_INPUT {
	float3 position		: POSITION;
	float2 uv			: TEXCOORD;
};
struct VS_UI_OUTPUT {
	float4 position		: SV_POSITION;
	float3 positionW	: POSITION;
	float2 uv			: TEXCOORD;
};

struct VS_DEBUG_INPUT {
	float3 position : POSITION;
};
struct VS_DEBUG_OUTPUT {
	float4 position : SV_POSITION;
};