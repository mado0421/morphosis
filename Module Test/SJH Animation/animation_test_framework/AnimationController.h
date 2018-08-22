#pragma once
#include"FBXDataManager.h"
#include<vector>
#include<iostream>


struct Bone
{
	XMFLOAT4X4 toParent;
	XMFLOAT4X4 offset;

	XMFLOAT4X4 matrix;
	int parentIdx;
	float padding[3];
};


#define MAXBONE	60
struct BONES {
	Bone bone[MAXBONE];
};

struct CurveSet
{
	cv::CurveNode* m_pCurveNode;
	int m_nCurveNode;
	float end_time;

	CurveSet(){
		m_pCurveNode = nullptr;
		m_nCurveNode = 0;
	}
	~CurveSet(){
		delete[] m_pCurveNode;
	}
};

class AnimationController
{
public:
	int m_nBone;
	cv::CBone* m_pBone;

	BONES* m_Bones;
	ID3D12Resource *m_pBoneResource;

	BONES* m_pcbMappedBones;


	//	애니메이션 1개일 때
	int m_nCurveNode;
	cv::CurveNode* m_pCurveNode;


	XMFLOAT4X4* res_matrix;

	float local_time = 0;
	float endTime;

	/* 나중에 수정하여 사용 할 것 */
public:
	std::vector<CurveSet> m_vCurveNode;
	int AnimationState = 0;	// e.g: idle=0, run=1, jump=2, ...

public:
	AnimationController();
	virtual ~AnimationController();
	
	void Init(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,FBXDataManager* pFbxDM);

	void Update(float fElapsedTime, ID3D12GraphicsCommandList *pd3dCommandList);

	void AnimaionUpdate();
	void BoneUpdate(ID3D12GraphicsCommandList *pd3dCommandList);

	inline XMFLOAT4X4 AffineTransformation(XMFLOAT3& Scaling, XMFLOAT4& RotationOrigin,
		XMFLOAT3& Rotation, XMFLOAT3& Translation);
	
	inline XMFLOAT4X4 Transpose(XMFLOAT4X4& xmmtx4x4Matrix);
	inline XMFLOAT4X4 Identity();
	
	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2);

	inline XMFLOAT4X4 Inverse(XMFLOAT4X4& matrix);
};



