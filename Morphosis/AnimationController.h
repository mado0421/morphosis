#pragma once
#include "stdafx.h"

struct CB_ANIMDATA_INFO {
	XMFLOAT4X4 interpolatedMatrix;
};

struct ImportBone;

struct AnimationClip {
public:
	AnimationClip()
		: m_AnimName("")
		, m_nBoneList(0)
		, m_nKeyTime(0)
		, m_BoneList(NULL)
		, m_KeyTime(NULL)
		, m_IsLoop(false)
	{}
	AnimationClip(const AnimationClip& a);
	~AnimationClip();

public:
	void ImportFile(const char* fileName);
	void SetIsLoop(bool IsLoop) { m_IsLoop = IsLoop; }

public:
	std::string m_AnimName;
	int			m_nBoneList;
	int			m_nKeyTime;

	ImportBone*	m_BoneList;
	double*		m_KeyTime;

	bool		m_IsLoop;
};

class CAnimationController {
public:
	~CAnimationController();

public:
	virtual void		AddAnimData(AnimationClip* animData);
	virtual XMMATRIX	GetFinalMatrix(int boneIdx, float time);
	float				GetEndTime();
	void				ChangeAnimClip(const char* animClipName);

	void				SetName(const char* name) { m_strName = name; }

	bool				IsClipEnd(float time);
protected:
	virtual XMMATRIX	GetInterpolatedToRootMtx(int boneIdx, float time);
	XMMATRIX			GetOffset(int boneIdx);
	int					GetPrevIdx(float time);
	float				GetNormalizedTime(float time, int boneIdx);
	float				GetClampTime(float time);

public:
	CAnimationController() = default;

public:
	std::vector<AnimationClip*>		m_AnimData;
	int								m_AnimState = 0;

	string							m_strName	= "";
};

class CHumanoidAnimCtrl : public CAnimationController {
public:
	virtual void		AddAnimData(AnimationClip* animData);
	virtual XMMATRIX	GetFinalMatrix(int boneIdx, float time);
protected:
	virtual XMMATRIX	GetInterpolatedToRootMtx(int boneIdx, float time);

public:
	float m_XAxisRotationAngle = 0;
	std::vector<float> m_vecRotationAngle;
};