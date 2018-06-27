#pragma once
#include"FBXDataManager.h"
#include<vector>
#include<iostream>
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
	cv::Bone* m_pBone;
	int m_nBone;

	//	�ִϸ��̼� 1���� ��
	cv::CurveNode* m_pCurveNode;
	int m_nCurveNode;

	float time = 0;
	float endTime;

	/* ���߿� �����Ͽ� ��� �� �� */
public:
	std::vector<CurveSet> m_vCurveNode;
	int AnimationState = 0;	// e.g: idle=0, run=1, jump=2, ...

public:
	AnimationController();
	virtual ~AnimationController();
	
	void Init(FBXDataManager* pFbxDM);
	void Update(float fElapsedTime);


};

