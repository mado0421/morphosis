#pragma once
#include <iostream>		// default header
#include <fstream>		// for read/write file
#include <Windows.h>	// for use UINT
#include <DirectXMath.h>// for use types
#include <vector>
#include <algorithm>

using namespace std;
using namespace DirectX;

struct Vertex {
	XMFLOAT2	uv;
	XMFLOAT3	pos;
	XMFLOAT3	normal;
	XMFLOAT4	weights;
	XMINT4		boneIdx;
};

// 먼저 Geometry를 채운다.
// 그 다음 그 안의 정보를 이용해서 Vertex를 만들고
// 그걸 바이너리 파일로 저장해서 한 번에 읽어서 올릴 수 있게 만들기
struct Geometry {
	__int64		ID;

	UINT		nVertices;
	XMFLOAT3	*pVertices;
	UINT		nPolygonVertexIndex;
	UINT		*pPolygonVertexIndex;
	UINT		nNormals;
	XMFLOAT3	*pNormals;
	UINT		nUV;
	XMFLOAT2	*pUV;
	UINT		nUVIndex;
	UINT		*pUVIndex;

	XMFLOAT3	LclTranslation = { 0,0,0 };
	XMFLOAT3	LclRotation = { 0,0,0 };
};

struct AnimationCurve {
	__int64 ID;
	UINT	nKeyTime;
	__int64 *pKeyTime;
	UINT	nKeyValueFloat;
	float	*pKeyValueFloat;
};

struct AnimationCurveNode {
	__int64		ID;

	AnimationCurve *animCurve[3] = { nullptr,nullptr,nullptr };
	XMFLOAT3		animCurveValue;
};

struct Model {
	Model		*parent			= nullptr;
	__int64		ID;
	XMFLOAT3	PreRotaion		= { 0,0,0 };
	XMFLOAT3	LclTranslation	= { 0,0,0 };
	XMFLOAT3	LclRotation		= { 0,0,0 };
	XMFLOAT3	LclScale		= { 0,0,0 };

	AnimationCurveNode *ACN[3] = { nullptr,nullptr,nullptr };
};

struct Cluster {
	__int64		ID;

	int			boneIdx;

	UINT		nIndexes;
	UINT		*pIndexes;
	UINT		nWeights;
	float		*pWeights;
	XMFLOAT4X4	Transform;
	XMFLOAT4X4	TransformLink;
};

//struct Bone {
//	__int64 ID;
//	Bone * parent;
//	LimbNode * limbnodeData;
//	AnimationCurve *x, *y, *z;
//	Cluster *cluster;
//};
namespace C {
	enum {
		t = 0,
		r,
		s
	};
}

struct Connection {
	__int64 left, right;
	int type;

	Connection(__int64 l, __int64 r, int type = -1) : left(l), right(r), type(type) {}
};

struct Deformer {
	__int64 ID;

	Cluster *cluster;
};

struct Key {
	float ketTime;
	float value;
};

struct EXBone {
	// PreRotation, LclInfos
	Model		*modelData;

	//// AnimCurveInfo
	//vector<Key> animCurve[3];
	//XMFLOAT3	animCurveValue;
};

struct EXGeometry {
	// PreRotation, LclInfos
	Model		*modelData;

	// Vertices, PolygonVertexIndex, Normals, UV, UVIndex
	Geometry	*geoData;

	// per PolygonVertexIndex 마다 boneIdx, weight
	UINT		*EffectedCount;
	XMINT4		*pBoneIdx;
	XMFLOAT4	*pWeight;
};

