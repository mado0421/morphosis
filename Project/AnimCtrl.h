#pragma once

class Bone {
public:
	string				m_BoneName;
	XMFLOAT4X4			m_xmf4x4GlobalTransform;
	vector<XMFLOAT4X4>	m_vecToRootTransform;
};

class AnimClip {
public:
	string			m_AnimName;
	vector<Bone>	m_vecBone;
	vector<double>	m_vecKeyTime;
	bool			m_IsLoop;
};

class AnimCtrl {
public:
	AnimCtrl();

public:
	void		AddAnimClip(const char* AnimName);
	int			GetNumOfBone();

	XMMATRIX	GetAnimationMatrix(int boneIdx, float time);

protected:
	XMMATRIX	GetInterpolatedToRootMtx(int boneIdx, float time);
	float		GetClampTime(float time);
	int			GetPrevIdx(float time);
	float		GetNormalizedTime(float time, int boneIdx);

public:
	string		m_AnimCtrlName;
	vector<int> m_vecClipIdx;
	int			m_AnimState = 0;
};