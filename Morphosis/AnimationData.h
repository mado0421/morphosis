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
				keyList[0]->boneList[boneIdx-1]->toWorld._41, 
				keyList[0]->boneList[boneIdx-1]->toWorld._42, 
				keyList[0]->boneList[boneIdx-1]->toWorld._43
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
	Anim(int nKeys, int nBones, vector<CBone>& b, vector<CKey>& k);
	~Anim();


public:
	CKey ** keyList;
	int nKeys;

	int nBones;
	bool isLoop = true;

	vector<CKey> keys;


	// y축으로 -90도 회전 후 y축 반전!
	XMMATRIX a = {
		0, 0, -1, 0,
		0, -1, 0, 0,
		1, 0, 0, 0,
		0, 0, 0, 1
	};
};
