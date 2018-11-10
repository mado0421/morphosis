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
	// Offset ����� ���ϱ� ���� ����Լ�
	XMMATRIX GetOffsetMatrix(CBone * p) {
		if (p->parent == NULL) return XMLoadFloat4x4(&p->local);
		XMMATRIX temp = XMMatrixMultiply(GetOffsetMatrix(p->parent), XMLoadFloat4x4(&toParent));
		return  XMMatrixMultiply(temp, XMLoadFloat4x4(&local));
	}

	void MakeOffsetMatrix() {
		//XMStoreFloat4x4(&offset, XMMatrixMultiply(GetOffsetMatrix(this), XMMatrixMultiply(XMLoadFloat4x4(&toParent), XMLoadFloat4x4(&local))));
		XMStoreFloat4x4(&offset, XMMatrixMultiply(GetOffsetMatrix(this), XMLoadFloat4x4(&local)));

	}

	// ���� ���� ���� ��ķ�
	// Offset ����� ���� �� ����?
	void MakeLocalMatrix() {
		XMStoreFloat4x4(&local, XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));
		local._41 = position.x;
		local._42 = position.y;
		local._43 = position.z;
	}

	// �̰� �ϱ� ���� �̹� ���� ���� ���ִ� �θ� �� �����Ͱ� �־�� ��
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

	// ���� ���
	XMFLOAT4X4 local;	

	// �̰� �Ƹ� ��� �θ� ��ȯ �� ��ģ ����� ��
	XMFLOAT4X4 offset;	

	// ���� ��
	// ó�� �ʱ�ȭ�� ���� ����
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
	// �� ��° ���� ���� ��� ���� ������ �ִ� �Լ�
	XMMATRIX InterpolateLocalMatrix(int boneIdx, float time) {
		// nBone�� 1���� �׳� �ٷ� ��ȯ
		if (nKeys == 1) return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->local);

		// time�� 0�� ������ Ű������ �������� Ȯ��
		if (time <= keyList[0]->keyTime) return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->local);

		//if (isLoop) {
		//	if (time >= keyList[nKeys - 1]->keyTime)
		//}
		//else {
		//	if (time >= keyList[nKeys - 1]->keyTime) return XMLoadFloat4x4(&keyList[nKeys - 1]->boneList[boneIdx]->local);
		//}
		if (time >= keyList[nKeys - 1]->keyTime) return XMLoadFloat4x4(&keyList[nKeys - 1]->boneList[boneIdx]->local);
		// time�� n�� n+1 Ű������ ���̸� �� ���� ȸ���� �̵� ��ȯ�� ������ ����� ������ ��
		// �ش� n�� ã�ƾ� �� ��
		int n = 0;
		for (int i = 0; i < nKeys; ++i) if (time < keyList[i]->keyTime) { n = i - 1; break; }

		// time�� 0�� 1 ���̷� ������ ��(�׷��� ������ �ϴϱ�)
		time = (time - keyList[n]->keyTime) / (keyList[n + 1]->keyTime - keyList[n]->keyTime);


		// �ٵ� ���°� ���� ���ε� ���� �߸��� ���� �ƴұ��?

		// �̵�
		XMVECTOR t0 = XMLoadFloat3(&keyList[n]->boneList[boneIdx]->position);
		XMVECTOR t1 = XMLoadFloat3(&keyList[n + 1]->boneList[boneIdx]->position);
		/*XMVECTOR interpolatedT = XMVectorLerp(t0, t1, time);*/

		// ȸ��
		XMVECTOR q0 = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&keyList[n]->boneList[boneIdx]->rotation));
		XMVECTOR q1 = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&keyList[n + 1]->boneList[boneIdx]->rotation));
		/*XMVECTOR interpolatedR = XMQuaternionSlerp(q0, q1, time);*/

		// ���ɺ�ȯ
		return XMMatrixAffineTransformation(XMVectorSplatOne(), XMVectorZero(), XMQuaternionSlerp(q0, q1, time), XMVectorLerp(t0, t1, time));
	}

	XMMATRIX OffsetXtoParentXLocalMatrix(int boneIdx, float time) {
		// offset�� toParent�� � Ű���� ������ �Ȱ����״ϱ� [0]����
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
