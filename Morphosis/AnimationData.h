#pragma once
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

struct CBone {
public:
	XMMATRIX GetLocalMatrix() {
		XMMATRIX rotM = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rot.x), XMConvertToRadians(Rot.y), XMConvertToRadians(Rot.z));
		XMMATRIX trsM = XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
		return XMMatrixMultiply(trsM, rotM);
	}
	void Initialize(XMFLOAT3 LclTrans, XMFLOAT3 LclRotate, CBone*parent) {
		
		//부모 주소 줌
		this->parent = parent;

		//toParent 구함
		MakeToParentMatrix(LclTrans, LclRotate);

		//toDresspose 구함
		MakeToDressposeMatrix();

		//toDressposeInverse 구함
		MakeToDressposeInverseMatrix();
	}
	void MakeToParentMatrix() {
		if (!parent) { XMStoreFloat4x4(&toParent, XMMatrixIdentity()); return; }
		XMFLOAT3 toParentTrans = XMFLOAT3(parent->Pos.x - Pos.x, parent->Pos.y - Pos.y, parent->Pos.z - Pos.z);
		XMFLOAT3 toParentRotate = XMFLOAT3(parent->Rot.x - Rot.x, parent->Rot.y - Rot.y, parent->Rot.z - Rot.z);

		XMStoreFloat4x4(&toParent, XMMatrixRotationRollPitchYaw(XMConvertToRadians(toParentRotate.x), XMConvertToRadians(toParentRotate.y), XMConvertToRadians(toParentRotate.z)));
		toParent._41 = toParentTrans.x;
		toParent._42 = toParentTrans.y;
		toParent._43 = toParentTrans.z;
	}
	void MakeToParentMatrix(XMFLOAT3 LclTrans, XMFLOAT3 LclRotate) {
		XMStoreFloat4x4(&toParent, XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(LclRotate.x), 
			XMConvertToRadians(LclRotate.y), 
			XMConvertToRadians(LclRotate.z)
		));
		toParent._41 = LclTrans.x;
		toParent._42 = LclTrans.y;
		toParent._43 = LclTrans.z;
	}
	void MakeToDressposeMatrix() {
		if (!parent) XMStoreFloat4x4(&toDresspose, XMMatrixMultiply(XMLoadFloat4x4(&toParent), XMMatrixIdentity()));
		else XMStoreFloat4x4(&toDresspose, XMMatrixMultiply(XMLoadFloat4x4(&toParent), XMLoadFloat4x4(&parent->toDresspose)));
	}
	void MakeToDressposeInverseMatrix() {
		XMVECTOR det;
		det = XMMatrixDeterminant(XMLoadFloat4x4(&toDresspose));
		XMStoreFloat4x4(&toDressposeInverse, XMMatrixInverse(&det, XMLoadFloat4x4(&toDresspose)));
	}
	void MakeToWorldMatrix() {
		XMMATRIX ToWorld;
		if (!parent) ToWorld = XMMatrixMultiply(XMLoadFloat4x4(&Local), XMMatrixMultiply(XMLoadFloat4x4(&toParent), XMMatrixIdentity()));
		else ToWorld = XMMatrixMultiply(XMLoadFloat4x4(&Local), XMMatrixMultiply(XMLoadFloat4x4(&toParent), XMLoadFloat4x4(&parent->toWorld)));
		XMStoreFloat4x4(&toWorld, ToWorld);
	}
public:
	CBone() = default;
	CBone(const CBone& other) {
		parent = other.parent;
		toParent = other.toParent;
		toDresspose = other.toDresspose;
		toDressposeInverse = other.toDressposeInverse;
	}
public:
	CBone * parent = NULL;
	XMFLOAT3 Pos;
	XMFLOAT3 Rot;
	XMFLOAT4X4 toParent;
	XMFLOAT4X4 toDresspose;
	XMFLOAT4X4 toDressposeInverse;
	XMFLOAT4X4 toWorld;
	XMFLOAT4X4 Local;
};

struct CKey {
	float keyTime;

	CBone ** boneList;
	int nBones;
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
		XMMATRIX finalMatrix = XMMatrixMultiply(
			XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toDressposeInverse),
			XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toWorld));
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

		//XMVECTOR t0 = XMLoadFloat3(&keyList[n]->boneList[boneIdx]->Pos);
		//XMVECTOR t1 = XMLoadFloat3(&keyList[n + 1]->boneList[boneIdx]->Pos);

		XMVECTOR t0 = { 0,0,0,1 };
		XMVECTOR t1 = { 0,0,0,1 };
		/*XMVECTOR interpolatedT = XMVectorLerp(t0, t1, time);*/

		// 회전
		XMVECTOR q0 = { XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.x), XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.y), XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.z) };
		XMVECTOR q1 = { XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.x), XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.y), XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.z) };
		q0 = XMQuaternionRotationRollPitchYawFromVector(q0);
		q1 = XMQuaternionRotationRollPitchYawFromVector(q1);
		/*XMVECTOR interpolatedR = XMQuaternionSlerp(q0, q1, time);*/

		// 아핀변환
		return XMMatrixAffineTransformation(XMVectorSplatOne(), XMVectorZero(), XMQuaternionSlerp(q0, q1, time), XMVectorLerp(t0, t1, time));

	}
	// helper
	bool IsFurtherThanFront(float time) 
	{ return (time <= keyList[0]->keyTime); }
	bool IsFurtherThanBack(float time) 
	{ return (time >= keyList[nKeys - 1]->keyTime); }
	float ClampTime(float time) 
	{ return (time - ((int)(time / keyList[nKeys - 1]->keyTime) * keyList[nKeys - 1]->keyTime)); }

public:
	Anim();
	~Anim();

public:
	CKey * * keyList;
	int nKeys;

	int nBones;

	bool isLoop = true;
};
