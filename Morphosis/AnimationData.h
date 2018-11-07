#pragma once

inline float GetTime(__int64 int64time) {
	__int64 i64 = int64time * 0.01;
	double i64d = i64 / 30790772.f;
	return (i64d / 30.0f);
}

//struct AnimCurveNode {
//	
//};
//
//struct AnimCurve {
//
//};
//
//struct Key {
//	__int64 bookmark;
//	float time;
//	XMFLOAT3 translation;
//	XMFLOAT3 rotation;
//};
//
//struct Bone {
//public:
//	int FindKeyIdxByBookmark(__int64 v) {
//		for (int i = 0; i < numOfKeys; ++i) {
//			if (keys[i]->bookmark == v) return i;
//		}
//		return -1;	//failed
//	}
//
//public:
//	XMFLOAT3 translation;
//	XMFLOAT3 rotation;
//	Bone* parent;
//	Bone** sibling;
//	int numOfSibling;
//
//	XMFLOAT4X4 bindPose;
//	XMFLOAT4X4 transform;
//	XMFLOAT4X4 transformLink;
//
//	int numOfKeys;
//	Key **keys;
//
//	int numOfVertice;
//	int *idx;
//	float *weights;
//};
//
//class CAnimationData
//{
//public:
//
//
//public:
//	CAnimationData();
//	~CAnimationData();
//
//public:
//	Bone **bones;
//	int numOfBones;
//};
//

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
		parent = other.parent;
		toParent = other.toParent;
		local = other.local;
		offset = other.offset;
		position = other.position;
		rotation = other.rotation;
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
		if (time <= keyList[0]->keyTime) return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->local);
		if (time >= keyList[nKeys - 1]->keyTime) return XMLoadFloat4x4(&keyList[nKeys - 1]->boneList[boneIdx]->local);

		// time이 n과 n+1 키프레임 사이면 그 둘의 회전과 이동 변환을 보간한 행렬을 만들어야 함
		// 해당 n을 찾아야 할 것
		int n = 0;
		for (int i = 0; i < nKeys; ++i) if (time < keyList[i]->keyTime) { n = i - 1; break; }

		// time을 0과 1 사이로 만들어야 함(그래야 보간을 하니까)
		time = (time - keyList[n]->keyTime) / (keyList[n + 1]->keyTime - keyList[n]->keyTime);

		// 이동
		XMVECTOR t0 = XMLoadFloat3(&keyList[n]->boneList[boneIdx]->position);
		XMVECTOR t1 = XMLoadFloat3(&keyList[n + 1]->boneList[boneIdx]->position);
		/*XMVECTOR interpolatedT = XMVectorLerp(t0, t1, time);*/

		// 회전
		XMVECTOR q0 = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&keyList[n]->boneList[boneIdx]->rotation));
		XMVECTOR q1 = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&keyList[n + 1]->boneList[boneIdx]->rotation));
		/*XMVECTOR interpolatedR = XMQuaternionSlerp(q0, q1, time);*/

		// 아핀변환
		return XMMatrixAffineTransformation(XMVectorSplatOne(), XMVectorZero(), XMQuaternionSlerp(q0, q1, time), XMVectorLerp(t0, t1, time));
	}


public:
	CKey * * keyList;
	int nKeys;

	int nBones;
};


class AnimObject {

};