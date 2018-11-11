// ConsoleApplication1.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//
#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <DirectXMath.h>	//<-XMVECTOR, XMMATRIX 등등

using namespace DirectX;
using namespace std;



void Print(const XMFLOAT4X4 &m) {
	cout.setf(ios::fixed, ios::floatfield);
	cout << setw(10) << m._11 << " " << setw(10) << m._12 << " " << setw(10) << m._13 << " " << setw(10) << m._14 << "\n";
	cout << setw(10) << m._21 << " " << setw(10) << m._22 << " " << setw(10) << m._23 << " " << setw(10) << m._24 << "\n";
	cout << setw(10) << m._31 << " " << setw(10) << m._32 << " " << setw(10) << m._33 << " " << setw(10) << m._34 << "\n";
	cout << setw(10) << m._41 << " " << setw(10) << m._42 << " " << setw(10) << m._43 << " " << setw(10) << m._44 << "\n\n";
}
void Print(const XMMATRIX & m) {
	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, m);
	Print(temp);
}
void Print(const XMFLOAT4 &v) {
	cout.setf(ios::fixed, ios::floatfield);
	cout << setw(10) << v.x << " " << setw(10) << v.y << " " << setw(10) << v.z << " " << setw(10) << v.w << "\n\n";
}
void Print(const XMVECTOR &v) {
	XMFLOAT4 temp;
	XMStoreFloat4(&temp, v);
	Print(temp);
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
	XMFLOAT4X4 bindPos;

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
	//Anim a;
	//CBone ** boneList;
	//int nBones = 3;
	//double time = 0;

	//boneList = new CBone*[nBones];

	//{
	//	CBone * pB;
	//	pB = new CBone();
	//	pB->position = XMFLOAT3(0, 0, 0);
	//	pB->rotation = XMFLOAT3(0, 0, 0);
	//	XMStoreFloat4x4(&pB->toParent, XMMatrixIdentity());
	//	pB->parent = NULL;
	//	pB->MakeLocalMatrix();
	//	pB->MakeOffsetMatrix();
	//	boneList[0] = pB;
	//}

	//{
	//	CBone * pB;
	//	pB = new CBone();
	//	pB->position = XMFLOAT3(0, 0, 0);
	//	pB->rotation = XMFLOAT3(0, 0, 0);
	//	XMStoreFloat4x4( &pB->toParent ,XMMatrixIdentity());
	//	pB->toParent._42 = 10;
	//	pB->parent = boneList[0];
	//	pB->MakeLocalMatrix();
	//	pB->MakeOffsetMatrix();
	//	boneList[1] = pB;
	//}

	//{
	//	CBone * pB;
	//	pB = new CBone();
	//	pB->position = XMFLOAT3(0, 0, 0);
	//	pB->rotation = XMFLOAT3(0, 0, 0);
	//	XMStoreFloat4x4(&pB->toParent, XMMatrixIdentity());
	//	pB->toParent._42 = 10;
	//	pB->parent = boneList[1];
	//	pB->MakeLocalMatrix();
	//	pB->MakeOffsetMatrix();
	//	boneList[2] = pB;
	//}

	//a.nBones = nBones;
	//a.nKeys = 2;
	//a.keyList = new CKey*[a.nKeys];

	//for (int i = 0; i < a.nKeys; ++i) {
	//	CKey * pK = new CKey();
	//	pK->nBones = 2;
	//	pK->boneList = new CBone*[pK->nBones];
	//	//a.keyList[i]->boneList = new CBone*[a.keyList[i]->nBones];
	//	for (int j = 0; j < nBones; ++j) { 
	//		pK->boneList[j] = new CBone(*boneList[j]);
	//	}

	//	pK->keyTime = i * 5;
	//	a.keyList[i] = pK;
	//}

	//XMStoreFloat4x4(&a.keyList[1]->boneList[1]->local, XMMatrixIdentity());
	//a.keyList[1]->boneList[1]->local._41 = 10;
	//a.keyList[1]->boneList[1]->position.x = 10;
	//a.keyList[1]->boneList[1]->rotation.x = 90;

	//while (time < 10) {

	//	XMFLOAT4X4 temp;
	//	XMStoreFloat4x4(&temp, a.InterpolateBones(1, time));
	//	PrintMatrix(temp);

	//	Sleep(10);
	//	time += 0.01;
	//}

	//float time = 0 ;
	//float keyTime = 1.6777;
	//while (true) {
	//	Sleep(10);
	//	time += 0.01;
	//	if(time >= keyTime)	time = time - ((int)(time / keyTime) * keyTime);
	//	cout << time << "\n";
	//}

	//XMFLOAT4X4 m1;
	//XMFLOAT4X4 m2;
	
	XMMATRIX b0toParent = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	XMMATRIX b1toParent = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		4,0,0,1
	};
	XMMATRIX b2toParent = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		3,0,0,1
	};

	XMVECTOR det;
	det = XMMatrixDeterminant(b0toParent);
	XMMATRIX b0toDressPos = XMMatrixInverse(&det, b0toParent);
	det = XMMatrixDeterminant(b1toParent);
	XMMATRIX b1toDressPos = XMMatrixInverse(&det, b1toParent);
	det = XMMatrixDeterminant(b2toParent);
	XMMATRIX b2toDressPos = XMMatrixInverse(&det, b2toParent);

	XMMATRIX b0Local = XMMatrixIdentity();
	XMMATRIX b1Local = XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(90));
	XMMATRIX b2Local = XMMatrixRotationRollPitchYaw(0, 0, XMConvertToRadians(90));

	XMMATRIX b0toWorld = XMMatrixIdentity();
	XMMATRIX b1toWorld = XMMatrixMultiply(XMMatrixMultiply(b1Local, b1toParent), b0toWorld);
	XMMATRIX b2toWorld = XMMatrixMultiply(XMMatrixMultiply(b2Local, b2toParent), b1toWorld);

	XMVECTOR vPrev = { 6, 0, 0, 1 };
	XMVECTOR vNext = XMVector4Transform(vPrev, XMMatrixMultiply(b1toDressPos, b1toWorld));

	XMFLOAT4 result;
	XMStoreFloat4(&result, vNext);

	Print(b0toParent);
	Print(b1toParent);
	Print(b2toParent);
	Print(b0toDressPos);
	Print(b1toDressPos);
	Print(b2toDressPos);
	Print(b0Local);
	Print(b1Local);
	Print(b2Local);
	Print(b0toWorld);
	Print(b1toWorld);
	Print(b2toWorld);
	Print(result);

	XMMATRIX rotM = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(0), XMConvertToRadians(0));
	XMMATRIX trsM = XMMatrixTranslation(10, 0, 0);
	Print(XMMatrixMultiply(trsM, rotM));

	XMVECTOR rotV = { XMConvertToRadians(90), 0, 0 };
	XMVECTOR qV = XMQuaternionRotationRollPitchYawFromVector(rotV);
	rotM = XMMatrixRotationQuaternion(qV);
	Print(XMMatrixMultiply(trsM, rotM));


    return 0;
}

