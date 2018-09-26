#pragma once
#include <string>

#define NUMOFBONE 4




namespace FBX
{
	struct float3
	{
		float x, y, z;
	};
	struct float4
	{
		float x, y, z, w;
	};

	using Matrix = float[16];
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
		Matrix	mat;
	};
	struct Curve
	{
		__int64				curveIdx;
		std::vector<Key>	key;
	};
	struct CurveNode
	{
		__int64				nodeIdx;
		std::vector<Key>	translation;
		std::vector<Key>	rotation;
		std::vector<Key>	scale;
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
		char boneName[32];
		Matrix toParent;
		Matrix offset;

		std::vector<Keyframe> translation;
		std::vector<Keyframe> rotation;
		std::vector<Keyframe> scale;
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

class CFBXData
{
public:
	bool ReadFile(const char* fileName);
	void Destroy();
	inline bool FileErrorShutDown(std::ifstream& fs) {
		fs.close();
		Destroy();
		return false;
	};

	//모델 길이만큼 위로 올릴려고!
	XMFLOAT3 GetExtents() {
		return m_xmf3Extents;
	}

public:
	CFBXData();
	~CFBXData();

public:
	std::string m_fileName;

	FBX::Material * m_pMaterial;
	int m_nMaterial;

	FBX::Name* m_pName;
	int m_nName;

	FBX::Vertex* m_pVertex;
	int m_nVertex;

	FBX::UV* m_pUV;
	int m_nUV;

	FBX::IAVertex* m_pIAVertex;
	int m_nIAVertex;

	XMFLOAT3 m_xmf3Extents;

};

