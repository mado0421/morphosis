#pragma once
#include "stdafx.h"
#include <iostream>
#include <iomanip>

struct CB_ANIMDATA_INFO {
	XMFLOAT4X4 interpolatedMatrix;
};

struct AnimationVertex {
public:
	AnimationVertex() {}
	AnimationVertex(const XMFLOAT3& p) :pos(p) {}

public:
	XMFLOAT3	pos;
	XMINT4		boneIdx;
	XMFLOAT4	weight;
};

class AnimationMesh {
public:
	AnimationMesh() {}
	AnimationMesh(const char* name) { this->name = name; }

public:
	std::string						name;
	std::vector<AnimationVertex>	controlPoints;
	std::vector<int>				polygonVertexIndex;
};

class AnimationBone {
public:
	AnimationBone() = default;

	void MakeToWorldMatrix() {
		XMMATRIX tmp = XMMatrixMultiply(XMLoadFloat4x4(&m_Local), XMLoadFloat4x4(&m_toParent));
		XMMATRIX toWorld;
		if (m_parent)	toWorld = XMMatrixMultiply(tmp, XMLoadFloat4x4(&m_parent->m_toWorld));
		else			toWorld = tmp;
		XMStoreFloat4x4(&m_toWorld, toWorld);
	}

	void Init() {
		MakeToParentMatrix();
		MakeDressposeInverseMatrix();
	}

private:
	void MakeToParentMatrix() {
		XMMATRIX t = XMMatrixTranslation(m_translation.x, m_translation.y, m_translation.z);
		XMMATRIX r = XMMatrixTranspose(XMMatrixRotationRollPitchYawDegree(m_rotation.x, m_rotation.y, m_rotation.z));
		XMStoreFloat4x4(&m_toParent, XMMatrixMultiply(r, t));
	}
	void MakeDressposeInverseMatrix() {
		XMVECTOR det = XMMatrixDeterminant(XMLoadFloat4x4(&m_toParent));
		XMMATRIX toParentInv = XMMatrixInverse(&det, XMLoadFloat4x4(&m_toParent));
		XMMATRIX dressposeInv;
		if (m_parent) 	dressposeInv = XMMatrixMultiply(XMLoadFloat4x4(&m_parent->m_dressposeInv), toParentInv);		
		else 			dressposeInv = toParentInv;
		XMStoreFloat4x4(&m_dressposeInv, dressposeInv);
	}

public:
	AnimationBone*	m_parent;
	XMFLOAT4X4		m_Local;
	XMFLOAT3		m_translation;
	XMFLOAT3		m_rotation;

	XMFLOAT4X4		m_dressposeInv;
	XMFLOAT4X4		m_toWorld;
	XMFLOAT4X4		m_toParent;

public:
	int				m_parentIdx;
};

class AnimationKey {
public:

	static bool CompareByKeyTime(const AnimationKey& k1, const AnimationKey& k2) {
		return (k1.m_keytime < k2.m_keytime);
	}

public:
	std::vector<int>			m_boneIdx;
	std::vector<XMFLOAT3>		m_translations;
	std::vector<XMFLOAT3>		m_rotations;

	std::vector<AnimationBone*>	m_pBones;
	float						m_keytime;
};

struct BoneReferenceInfo {
	std::set<int> idxOfKeySet;
};

class AnimationData {
public:
	void		Init(std::vector<AnimationBone> bones, std::vector<AnimationKey> keys) {

		m_bones.resize(bones.size());
		for (int i = 0; i < m_bones.size(); ++i) {
			if (-1 != bones[i].m_parentIdx) {
				m_bones[i].m_parent = &m_bones[bones[i].m_parentIdx];
			}
			else m_bones[i].m_parent = NULL;
			m_bones[i].m_translation	= bones[i].m_translation;
			m_bones[i].m_rotation		= bones[i].m_rotation;
		}
		for (int i = 0; i < m_bones.size(); ++i) {
			m_bones[i].Init();
		}

		m_boneReferenceInfo.resize(bones.size());

		m_keys.resize(keys.size());
		for (int i = 0; i < m_keys.size(); ++i) {

			m_keys[i].m_keytime =				keys[i].m_keytime;
			m_keys[i].m_pBones.resize(			keys[i].m_pBones.size()			);
			m_keys[i].m_boneIdx.resize(			keys[i].m_boneIdx.size()		);
			m_keys[i].m_translations.resize(	keys[i].m_translations.size()	);
			m_keys[i].m_rotations.resize(		keys[i].m_rotations.size()		);

			for (int j = 0; j < m_keys[i].m_boneIdx.size(); ++j) {

				m_keys[i].m_pBones[j]		= &m_bones[keys[i].m_boneIdx[j]];
				m_keys[i].m_boneIdx[j]		= keys[i].m_boneIdx[j];
				m_keys[i].m_translations[j] = keys[i].m_translations[j];
				m_keys[i].m_rotations[j]	= keys[i].m_rotations[j];

				m_boneReferenceInfo[keys[i].m_boneIdx[j]].idxOfKeySet.insert(i);
			}
		}
	}

	void		GenerateToWorldMatrix(float time) {
		for (int i = 0; i < m_bones.size(); ++i) {
			XMStoreFloat4x4(&m_bones[i].m_Local, GetInterpolatedLocalMatrix(i, time));
			m_bones[i].MakeToWorldMatrix();
		}
	}
	XMMATRIX	GetFinalMatrix(int boneIdx) {
		return XMMatrixMultiply(XMLoadFloat4x4(&m_bones[boneIdx].m_dressposeInv), XMLoadFloat4x4(&m_bones[boneIdx].m_toWorld));
	}

private:
	XMMATRIX	GetInterpolatedLocalMatrix(int boneIdx, float time) {
		int curKeyIdx, nextKeyIdx;
		__int64 boneIdxInCurKey;
		__int64 boneIdxInNextKey;
		__int64 boneIdxInFirstKey;


		if (m_boneReferenceInfo[boneIdx].idxOfKeySet.size() == 0) { return XMMatrixIdentity(); }
		if (isFurtherThanBack(time)) if (isLoop) time = GetClampTime(time);

		GetKeyCurIdxNextIdxFromRefInfo(boneIdx, time, curKeyIdx, nextKeyIdx);
		int firstKeyIdx = GetKeyFirstIdxFromRefInfo(boneIdx);
		boneIdxInCurKey = GetBoneIdxInKey(boneIdx, curKeyIdx);
		boneIdxInNextKey = GetBoneIdxInKey(boneIdx, nextKeyIdx);
		boneIdxInFirstKey = GetBoneIdxInKey(boneIdx, firstKeyIdx);


		if (curKeyIdx == nextKeyIdx || m_keys.size() == 1 || isFurtherThanFront(time)) {
			XMFLOAT4X4 tmp;
			Matrix4x4::ToTransform(
				&tmp,
				Vector3::Subtract(m_keys[curKeyIdx].m_translations[boneIdxInCurKey], m_bones[boneIdx].m_translation),
				Vector4::QuatFromAngle(Vector3::Subtract(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey], m_bones[boneIdx].m_rotation)));
			return XMLoadFloat4x4(&tmp);
		}

		//XMFLOAT3 xmf3B = m_bones[boneIdx].m_rotation;
		XMFLOAT3 xmf3R1 = m_keys[curKeyIdx].m_rotations[boneIdxInCurKey];
		XMFLOAT3 xmf3R2 = m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey];
		XMFLOAT3 xmf3B = m_keys[firstKeyIdx].m_rotations[boneIdxInFirstKey];

		return XMMatrixIdentity();

	}
	bool		isFurtherThanBack(float time) { return (time >= m_keys.back().m_keytime); }
	bool		isFurtherThanFront(float time) { return (time <= m_keys.front().m_keytime); }
	float		GetClampTime(float time) { return (time - ((int)(time / m_keys.back().m_keytime) * m_keys.back().m_keytime)); }
	__int64		GetKeyIdxByTime(float time) {
		for (auto p = m_keys.cbegin(); p != m_keys.cend(); ++p) 
			if (time < p->m_keytime) return distance(m_keys.cbegin(), p); 
	}
	__int64		GetBoneIdxInKey(int boneIdx, int keyIdx) {
		for (auto p = m_keys[keyIdx].m_boneIdx.cbegin(); p != m_keys[keyIdx].m_boneIdx.cend(); ++p)
			if (*p == boneIdx) return distance(m_keys[keyIdx].m_boneIdx.cbegin(), p);
	}
	void		GetNormalizedTime(float& time, int keyIdx) {
		time = (time - m_keys[keyIdx].m_keytime) / (m_keys[keyIdx + 1].m_keytime - m_keys[keyIdx].m_keytime);
	}
	int			GetKeyIdxByTimeFromRefInfo(int boneIdx, float time) {
		for (auto p = m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin(); p != m_boneReferenceInfo[boneIdx].idxOfKeySet.cend(); ++p)
			if (time < m_keys[(*p)].m_keytime) return (*p);
	}
	void		GetKeyCurIdxNextIdxFromRefInfo(int boneIdx, float time, int& curIdx, int& nextIdx) {
		if (m_boneReferenceInfo[boneIdx].idxOfKeySet.size() == 1) {
			curIdx = *m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin();
			nextIdx = *m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin();
			return;
		}

		if (m_keys[*m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin()].m_keytime >= time) {
			curIdx = *m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin();
			nextIdx = *m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin();
			return;
		}
		for (auto p = m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin(); p != m_boneReferenceInfo[boneIdx].idxOfKeySet.cend(); ++p) {
			if (time < m_keys[(*p)].m_keytime) {
				nextIdx = *p; p--;
				curIdx = *p;
				return;
			}
		}
		curIdx = *m_boneReferenceInfo[boneIdx].idxOfKeySet.crbegin();
		nextIdx = curIdx;
	}
	int			GetKeyFirstIdxFromRefInfo(int boneIdx) {
		return *m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin();
	}
	float		GetNormalizedTimeByTwoKeys(float time, int curKeyIdx, int nextKeyIdx) {
		return (time - m_keys[curKeyIdx].m_keytime) / (m_keys[nextKeyIdx].m_keytime - m_keys[curKeyIdx].m_keytime);
	}

public:
	AnimationData() = default;

public:
	std::vector<AnimationKey>		m_keys;
	std::vector<AnimationBone>		m_bones;
	std::vector<BoneReferenceInfo>	m_boneReferenceInfo;

	bool							isLoop = true;
};

