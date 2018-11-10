#pragma once

struct CB_ANIMDATA_INFO {
	XMFLOAT4X4 interpolatedMatrix;
};

inline double GetTime(__int64 int64time) {
	__int64 i64 = int64time * 0.01;
	double i64d = i64 / 30790772.0;
	return (i64d / 30.0);
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

	// 로컬 값을 로컬 행렬로
	// Offset 행렬을 구할 때 쓰임?
	void MakeLocalMatrix() {
		XMStoreFloat4x4(&local, XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));
		local._41 = position.x;
		local._42 = position.y;
		local._43 = position.z;
	}

	// 이걸 하기 전에 이미 로컬 값이 들어가있는 부모 본 포인터가 있어야 함
	void MakeToParentMatrix() {
		if (!parent) {
			XMStoreFloat4x4(&toParent, XMMatrixIdentity());
			return;
		}
		XMFLOAT3 toParentTrans;
		XMFLOAT3 toParentRotate;
		toParentTrans.x = parent->position.x - position.x;
		toParentTrans.y = parent->position.y - position.y;
		toParentTrans.z = parent->position.z - position.z;
		toParentRotate.x = parent->rotation.x - rotation.x;
		toParentRotate.y = parent->rotation.y - rotation.y;
		toParentRotate.z = parent->rotation.z - rotation.z;
		XMStoreFloat4x4(&toParent, XMMatrixRotationRollPitchYaw(toParentRotate.x, toParentRotate.y, toParentRotate.z));
		toParent._41 = toParentTrans.x;
		toParent._42 = toParentTrans.y;
		toParent._43 = toParentTrans.z;
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

	// 로컬 행렬
	XMFLOAT4X4 local;	

	// 이게 아마 모든 부모 변환 다 합친 행렬일 것
	XMFLOAT4X4 offset;	

	// 로컬 값
	// 처음 초기화할 때만 쓰자
	XMFLOAT3 position;
	XMFLOAT3 rotation;
};

struct CKey {
	float keyTime;

	CBone ** boneList;
	int nBones;
};

class Anim {
public:
	// 몇 번째 본의 로컬 행렬 보간 내용을 주는 함수
	XMMATRIX InterpolateLocalMatrix(int boneIdx, float time) {
		// nBone이 1개면 그냥 바로 반환
		if (nKeys == 1) return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->local);

		// time이 0과 마지막 키프레임 사이인지 확인
		if (time <= keyList[0]->keyTime) return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->local);

		//if (isLoop) {
		//	if (time >= keyList[nKeys - 1]->keyTime)
		//}
		//else {
		//	if (time >= keyList[nKeys - 1]->keyTime) return XMLoadFloat4x4(&keyList[nKeys - 1]->boneList[boneIdx]->local);
		//}
		if (time >= keyList[nKeys - 1]->keyTime) return XMLoadFloat4x4(&keyList[nKeys - 1]->boneList[boneIdx]->local);
		// time이 n과 n+1 키프레임 사이면 그 둘의 회전과 이동 변환을 보간한 행렬을 만들어야 함
		// 해당 n을 찾아야 할 것
		int n = 0;
		for (int i = 0; i < nKeys; ++i) if (time < keyList[i]->keyTime) { n = i - 1; break; }

		// time을 0과 1 사이로 만들어야 함(그래야 보간을 하니까)
		time = (time - keyList[n]->keyTime) / (keyList[n + 1]->keyTime - keyList[n]->keyTime);


		// 근데 쓰는건 로컬 값인데 뭔가 잘못된 것이 아닐까요?

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

	XMMATRIX OffsetXtoParentXLocalMatrix(int boneIdx, float time) {
		// offset과 toParent는 어떤 키든지 본마다 똑같을테니까 [0]으로
		XMMATRIX temp = XMMatrixMultiply(XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->offset), XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toParent));
		return XMMatrixMultiply(temp, InterpolateLocalMatrix(boneIdx, time));
	}

public:
	Anim();
	~Anim();

public:
	CKey * * keyList;
	int nKeys;

	int nBones;

	bool isLoop = true;
};
