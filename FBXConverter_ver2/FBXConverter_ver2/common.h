#pragma once

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

#include <fbxsdk.h>

#include <d3d12.h>
#include <DirectXMath.h>

#include <vector>

using namespace DirectX;
using namespace std;

struct MyControlPoint {
public:
	MyControlPoint() = default;
	MyControlPoint(const XMFLOAT3& pos) :m_position(pos) {}

public:
	XMFLOAT3	m_position;
	XMINT4		m_boneIdx;
	XMFLOAT4	m_boneWeight;	//XMFLOAT3로 하고 z = 1 - (x + y + z) 가 정석이지만 귀찮으므로
};

struct MyVertex {
public:
	int		 m_iCtrlPointIdx;
	XMFLOAT2 m_xmf2UV;
	XMFLOAT3 m_xmf3Normal;
	XMFLOAT3 m_xmfBinormal;
	XMFLOAT3 m_xmfTangent;
};

struct MyMesh {
public:
	MyMesh() = default;
	MyMesh(const char* name) :m_strName(name) {}

public:
	string					m_strName;
	vector<MyControlPoint>	m_vecControlPoint;
	vector<MyVertex>		m_vecVertex;
	vector<int>				m_vecPolygonVertexIndex;
};

struct MyBone {
public:
	MyBone() = default;
	MyBone(
		const char* name,
		int boneIdx,
		const XMFLOAT3& LclTranslation,
		const XMFLOAT3& LclRotation,
		FbxNode* node)
		: m_strName(name)
		, m_boneIdx(boneIdx)
		, m_xmf3LclTranslation(LclTranslation)
		, m_xmf3LclRotation(LclRotation)
		, m_pNode(node)
	{}

public:
	string		m_strName;
	XMFLOAT3	m_xmf3LclTranslation;
	XMFLOAT3	m_xmf3LclRotation;
	MyBone*		m_pParent	= NULL;
	FbxNode*	m_pNode		= NULL;
	int			m_boneIdx;
};

struct MyKey {
	
};

struct MyKeyFrame {
	double			m_dKeyTime;
	vector<MyKey>	m_vecKey;
};