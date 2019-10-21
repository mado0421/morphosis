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
cbuffer cbAnimMatrixInfo				: register(b3)
{
	matrix		AnimMatrix[64]			: packoffset(c0);
};
cbuffer cbUIInfo						: register(b2)
{
	matrix		gmtxUI					: packoffset(c0);
	float2		gf2Size					: packoffset(c4);
	float		gfAlpha					: packoffset(c8);
};
Texture2D gtxtDiffuseTexture			: register(t4);
Texture2D gtxtNormalTexture				: register(t5);
RWTexture2D<float4> gtxtOutput;

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
	float3 tangent		: TANGENT;
};
struct VS_MODEL_OUTPUT {
	float4 position		: SV_POSITION;
	float3 positionW	: POSITION;
	float3 normalW		: NORMAL;
	float3 tangentW		: TANGENT;
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

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	// Uncompress each component from [0,1] to [-1,1].
	float3 normalT = 2.0f*normalMapSample - 1.0f;

	// Build orthonormal basis.
	float3 N = unitNormalW;
	float3 T = normalize(tangentW - dot(tangentW, N)*N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	// Transform from tangent space to world space.
	float3 bumpedNormalW = mul(normalT, TBN);

	return bumpedNormalW;
}

