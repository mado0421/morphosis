// ConsoleApplication1.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//
#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <DirectXMath.h>	//<-XMVECTOR, XMMATRIX 등등

using namespace DirectX;
using namespace std;

void PrintMatrix(const XMFLOAT4X4 &m) {
	cout.setf(ios::fixed, ios::floatfield);
	cout << setw(10) << m._11 << " " << setw(10) << m._12 << " " << setw(10) << m._13 << " " << setw(10) << m._14 << "\n";
	cout << setw(10) << m._21 << " " << setw(10) << m._22 << " " << setw(10) << m._23 << " " << setw(10) << m._24 << "\n";
	cout << setw(10) << m._31 << " " << setw(10) << m._32 << " " << setw(10) << m._33 << " " << setw(10) << m._34 << "\n";
	cout << setw(10) << m._41 << " " << setw(10) << m._42 << " " << setw(10) << m._43 << " " << setw(10) << m._44 << "\n";
}

struct CBone {
public:
	// Offset 행렬을 구하기 위한 재귀함수
	XMMATRIX GetOffsetMatrix(CBone * p) {
		if (p->parent == NULL) return XMLoadFloat4x4(&p->local);
		XMMATRIX temp = XMMatrixMultiply(GetOffsetMatrix(p->parent), XMLoadFloat4x4(&toParent));
		return  XMMatrixMultiply(temp, XMLoadFloat4x4(&local));
	}

	void MakeOffsetMatrix() {
		//XMStoreFloat4x4(&offset, XMMatrixMultiply(GetOffsetMatrix(this), XMMatrixMultiply(XMLoadFloat4x4(&toParent), XMLoadFloat4x4(&local))));
		XMStoreFloat4x4(&offset, XMMatrixMultiply(GetOffsetMatrix(this), XMLoadFloat4x4(&local)));

	}

	void MakeLocalMatrix() {
		XMStoreFloat4x4(&local, XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));
		local._41 = position.x;
		local._42 = position.y;
		local._43 = position.z;
	}

public:
	CBone() = default;
	CBone(const CBone& other) {
		parent		= other.parent;
		toParent	= other.toParent;
		local		= other.local;
		offset		= other.offset;
		position	= other.position;
		rotation	= other.rotation;
	}

public:
	CBone * parent = NULL;
	XMFLOAT4X4 toParent;
	XMFLOAT4X4 local;

	XMFLOAT4X4 offset;	//이게 아마 모든 부모 변환 다 합친 행렬일 것

	XMFLOAT3 position;
	XMFLOAT3 rotation;
};

struct CKey {
	double keyTime;

	CBone ** boneList;
	int nBones;
};

class Anim {
public:
	// 몇 번째 본의 로컬 행렬 보간 내용을 주는 함수
	XMMATRIX InterpolateBones(int boneIdx, double time) {
		// nBone이 1개면 그냥 바로 반환
		if (nKeys == 1) return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->local);

		// time이 0과 마지막 키프레임 사이인지 확인
		if (time <= keyList[0]->keyTime) return XMLoadFloat4x4( &keyList[0]->boneList[boneIdx]->local);
		if (time >= keyList[nKeys - 1]->keyTime) return XMLoadFloat4x4(&keyList[nKeys-1]->boneList[boneIdx]->local);

		// time이 n과 n+1 키프레임 사이면 그 둘의 회전과 이동 변환을 보간한 행렬을 만들어야 함
		// 해당 n을 찾아야 할 것
		int n = 0;
		for (int i = 0; i < nKeys; ++i) if (time < keyList[i]->keyTime) { n = i-1; break; }

		// time을 0과 1 사이로 만들어야 함(그래야 보간을 하니까)
		time = (time - keyList[n]->keyTime) / (keyList[n + 1]->keyTime - keyList[n]->keyTime);

		// 이동
		XMVECTOR t0 = XMLoadFloat3(&keyList[n]->boneList[boneIdx]->position);
		XMVECTOR t1 = XMLoadFloat3(&keyList[n+1]->boneList[boneIdx]->position);
		/*XMVECTOR interpolatedT = XMVectorLerp(t0, t1, time);*/

		// 회전
		XMVECTOR q0 = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&keyList[n]->boneList[boneIdx]->rotation));
		XMVECTOR q1 = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&keyList[n+1]->boneList[boneIdx]->rotation));
		/*XMVECTOR interpolatedR = XMQuaternionSlerp(q0, q1, time);*/

		// 아핀변환
		return XMMatrixAffineTransformation(XMVectorSplatOne(), XMVectorZero(), XMQuaternionSlerp(q0, q1, time), XMVectorLerp(t0, t1, time));
	}


public:
	CKey * * keyList;
	int nKeys;

	int nBones;
};

int main()
{
	Anim a;
	CBone ** boneList;
	int nBones = 2;
	double time = 0;

	boneList = new CBone*[nBones];

	{
		CBone * pB;
		pB = new CBone();
		pB->position = XMFLOAT3(0, 0, 0);
		pB->rotation = XMFLOAT3(0, 0, 0);
		XMStoreFloat4x4(&pB->toParent, XMMatrixIdentity());
		pB->parent = NULL;
		pB->MakeLocalMatrix();
		pB->MakeOffsetMatrix();
		boneList[0] = pB;
	}

	{
		CBone * pB;
		pB = new CBone();
		pB->position = XMFLOAT3(0, 0, 0);
		pB->rotation = XMFLOAT3(0, 0, 0);
		XMStoreFloat4x4( &pB->toParent ,XMMatrixIdentity());
		pB->toParent._42 = 10;
		pB->parent = boneList[0];
		pB->MakeLocalMatrix();
		pB->MakeOffsetMatrix();
		boneList[1] = pB;
	}
/*
	{
		CBone * pB;
		pB = new CBone();
		pB->position = XMFLOAT3(0, 0, 0);
		pB->rotation = XMFLOAT3(0, 0, 0);
		XMStoreFloat4x4(&pB->toParent, XMMatrixIdentity());
		pB->toParent._42 = 10;
		pB->parent = boneList[1];
		pB->MakeLocalMatrix();
		pB->MakeOffsetMatrix();
		boneList[2] = pB;
	}*/


	a.nBones = nBones;
	a.nKeys = 2;
	a.keyList = new CKey*[a.nKeys];

	for (int i = 0; i < a.nKeys; ++i) {
		CKey * pK = new CKey();
		pK->nBones = 2;
		pK->boneList = new CBone*[pK->nBones];
		//a.keyList[i]->boneList = new CBone*[a.keyList[i]->nBones];
		for (int j = 0; j < nBones; ++j) { 
			pK->boneList[j] = new CBone(*boneList[j]);
		}

		pK->keyTime = i * 5;
		a.keyList[i] = pK;
	}

	XMStoreFloat4x4(&a.keyList[1]->boneList[1]->local, XMMatrixIdentity());
	a.keyList[1]->boneList[1]->local._41 = 10;
	a.keyList[1]->boneList[1]->position.x = 10;
	a.keyList[1]->boneList[1]->rotation.x = 90;

	while (time < 10) {
		//for (int i = 0; i < nBones; ++i) {
		//	XMFLOAT4X4 temp;
		//	XMStoreFloat4x4(&temp, a.InterpolateBones(i, time));
		//	cout << "CBone #" << i << "\n";
		//	PrintMatrix(temp);
		//	cout << "\n\n";
		//}

		XMFLOAT4X4 temp;
		XMStoreFloat4x4(&temp, a.InterpolateBones(1, time));
		PrintMatrix(temp);

		Sleep(10);
		time += 0.01;
	}








    return 0;
}

