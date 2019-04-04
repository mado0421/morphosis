#pragma once
#include "stdafx.h"
#include <iostream>
#include <iomanip>


struct CB_ANIMDATA_INFO {
	XMFLOAT4X4 interpolatedMatrix;
};

inline double GetTime(__int64 int64time) {
	__int64 i64 = int64time * 0.01;
	double i64d = i64 / 30790772.0;
	return (i64d / 30.0);
}

struct AnimationVertex {
public:
	AnimationVertex() {}
	AnimationVertex(Float3 p) :pos(p) {}

public:
	Float3	pos;
	Int4	boneIdx;
	Float4	weight;
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
	AnimationBone(XMFLOAT3 translation, XMFLOAT3 rotation, AnimationBone* parent)
		: m_parent(parent)
	{
		//toParent 구함
		MakeToParentMatrix(translation, rotation);

		//toDresspose 구함
		MakeToDressposeMatrix();

		//toDressposeInverse 구함
		MakeToDressposeInverseMatrix();
	}
	XMMATRIX GetLocalMatrix() {
		XMMATRIX rotM = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_rotation.x), XMConvertToRadians(m_rotation.y), XMConvertToRadians(m_rotation.z));
		XMMATRIX trsM = XMMatrixTranslation(m_translation.x, m_translation.y, m_translation.z);
		return XMMatrixMultiply(trsM, rotM);
	}
	XMFLOAT3 GetToWorldPosition() { return XMFLOAT3(m_toWorld._41, m_toWorld._42, m_toWorld._43); }
	XMFLOAT3 GetToDressposePosition() { return XMFLOAT3(m_toDresspose._41, m_toDresspose._42, m_toDresspose._43); }
	void SetThisBoneToRoot() { isRootBone = true; }
	void MakeToWorldMatrix() {
		XMMATRIX ToWorld;
		if (!m_parent) ToWorld = XMMatrixMultiply(XMLoadFloat4x4(&m_Local), XMMatrixMultiply(XMLoadFloat4x4(&m_toParent), XMMatrixIdentity()));
		else ToWorld = XMMatrixMultiply(XMLoadFloat4x4(&m_Local), XMMatrixMultiply(XMLoadFloat4x4(&m_toParent), XMLoadFloat4x4(&m_parent->m_toWorld)));
		XMStoreFloat4x4(&m_toWorld, ToWorld);
	}
	void MakeToParentMatrix() {
		if (!m_parent) { XMStoreFloat4x4(&m_toParent, XMMatrixIdentity()); return; }
		XMFLOAT3 toParentTrans = XMFLOAT3(m_parent->m_translation.x - m_translation.x, m_parent->m_translation.y - m_translation.y, m_parent->m_translation.z - m_translation.z);
		XMFLOAT3 toParentRotate = XMFLOAT3(m_parent->m_translation.x - m_translation.x, m_parent->m_translation.y - m_translation.y, m_parent->m_translation.z - m_translation.z);

		XMStoreFloat4x4(&m_toParent, XMMatrixRotationRollPitchYaw(XMConvertToRadians(toParentRotate.x), XMConvertToRadians(toParentRotate.y), XMConvertToRadians(toParentRotate.z)));
		m_toParent._41 = toParentTrans.x;
		m_toParent._42 = toParentTrans.y;
		m_toParent._43 = toParentTrans.z;
	}

	void Init() {
		//toParent 구함
		MakeToParentMatrix();

		//toDresspose 구함
		MakeToDressposeMatrix();

		//toDressposeInverse 구함
		MakeToDressposeInverseMatrix();
	}

private:
	void MakeToParentMatrix(XMFLOAT3 LclTrans, XMFLOAT3 LclRotate) {
		XMStoreFloat4x4(&m_toParent, XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(LclRotate.x),
			XMConvertToRadians(LclRotate.y),
			XMConvertToRadians(LclRotate.z)
		));
		m_toParent._41 = LclTrans.x;
		m_toParent._42 = LclTrans.y;
		m_toParent._43 = LclTrans.z;
	}
	void MakeToDressposeMatrix() {
		if (!m_parent) XMStoreFloat4x4(&m_toDresspose, XMMatrixMultiply(XMLoadFloat4x4(&m_toParent), XMMatrixIdentity()));
		else XMStoreFloat4x4(&m_toDresspose, XMMatrixMultiply(XMLoadFloat4x4(&m_toParent), XMLoadFloat4x4(&m_parent->m_toDresspose)));
	}
	void MakeToDressposeInverseMatrix() {
		XMVECTOR det;
		det = XMMatrixDeterminant(XMLoadFloat4x4(&m_toDresspose));
		XMStoreFloat4x4(&m_toDressposeInverse, XMMatrixInverse(&det, XMLoadFloat4x4(&m_toDresspose)));
	}

public:
	AnimationBone*	m_parent;
	XMFLOAT4X4		m_Local;
	XMFLOAT3		m_translation;
	XMFLOAT3		m_rotation;
	XMFLOAT4X4		m_toDressposeInverse;
	XMFLOAT4X4		m_toWorld;
	XMFLOAT4X4		m_toParent;
	XMFLOAT4X4		m_toDresspose;
	bool			isRootBone = false;

public:
	int				m_parentIdx;
};

class AnimationKey {
public:
	std::vector<int>			m_boneIdx;
	std::vector<Float3>			m_translations;
	std::vector<Float3>			m_rotations;

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
			m_bones[i].MakeToParentMatrix(); 
		}

		m_boneReferenceInfo.resize(bones.size());

		m_keys.resize(keys.size());
		for (int i = 0; i < m_keys.size(); ++i) {
			m_keys[i].m_keytime = keys[i].m_keytime;
			m_keys[i].m_pBones.resize(keys[i].m_pBones.size());
			m_keys[i].m_boneIdx.resize(keys[i].m_boneIdx.size());
			m_keys[i].m_translations.resize(keys[i].m_translations.size());
			m_keys[i].m_rotations.resize(keys[i].m_rotations.size());
			for (int j = 0; j < m_keys[i].m_boneIdx.size(); ++j) {
				m_keys[i].m_pBones[j] = &m_bones[keys[i].m_boneIdx[j]];
				//m_keys[i].m_boneIdx[j] = keys[i].m_boneIdx[j];
				//m_key
				//m_keys[i].m_pBones[j]->m_translation = XMFLOAT3(keys[i].m_translations[j].x, keys[i].m_translations[j].y, keys[i].m_translations[j].z);
				//m_keys[i].m_pBones[j]->m_rotation = XMFLOAT3(keys[i].m_rotations[j].x, keys[i].m_rotations[j].y, keys[i].m_rotations[j].z);

				m_keys[i].m_boneIdx[j]		= keys[i].m_boneIdx[j];
				m_keys[i].m_translations[j] = keys[i].m_translations[j];
				m_keys[i].m_rotations[j]	= keys[i].m_rotations[j];


				m_boneReferenceInfo[keys[i].m_boneIdx[j]].idxOfKeySet.insert(i);
			}
		}
	}

	/*각 bone들의 toWorld 변환행렬을 생성해서 넣어주는 함수*/
	void		GenerateToWorldMatrix(float time) {
		for (int i = 0; i < m_bones.size(); ++i) {
			XMStoreFloat4x4(&m_bones[i].m_Local, GetInterpolatedLocalMatrix(i, time));
			m_bones[i].MakeToWorldMatrix();
		}
	}
	XMMATRIX	GetFinalMatrix(int boneIdx) {
		XMMATRIX tmp = XMMatrixMultiply(XMLoadFloat4x4(&m_bones[boneIdx].m_toDressposeInverse), XMLoadFloat4x4(&m_bones[boneIdx].m_toWorld));
		return XMMatrixMultiply(tmp, compensatingMatrix);
		//return XMMatrixMultiply(XMLoadFloat4x4(&m_bones[boneIdx].m_toDressposeInverse), XMLoadFloat4x4(&m_bones[boneIdx].m_toWorld));
	}

private:
	XMMATRIX	GetInterpolatedLocalMatrix(int boneIdx, float time) {
		if (boneIdx == 5) {
			cout << "a";
		}



		if (m_boneReferenceInfo[boneIdx].idxOfKeySet.size() == 0) return XMMatrixIdentity();
		if (isFurtherThanBack(time)) if (isLoop) time = GetClampTime(time);

		int curKeyIdx, nextKeyIdx;
		GetKeyCurIdxNextIdxFromRefInfo(boneIdx, time, curKeyIdx, nextKeyIdx);

		int boneIdxInCurKey = GetBoneIdxInKey(boneIdx, curKeyIdx);
		int boneIdxInNextKey = GetBoneIdxInKey(boneIdx, nextKeyIdx);

		/* 4번 키의 4번 본은 2개를 갖고 있어서 1을 반환했는데
		m_keys.front()는 1개만 갖고 있어서 터지는 중.*/





		if (m_keys.size() == 1 || isFurtherThanFront(time))
		{
			AnimationBone temp;
			temp.m_translation = XMFLOAT3(m_keys.front().m_translations[boneIdxInCurKey].x, m_keys.front().m_translations[boneIdxInCurKey].y, m_keys.front().m_translations[boneIdxInCurKey].z);
			temp.m_rotation = XMFLOAT3(m_keys.front().m_rotations[boneIdxInCurKey].x, m_keys.front().m_rotations[boneIdxInCurKey].y, m_keys.front().m_rotations[boneIdxInCurKey].z);
			return temp.GetLocalMatrix();

			//m_keys.front().m_pBones[boneIdxInCurKey]->m_translation = XMFLOAT3(m_keys.front().m_translations[boneIdxInCurKey].x, m_keys.front().m_translations[boneIdxInCurKey].y, m_keys.front().m_translations[boneIdxInCurKey].z);
			//m_keys.front().m_pBones[boneIdxInCurKey]->m_rotation = XMFLOAT3(m_keys.front().m_rotations[boneIdxInCurKey].x, m_keys.front().m_rotations[boneIdxInCurKey].y, m_keys.front().m_rotations[boneIdxInCurKey].z);
			//return m_keys.front().m_pBones[boneIdxInCurKey]->GetLocalMatrix();
		}

		time = GetNormalizedTimeByTwoKeys(time, curKeyIdx, nextKeyIdx);

		XMFLOAT3 tmpFloat3;
		tmpFloat3 = XMFLOAT3(m_keys[curKeyIdx].m_translations[boneIdxInCurKey].x, m_keys[curKeyIdx].m_translations[boneIdxInCurKey].y, m_keys[curKeyIdx].m_translations[boneIdxInCurKey].z);
		XMVECTOR t0 = XMLoadFloat3(&tmpFloat3);
		tmpFloat3 = XMFLOAT3(m_keys[nextKeyIdx].m_translations[boneIdxInNextKey].x, m_keys[nextKeyIdx].m_translations[boneIdxInNextKey].y, m_keys[nextKeyIdx].m_translations[boneIdxInNextKey].z);
		XMVECTOR t1 = XMLoadFloat3(&tmpFloat3);

		//t0 *= 0;
		//t1 *= 0;


		XMVECTOR q0 = {
			XMConvertToRadians(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey].x),
			XMConvertToRadians(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey].y),
			XMConvertToRadians(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey].z) };
		XMVECTOR q1 = { 
			XMConvertToRadians(m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey].x),
			XMConvertToRadians(m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey].y),
			XMConvertToRadians(m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey].z) };
		//XMVECTOR q0 = {
		//	m_keys[curKeyIdx].m_rotations[boneIdxInCurKey].x,
		//	m_keys[curKeyIdx].m_rotations[boneIdxInCurKey].y,
		//	m_keys[curKeyIdx].m_rotations[boneIdxInCurKey].z };
		//XMVECTOR q1 = {
		//	m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey].x,
		//	m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey].y,
		//	m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey].z };
		q0 = XMQuaternionRotationRollPitchYawFromVector(q0);
		q1 = XMQuaternionRotationRollPitchYawFromVector(q1);

		XMFLOAT3 rotateOriginTemp;
		XMVECTOR rotateOrigin;
		if (m_bones[boneIdx].m_parent)	rotateOriginTemp = m_bones[boneIdx].m_parent->GetToWorldPosition();
		else							rotateOriginTemp = m_bones[boneIdx].GetToDressposePosition();
		rotateOrigin = XMLoadFloat3(&rotateOriginTemp);

		return XMMatrixAffineTransformation(
			XMVectorSplatOne(),
			rotateOrigin,
			XMQuaternionSlerp(q0, q1, time),
			XMVectorLerp(t0, t1, time));
	}
	bool		isFurtherThanBack(float time) { return (time >= m_keys.back().m_keytime); }
	bool		isFurtherThanFront(float time) { return (time <= m_keys.front().m_keytime); }
	float		GetClampTime(float time) { return (time - ((int)(time / m_keys.back().m_keytime) * m_keys.back().m_keytime)); }
	int			GetKeyIdxByTime(float time) {
		for (auto p = m_keys.cbegin(); p != m_keys.cend(); ++p) 
			if (time < p->m_keytime) return distance(m_keys.cbegin(), p); 
	}
	int			GetBoneIdxInKey(int boneIdx, int keyIdx) {
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
				//if (p == m_boneReferenceInfo[boneIdx].idxOfKeySet.cend())	nextIdx = curIdx;
				//else														nextIdx = *p;
				return;
			}
		}
		curIdx = *m_boneReferenceInfo[boneIdx].idxOfKeySet.crbegin();
		nextIdx = curIdx;
		/* for Error Check */
		//curIdx = -1;
		//nextIdx = -1;
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
	XMMATRIX						compensatingMatrix = {
	0, 0, -1, 0,
	0, -1, 0, 0,
	1, 0, 0, 0,
	0, 0, 0, 1
	};
};


//class Anim {
//public:	// 이것만 보여주면 됨
//	// 하기 전에 한 번 불러주세여~~~
//	void GenerateToWorld(float time) {
//		for (int i = 0; i < nBones; ++i) {
//			XMStoreFloat4x4(&keyList[0]->boneList[i]->Local, GetInterpolatedLocalMatrix(i, time));
//			//if (i < 2) XMStoreFloat4x4(&keyList[0]->boneList[i]->Local, XMMatrixIdentity());
//			keyList[0]->boneList[i]->MakeToWorldMatrix();
//		}
//	}
//
//	// 이건 행렬마다 매번 부르기
//	XMMATRIX GetFinalMatrix(int boneIdx) {
//#ifdef TEST_
//		//return XMMatrixIdentity();
//		//return XMMatrixIdentity();
//		//if(boneIdx!=0) return XMMatrixIdentity();
//		//else return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toDressposeInverse);
//
//#endif
//
//
//		XMMATRIX finalMatrix = XMMatrixMultiply(
//			XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toDressposeInverse),
//			XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toWorld));
//
//#ifdef TEST_
//		return XMMatrixMultiply(finalMatrix, a);
//#endif
//		return finalMatrix;
//	}
//
//private:	// 이건 이 클래스 안에서만 쓸거야
//	XMMATRIX GetInterpolatedLocalMatrix(int boneIdx, float time) {
//		if (nKeys == 1) return keyList[0]->boneList[boneIdx]->GetLocalMatrix();
//
//		if (IsFurtherThanBack(time)) {
//			if (isLoop) time = ClampTime(time);
//			else		return keyList[nKeys - 1]->boneList[boneIdx]->GetLocalMatrix();
//		}
//		if (IsFurtherThanFront(time)) return keyList[0]->boneList[boneIdx]->GetLocalMatrix();
//
//		int n = 0;
//		for (int i = 0; i < nKeys; ++i) if (time < keyList[i]->keyTime) { n = i - 1; break; }
//
//		time = (time - keyList[n]->keyTime) / (keyList[n + 1]->keyTime - keyList[n]->keyTime);
//
//		XMVECTOR t0 = XMLoadFloat3(&keyList[n]->boneList[boneIdx]->Pos);
//		XMVECTOR t1 = XMLoadFloat3(&keyList[n + 1]->boneList[boneIdx]->Pos);
//
//		//XMVECTOR t0 = { 0,0,0,1 };
//		//XMVECTOR t1 = { 0,0,0,1 };
//		/*XMVECTOR interpolatedT = XMVectorLerp(t0, t1, time);*/
//
//		// 회전
//		XMVECTOR q0 = { XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.x), XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.y), XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.z) };
//		XMVECTOR q1 = { XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.x), XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.y), XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.z) };
//		q0 = XMQuaternionRotationRollPitchYawFromVector(q0);
//		q1 = XMQuaternionRotationRollPitchYawFromVector(q1);
//		/*XMVECTOR interpolatedR = XMQuaternionSlerp(q0, q1, time);*/
//		XMFLOAT3 rotateOriginTemp;
//		XMVECTOR rotateOrigin;
//		if (boneIdx != 0) {
//			rotateOriginTemp = XMFLOAT3(
//				keyList[0]->boneList[boneIdx - 1]->toWorld._41,
//				keyList[0]->boneList[boneIdx - 1]->toWorld._42,
//				keyList[0]->boneList[boneIdx - 1]->toWorld._43
//			);
//			rotateOrigin = XMLoadFloat3(&rotateOriginTemp);
//		}
//		else {
//			rotateOriginTemp = XMFLOAT3(
//				keyList[0]->boneList[boneIdx]->toDresspose._41,
//				keyList[0]->boneList[boneIdx]->toDresspose._42,
//				keyList[0]->boneList[boneIdx]->toDresspose._43
//			);
//			rotateOrigin = XMLoadFloat3(&rotateOriginTemp);
//		}
//
//		// 아핀변환
//		return XMMatrixAffineTransformation(XMVectorSplatOne(), rotateOrigin, XMQuaternionSlerp(q0, q1, time), XMVectorLerp(t0, t1, time));
//
//	}
//	// helper
//	bool IsFurtherThanFront(float time)
//	{
//		return (time <= keyList[0]->keyTime);
//	}
//	bool IsFurtherThanBack(float time)
//	{
//		return (time >= keyList[nKeys - 1]->keyTime);
//	}
//	float ClampTime(float time)
//	{
//		return (time - ((int)(time / keyList[nKeys - 1]->keyTime) * keyList[nKeys - 1]->keyTime));
//	}
//
//public:
//	Anim();
//	~Anim();
//
//public:
//	CKey * * keyList;
//	int nKeys;
//
//	int nBones;
//
//	bool isLoop = true;
//
//	// y축으로 -90도 회전 후 y축 반전!
//	XMMATRIX a = {
//		0, 0, -1, 0,
//		0, -1, 0, 0,
//		1, 0, 0, 0,
//		0, 0, 0, 1
//	};
//};
