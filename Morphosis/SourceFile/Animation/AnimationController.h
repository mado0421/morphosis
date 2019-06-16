#pragma once
#include "stdafx.h"

struct CB_ANIMDATA_INFO {
	XMFLOAT4X4 interpolatedMatrix;
};

struct AnimationClip;

class CAnimationController {
public:
	void		Init(AnimationClip& animData);
	void		AddAnimData(AnimationClip* animData);
	XMMATRIX	GetFinalMatrix(int boneIdx);
	float		GetEndTime();
	void		ChangeAnimClip(const char* animClipName);
	void		Update(float fTime);

private:
	XMMATRIX	GetInterpolatedToRootMtx(int boneIdx);
	XMMATRIX	GetOffset(int boneIdx);
	int			GetPrevIdx(float time);
	float		GetNormalizedTime(float time, int boneIdx);
	float		GetClampTime(float time);

public:
	CAnimationController() = default;

public:
	std::vector<AnimationClip*>		m_AnimData;
	int								m_AnimState = 0;

	bool							isLoop = true;
	float							m_fTime = 0;
};

