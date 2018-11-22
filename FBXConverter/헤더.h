#pragma once
#include <iostream>		// default header
#include <fstream>		// for read/write file
#include <Windows.h>	// for use UINT
#include <DirectXMath.h>// for use types

using namespace std;
using namespace DirectX;

struct Vertex {
	XMFLOAT2	uv;
	XMFLOAT3	pos;
	XMFLOAT3	normal;
	XMFLOAT4	weights;
	XMINT4		boneIdx;
};

// ���� Geometry�� ä���.
// �� ���� �� ���� ������ �̿��ؼ� Vertex�� �����
// �װ� ���̳ʸ� ���Ϸ� �����ؼ� �� ���� �о �ø� �� �ְ� �����
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

struct Model {
	__int64 ID;

	XMFLOAT3 LclTranslation = { 0,0,0 };
	XMFLOAT3 LclRotation	= { 0,0,0 };
};


struct LimbNode {
	__int64 ID;
	XMFLOAT3 LclTranslation;
	XMFLOAT3 LclRotation;
};

struct Cluster {
	__int64 ID;
	UINT nIndexes;
	UINT *pIndexes;
	UINT nWeights;
	float *pWeights;
	XMFLOAT4X4 Transform;
	XMFLOAT4X4 TransformLink;
};

struct AnimationCurve {
	__int64 ID;
	UINT nKeyTime;
	__int64 *pKeyTime;
	UINT nKeyValueFloat;
	float *pKeyValueFloat;
};

struct AnimationCurveNode {
	__int64 SubDeformerID;
	__int64 ID;
	XMFLOAT3 value;

	AnimationCurve *x, *y, *z;
};

struct SubDeformer {
	Cluster *p;
};


struct Bone {
	__int64 ID;
	Bone * parent;
	LimbNode * limbnodeData;
	AnimationCurve *x, *y, *z;
	Cluster *cluster;
};

namespace TYPE {
	enum {
		NONE = -1,
		MODEL,
		ANIMCURVENODE,
		ANIMCURVE,
		DEFORMER,
		SUBDEFORMER,

		count
	};
}

namespace SUBTYPE {
	enum {
		NONE = -1,
		DX,
		DY,
		DZ,

		count
	};
}

struct Connection {
	__int64 ID;
	int type;
	void * data;
};

struct ConnectionPair {
	__int64 leftID, rightID;
	int subtype;
};