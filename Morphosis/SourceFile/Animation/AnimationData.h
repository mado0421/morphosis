#pragma once
#include "stdafx.h"

struct CB_ANIMDATA_INFO {
	XMFLOAT4X4 interpolatedMatrix;
};

struct ImportAnimData;

class AnimationData {
public:
	void Init(ImportAnimData& animData);
	void AddAnimData(ImportAnimData* animData);
	XMMATRIX	GetFinalMatrix(int boneIdx, float time);
	float		GetEndTime();

private:
	XMMATRIX	GetInterpolatedToRootMtx(int boneIdx, float time);
	XMMATRIX	GetOffset(int boneIdx);
	int GetPrevIdx(float time);
	float GetNormalizedTime(float time, int boneIdx);
	float GetClampTime(float time);

public:
	AnimationData() = default;

public:
	std::vector<ImportAnimData*>	m_AnimData;
	int								m_AnimState = 0;

	bool							isLoop = true;
};

