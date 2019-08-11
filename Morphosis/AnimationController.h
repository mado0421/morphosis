//#pragma once
//#include "stdafx.h"
//
//struct CB_ANIMDATA_INFO {
//	XMFLOAT4X4 interpolatedMatrix;
//};
//
//struct BoneData {
//	std::string	m_Name;
//	XMFLOAT4X4	m_GlobalTransform;
//	XMFLOAT4X4	*m_pToRootTransforms;
//	int			m_nKeyframe;
//
//	~BoneData() {
//		if (m_pToRootTransforms) {
//			delete[] m_pToRootTransforms;
//		}
//	}
//};
//
//struct AnimClip {
//public:
//	~AnimClip();
//
//public:
//	std::string m_AnimName;
//	int			m_nBoneList;
//	int			m_nKeyTime;
//
//	BoneData	*m_BoneList;
//	double		*m_KeyTime;
//
//	bool		m_IsLoop;
//};
//
//class CAnimCtrl {
//public:
//
//public:
//	virtual void		AddAnimData(const char* animClipName);
//	virtual XMMATRIX	GetFinalMatrix(int boneIdx, float time);
//	float				GetEndTime();
//	void				ChangeAnimClip(const char* animClipName);
//
//	int GetNumOfBone() { return g_vecAnimClip[m_vecAnimClipIdx[m_AnimState]]->m_nBoneList; }
//
//
//	void				SetName(const char* name) { m_strName = name; }
//
//	bool				IsClipEnd(float time);
//protected:
//	virtual XMMATRIX	GetInterpolatedToRootMtx(int boneIdx, float time);
//	XMMATRIX			GetOffset(int boneIdx);
//	int					GetPrevIdx(float time);
//	float				GetNormalizedTime(float time, int boneIdx);
//	float				GetClampTime(float time);
//
//public:
//	CAnimCtrl() = default;
//
//public:
//	vector<int>					m_vecAnimClipIdx;
//	//std::vector<AnimClip*>		m_AnimData;
//	int							m_AnimState = 0;
//
//	string						m_strName	= "";
//};
//
////class CHumanoidAnimCtrl : public CAnimCtrl {
////public:
////	CHumanoidAnimCtrl() {
////
////	}
////	virtual void		AddAnimData(AnimClip* animData);
////	virtual XMMATRIX	GetFinalMatrix(int boneIdx, float time);
////protected:
////	virtual XMMATRIX	GetInterpolatedToRootMtx(int boneIdx, float time);
////
////public:
////	float m_XAxisRotationAngle = 0;
////	std::vector<float> m_vecRotationAngle;
////};