#pragma once
#ifndef _FSTREAM_
#include<fstream>
#endif // !_FSTREAM_
namespace cv
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

class FBXDataManager
{
public:
	FBXDataManager();
	virtual ~FBXDataManager();

	bool FileRead(std::string fileName);
	bool FileRead(const char* fileName);
	bool FileRead();
	void Destroy();
	inline bool FileErrorShutDown(std::ifstream& fs) {
		fs.close();
		Destroy();
		return false;
	};

public:
	std::string m_fileName;

	cv::Material * m_pMaterial;
	int m_nMaterial;

	cv::Name* m_pName;
	int m_nName;

	cv::Vertex* m_pVertex;
	int m_nVertex;

	cv::UV* m_pUV;
	int m_nUV;

	cv::IAVertex* m_pIAVertex;
	int m_nIAVertex;

};
