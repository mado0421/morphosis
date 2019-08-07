#pragma once
#include "stdafx.h"

struct CB_ANIMDATA_INFO {
	XMFLOAT4X4 interpolatedMatrix;
};

struct AnimationClip;

class CAnimationController {
public:
	~CAnimationController();

public:
	void		AddAnimData(AnimationClip* animData);
	XMMATRIX	GetFinalMatrix(int boneIdx, float time);
	float		GetEndTime();
	void		ChangeAnimClip(const char* animClipName);

	void		SetName(const char* name) { m_strName = name; }

	bool		IsClipEnd(float time);
private:
	XMMATRIX	GetInterpolatedToRootMtx(int boneIdx, float time);
	XMMATRIX	GetOffset(int boneIdx);
	int			GetPrevIdx(float time);
	float		GetNormalizedTime(float time, int boneIdx);
	float		GetClampTime(float time);

public:
	CAnimationController() = default;

public:
	std::vector<AnimationClip*>		m_AnimData;
	int								m_AnimState = 0;

	string							m_strName	= "";
};
