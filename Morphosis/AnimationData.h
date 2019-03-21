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

class AnimationBone {
public:
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

private:
	void MakeToParentMatrix() {
		if (!m_parent) { XMStoreFloat4x4(&m_toParent, XMMatrixIdentity()); return; }
		XMFLOAT3 toParentTrans = XMFLOAT3(m_parent->m_translation.x - m_translation.x, m_parent->m_translation.y - m_translation.y, m_parent->m_translation.z - m_translation.z);
		XMFLOAT3 toParentRotate = XMFLOAT3(m_parent->m_translation.x - m_translation.x, m_parent->m_translation.y - m_translation.y, m_parent->m_translation.z - m_translation.z);

		XMStoreFloat4x4(&m_toParent, XMMatrixRotationRollPitchYaw(XMConvertToRadians(toParentRotate.x), XMConvertToRadians(toParentRotate.y), XMConvertToRadians(toParentRotate.z)));
		m_toParent._41 = toParentTrans.x;
		m_toParent._42 = toParentTrans.y;
		m_toParent._43 = toParentTrans.z;
	}
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
};

//class AnimationKeyframe {
//	
//public:
//	XMFLOAT3 translation;
//	XMFLOAT3 rotation;
//};

class AnimationKey {
public:
	std::vector<AnimationBone>	m_bones;
	float						m_keytime;
};

class AnimationData {
public:
	/*각 bone들의 toWorld 변환행렬을 생성해서 넣어주는 함수*/
	void GenerateToWorldMatrix(float time) {
		for (int i = 0; i < keys.size(); ++i) {
			XMStoreFloat4x4(&keys.front().m_bones[i].m_Local, GetInterpolatedLocalMatrix(i, time));
			keys.front().m_bones[i].MakeToWorldMatrix();
		}
	}
	XMMATRIX GetFinalMatrix(int boneIdx) {
		XMMATRIX finalMatrix = XMMatrixMultiply(
			XMLoadFloat4x4(&keys.front().m_bones[boneIdx].m_toDressposeInverse),
			XMLoadFloat4x4(&keys.front().m_bones[boneIdx].m_toWorld));
		return XMMatrixMultiply(finalMatrix, compensatingMatrix);
	}

private:
	XMMATRIX GetInterpolatedLocalMatrix(int boneIdx, float time) {
		if (keys.size() == 1) return keys[0].m_bones[boneIdx].GetLocalMatrix();

		if (isFurtherThanBack(time)) {
			if (isLoop) time = GetClampTime(time);
			else return keys.back().m_bones[boneIdx].GetLocalMatrix();
		}
		if (isFurtherThanFront(time)) return keys.front().m_bones[boneIdx].GetLocalMatrix();

		int keyIdx = GetKeyIdxByTime(time);
		GetNormalizedTime(time, keyIdx);

		XMVECTOR t0 = XMLoadFloat3(&keys[keyIdx].m_bones[boneIdx].m_translation);
		XMVECTOR t1 = XMLoadFloat3(&keys[keyIdx + 1].m_bones[boneIdx].m_translation);

		XMVECTOR q0 = {
			XMConvertToRadians(keys[keyIdx].m_bones[boneIdx].m_rotation.x),
			XMConvertToRadians(keys[keyIdx].m_bones[boneIdx].m_rotation.y),
			XMConvertToRadians(keys[keyIdx].m_bones[boneIdx].m_rotation.z) };
		XMVECTOR q1 = { 
			XMConvertToRadians(keys[keyIdx+1].m_bones[boneIdx].m_rotation.x),
			XMConvertToRadians(keys[keyIdx+1].m_bones[boneIdx].m_rotation.y),
			XMConvertToRadians(keys[keyIdx+1].m_bones[boneIdx].m_rotation.z) };
		q0 = XMQuaternionRotationRollPitchYawFromVector(q0);
		q1 = XMQuaternionRotationRollPitchYawFromVector(q1);

		XMFLOAT3 rotateOriginTemp;
		XMVECTOR rotateOrigin;
		if (boneIdx != 0)	rotateOriginTemp = keys.front().m_bones[boneIdx].m_parent->GetToWorldPosition();
		else				rotateOriginTemp = keys.front().m_bones[boneIdx].GetToDressposePosition();
		rotateOrigin = XMLoadFloat3(&rotateOriginTemp);

		return XMMatrixAffineTransformation(
			XMVectorSplatOne(),
			rotateOrigin,
			XMQuaternionSlerp(q0, q1, time),
			XMVectorLerp(t0, t1, time));
	}
	bool isFurtherThanBack(float time) { return (time <= keys.back().m_keytime); }
	bool isFurtherThanFront(float time) { return (time >= keys.front().m_keytime); }
	float GetClampTime(float time) { return (time - ((int)(time / keys.back().m_keytime) * keys.back().m_keytime)); }
	int GetKeyIdxByTime(float time) {
		for (auto p = keys.cbegin(); p != keys.cend(); ++p) 
			if (time < p->m_keytime) return distance(keys.cbegin(), p); 
	}
	void GetNormalizedTime(float& time, int keyIdx) {
		time = (time - keys[keyIdx].m_keytime) / (keys[keyIdx + 1].m_keytime - keys[keyIdx].m_keytime);
	}

public:
	AnimationData() = default;
	AnimationData() {}

public:
	std::vector<AnimationKey>	keys;
	bool						isLoop;
	XMMATRIX					compensatingMatrix = {
	0, 0, -1, 0,
	0, -1, 0, 0,
	1, 0, 0, 0,
	0, 0, 0, 1
	};
};


class Anim {
public:	// 이것만 보여주면 됨
	// 하기 전에 한 번 불러주세여~~~
	void GenerateToWorld(float time) {
		for (int i = 0; i < nBones; ++i) {
			XMStoreFloat4x4(&keyList[0]->boneList[i]->Local, GetInterpolatedLocalMatrix(i, time));
			//if (i < 2) XMStoreFloat4x4(&keyList[0]->boneList[i]->Local, XMMatrixIdentity());
			keyList[0]->boneList[i]->MakeToWorldMatrix();
		}
	}

	// 이건 행렬마다 매번 부르기
	XMMATRIX GetFinalMatrix(int boneIdx) {
#ifdef TEST_
		//return XMMatrixIdentity();
		//return XMMatrixIdentity();
		//if(boneIdx!=0) return XMMatrixIdentity();
		//else return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toDressposeInverse);

#endif


		XMMATRIX finalMatrix = XMMatrixMultiply(
			XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toDressposeInverse),
			XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toWorld));

#ifdef TEST_
		return XMMatrixMultiply(finalMatrix, a);
#endif
		return finalMatrix;
	}

private:	// 이건 이 클래스 안에서만 쓸거야
	XMMATRIX GetInterpolatedLocalMatrix(int boneIdx, float time) {
		if (nKeys == 1) return keyList[0]->boneList[boneIdx]->GetLocalMatrix();

		if (IsFurtherThanBack(time)) {
			if (isLoop) time = ClampTime(time);
			else		return keyList[nKeys - 1]->boneList[boneIdx]->GetLocalMatrix();
		}
		if (IsFurtherThanFront(time)) return keyList[0]->boneList[boneIdx]->GetLocalMatrix();

		int n = 0;
		for (int i = 0; i < nKeys; ++i) if (time < keyList[i]->keyTime) { n = i - 1; break; }

		time = (time - keyList[n]->keyTime) / (keyList[n + 1]->keyTime - keyList[n]->keyTime);

		XMVECTOR t0 = XMLoadFloat3(&keyList[n]->boneList[boneIdx]->Pos);
		XMVECTOR t1 = XMLoadFloat3(&keyList[n + 1]->boneList[boneIdx]->Pos);

		//XMVECTOR t0 = { 0,0,0,1 };
		//XMVECTOR t1 = { 0,0,0,1 };
		/*XMVECTOR interpolatedT = XMVectorLerp(t0, t1, time);*/

		// 회전
		XMVECTOR q0 = { XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.x), XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.y), XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.z) };
		XMVECTOR q1 = { XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.x), XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.y), XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.z) };
		q0 = XMQuaternionRotationRollPitchYawFromVector(q0);
		q1 = XMQuaternionRotationRollPitchYawFromVector(q1);
		/*XMVECTOR interpolatedR = XMQuaternionSlerp(q0, q1, time);*/
		XMFLOAT3 rotateOriginTemp;
		XMVECTOR rotateOrigin;
		if (boneIdx != 0) {
			rotateOriginTemp = XMFLOAT3(
				keyList[0]->boneList[boneIdx - 1]->toWorld._41,
				keyList[0]->boneList[boneIdx - 1]->toWorld._42,
				keyList[0]->boneList[boneIdx - 1]->toWorld._43
			);
			rotateOrigin = XMLoadFloat3(&rotateOriginTemp);
		}
		else {
			rotateOriginTemp = XMFLOAT3(
				keyList[0]->boneList[boneIdx]->toDresspose._41,
				keyList[0]->boneList[boneIdx]->toDresspose._42,
				keyList[0]->boneList[boneIdx]->toDresspose._43
			);
			rotateOrigin = XMLoadFloat3(&rotateOriginTemp);
		}

		// 아핀변환
		return XMMatrixAffineTransformation(XMVectorSplatOne(), rotateOrigin, XMQuaternionSlerp(q0, q1, time), XMVectorLerp(t0, t1, time));

	}
	// helper
	bool IsFurtherThanFront(float time)
	{
		return (time <= keyList[0]->keyTime);
	}
	bool IsFurtherThanBack(float time)
	{
		return (time >= keyList[nKeys - 1]->keyTime);
	}
	float ClampTime(float time)
	{
		return (time - ((int)(time / keyList[nKeys - 1]->keyTime) * keyList[nKeys - 1]->keyTime));
	}

public:
	Anim();
	~Anim();

public:
	CKey * * keyList;
	int nKeys;

	int nBones;

	bool isLoop = true;

	// y축으로 -90도 회전 후 y축 반전!
	XMMATRIX a = {
		0, 0, -1, 0,
		0, -1, 0, 0,
		1, 0, 0, 0,
		0, 0, 0, 1
	};
};
