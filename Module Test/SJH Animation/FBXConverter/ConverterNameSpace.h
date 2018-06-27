#pragma once
#include"stdafx.h"

#define EPSILON 1.0e-10f
#define NUMOFBONE 4
using namespace DirectX;
namespace cv
{
	struct float3
	{
		float x, y, z;
		float3() {}
		float3(float nx, float ny, float nz) :x(nx), y(ny), z(nz) {}
	};
	struct float4
	{
		float x, y, z, w;
	};

	struct Name
	{
		char string[_MAX_PATH];
	};

	struct Key
	{
		float timePos;
		float value;
	};
	struct Keyframe
	{
		float timePos;
		float3 value;
	};
	struct PoseNode
	{
		__int64 poseIdx;
		XMFLOAT4X4	mat;
	};
	struct Curve
	{
		__int64				curveIdx;
		std::vector<Key>	key;

		bool operator==(__int64 i64) { return (curveIdx == i64); }
	};
	struct CurveNode
	{
		__int64		nodeIdx;

		//	begin from 0 / array index, -1 is invalid value
		int			bone_index = -1;
		int			nTranslation;
		int			nRotation;
		int			nScale;
		Keyframe*	pTranslation;
		Keyframe*	pRotation;
		Keyframe*	pScale;

		bool operator==(__int64 i64) { return (nodeIdx == i64); }
		~CurveNode()
		{
			if (nullptr != pTranslation) delete[] pTranslation;
			if (nullptr != pRotation) delete[] pRotation;
			if (nullptr != pScale) delete[] pScale;
		}
	};
	struct WeightInfo
	{
		int		idx;
		float	weight;
	};
	struct Deformer
	{
		__int64					deformerIdx;
		int						boneIdx;
		std::vector<WeightInfo> weight;
	};

	struct Vertex
	{
		float3	pos;
		WeightInfo weights[NUMOFBONE];

	};
	struct UV
	{
		float u, v;
	};
	struct IAVertex
	{
		int vertexIdx;
		int uvIdx;
		float3 normal;
		float3 biNormal;
		float3 tangent;
	};
	struct Bone
	{
		__int64 boneIdx;
		//char boneName[32];
		int parentIdx;
		XMFLOAT4X4 toParent;
		XMFLOAT4X4 offset;

		bool operator==(__int64 i64) { return (boneIdx == i64); }
	};
	struct Material
	{
		__int64 materialIdx;

		float	EmissiveFactor;
		float3	AmbientColor;
		float3	DiffuseColor;
		float3	TransparentColor;
		float3	SpecularColor;
		float	SpecularFactor;
		float	ShininessExponent;
		float3	Emissive;
		float3	Ambient;
		float3	Diffuse;
		float3	Specular;
		double	Shininess;
		double	Opacity;
		double	Reflectivity;
	};
	struct Model
	{
		//Mesh		mesh;
		char		texture_diff[32];
		char		texture_norm[32];
		Material	material;
	};

	struct AnimationModel : public Model
	{
		std::vector<Bone> bone;
	};

}