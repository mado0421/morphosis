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

inline void Print(const XMFLOAT4X4 &m) {
	cout.setf(ios::fixed, ios::floatfield);
	cout << setw(10) << m._11 << " " << setw(10) << m._12 << " " << setw(10) << m._13 << " " << setw(10) << m._14 << "\n";
	cout << setw(10) << m._21 << " " << setw(10) << m._22 << " " << setw(10) << m._23 << " " << setw(10) << m._24 << "\n";
	cout << setw(10) << m._31 << " " << setw(10) << m._32 << " " << setw(10) << m._33 << " " << setw(10) << m._34 << "\n";
	cout << setw(10) << m._41 << " " << setw(10) << m._42 << " " << setw(10) << m._43 << " " << setw(10) << m._44 << "\n\n";
}
inline void Print(const XMMATRIX & m) {
	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, m);
	Print(temp);
}
inline void Print(const XMFLOAT4 &v) {
	cout.setf(ios::fixed, ios::floatfield);
	cout << setw(10) << v.x << " " << setw(10) << v.y << " " << setw(10) << v.z << " " << setw(10) << v.w << "\n\n";
}
inline void Print(const XMVECTOR &v) {
	XMFLOAT4 temp;
	XMStoreFloat4(&temp, v);
	Print(temp);
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
	//// Offset ����� ���ϱ� ���� ����Լ�
	//XMMATRIX GetOffsetMatrix(CBone * p) {
	//	//if (p->parent == NULL) return XMLoadFloat4x4(&p->local);
	//	//XMMATRIX temp = XMMatrixMultiply(GetOffsetMatrix(p->parent), XMLoadFloat4x4(&toParent));
	//	//return  XMMatrixMultiply(temp, XMLoadFloat4x4(&local));
	//}
	//void MakeOffsetMatrix() {
	//	//XMStoreFloat4x4(&offset, XMMatrixMultiply(GetOffsetMatrix(this), XMMatrixMultiply(XMLoadFloat4x4(&toParent), XMLoadFloat4x4(&local))));
	//	//XMStoreFloat4x4(&offset, XMMatrixMultiply(GetOffsetMatrix(this), XMLoadFloat4x4(&local)));
	//}
	//// ���� ���� ���� ��ķ�
	//// Offset ����� ���� �� ����?
	//void MakeLocalMatrix() {
	//	//XMStoreFloat4x4(&local, XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));
	//	//local._41 = position.x;
	//	//local._42 = position.y;
	//	//local._43 = position.z;
	//}


public:
	XMMATRIX GetLocalMatrix() {
		XMMATRIX rotM = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rot.x), XMConvertToRadians(Rot.y), XMConvertToRadians(Rot.z));
		XMMATRIX trsM = XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
		return XMMatrixMultiply(trsM, rotM);
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
		XMStoreFloat4x4(&toParent, XMMatrixRotationRollPitchYaw(XMConvertToRadians(LclRotate.x), XMConvertToRadians(LclRotate.y), XMConvertToRadians(LclRotate.z)));
		toParent._41 = LclTrans.x;
		toParent._42 = LclTrans.y;
		toParent._43 = LclTrans.z;
	}
	void MakeToDressposeMatrix() {
		XMVECTOR det;
		det = XMMatrixDeterminant(XMLoadFloat4x4(&toParent));
		XMStoreFloat4x4(&toDresspose, XMMatrixInverse(&det, XMLoadFloat4x4(&toParent)));
	}
	void Initialize(XMFLOAT3 LclTrans, XMFLOAT3 LclRotate, CBone*parent) {
		
		//�θ� �ּ� ��
		this->parent = parent;

		//toParent ����
		MakeToParentMatrix(LclTrans, LclRotate);

		//toDresspose ����
		MakeToDressposeMatrix();
	}
	void MakeToWorldMatrix() {
		XMMATRIX ToWorld;
		if (!parent) ToWorld = XMMatrixMultiply(XMMatrixMultiply(XMLoadFloat4x4(&Local), XMLoadFloat4x4(&toParent)), XMMatrixIdentity());
		else ToWorld = XMMatrixMultiply(XMMatrixMultiply(XMLoadFloat4x4(&Local), XMLoadFloat4x4(&toParent)), XMLoadFloat4x4(&parent->toWorld));
		XMStoreFloat4x4(&toWorld, ToWorld);
	}
	//void Initialize(XMFLOAT3 bindPos, XMFLOAT3 bindRot, CBone*parent) {
	//	//���ε� ���� ����
	//	this->Pos = bindPos;
	//	this->Rot = bindRot;
	//	//�θ� �ּ� ��
	//	this->parent = parent;
	//	//toParent ����
	//	MakeToParentMatrix();
	//	//toDresspose ����
	//	MakeToDressposeMatrix();
	//}
public:
	CBone() = default;
	CBone(const CBone& other) {
		parent = other.parent;
		toParent = other.toParent;
		toDresspose = other.toDresspose;
	}
//public:
//	CBone * parent = NULL;
//	XMFLOAT4X4 toParent;
//
//	// ���� ���
//	XMFLOAT4X4 local;	
//
//	// �̰� �Ƹ� ��� �θ� ��ȯ �� ��ģ ����� ��
//	XMFLOAT4X4 offset;	
//
//	// ���� ��
//	// ó�� �ʱ�ȭ�� ���� ����
//	XMFLOAT3 position;
//	XMFLOAT3 rotation;
public:
	CBone * parent = NULL;
	XMFLOAT3 Pos;
	XMFLOAT3 Rot;
	XMFLOAT4X4 toParent;
	XMFLOAT4X4 toDresspose;
	XMFLOAT4X4 toWorld;
	XMFLOAT4X4 Local;
};

struct CKey {
	float keyTime;

	CBone ** boneList;
	int nBones;
};

class Anim {
public:

	bool IsFurtherThanFront(float time) 
	{ return (time <= keyList[0]->keyTime); }
	bool IsFurtherThanBack(float time) 
	{ return (time >= keyList[nKeys - 1]->keyTime); }
	float ClampTime(float time) 
	{ return (time - ((int)(time / keyList[nKeys - 1]->keyTime) * keyList[nKeys - 1]->keyTime)); }


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
		/*XMVECTOR interpolatedT = XMVectorLerp(t0, t1, time);*/

		// ȸ��
		XMVECTOR q0 = { XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.x), XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.y), XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.z) };
		XMVECTOR q1 = { XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.x), XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.y), XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.z) };
		q0 = XMQuaternionRotationRollPitchYawFromVector(q0);
		q1 = XMQuaternionRotationRollPitchYawFromVector(q1);
		/*XMVECTOR interpolatedR = XMQuaternionSlerp(q0, q1, time);*/

		// ���ɺ�ȯ
		return XMMatrixAffineTransformation(XMVectorSplatOne(), XMVectorZero(), XMQuaternionSlerp(q0, q1, time), XMVectorLerp(t0, t1, time));

	}

	// �� ��° Ű�� ã���� ���ؾ� ��
	// ������ ����� ������ ������ toWorld�� ���ؾ� �ϴµ�?
	// �ð����� �޶����°� local�� local�� ���� ����� toWorld ��
	// parent->toWorld * toParent * local
	// toParent�� Ű�� ���� ������ ����
	// toWorld�� ���� �������� local�� �־�߰ڴ�.
	
	// local�� ���ؼ� keyList[0]->bone[boneIdx]->local�� �ְ�
	// keyList[0]->bone[boneIdx]�� toWorld�� ���ϰ�
	// keyList[0]->bone[boneIdx+1]->local�� �ְ�
	// keyList[0]->bone[boneIdx+1]�� toWorld�� ���ϰ� �ݺ��� ��
	// �׷����� local�� toWorld ����� ���� �־�� �� ��
	
	// �� �� ���鼭 �� toWorld ����� �ְ�
	// �ٸ� �Լ��� �� �����°� ���� ��
	// GenerateToWorld()
	// {
	//     for() MakeLocalMatrix(i);
	//     for() MakeToWorldMatrix(i);
	// }
	// /*�� ��� �ϴ� �κ�(Matrix �ϳ��� �� ��)*/
	// for() GetToWorldMatrix(i);

	// �ϱ� ���� �� �� �ҷ��ּ���~~~
	void GenerateToWorld(float time) {
		for (int i = 0; i < nBones; ++i) {
			XMStoreFloat4x4(&keyList[0]->boneList[i]->Local, GetInterpolatedLocalMatrix(i, time));
			keyList[0]->boneList[i]->MakeToWorldMatrix();
		}
	}

	// �̰� ��ĸ��� �Ź� �θ���
	XMMATRIX GetToWorldMatrix(int boneIdx) {
		return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toWorld);
	}




	//// �� ��° ���� ���� ��� ���� ������ �ִ� �Լ�
	//XMMATRIX InterpolateLocalMatrix(int boneIdx, float time) {
	//	// nBone�� 1���� �׳� �ٷ� ��ȯ
	//	if (nKeys == 1) return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->local);
	//	// time�� 0�� ������ Ű������ �������� Ȯ��
	//	if (time >= keyList[nKeys - 1]->keyTime) {
	//		if (isLoop) time = time - ((int)(time / keyList[nKeys - 1]->keyTime) * keyList[nKeys - 1]->keyTime);
	//		else return XMLoadFloat4x4(&keyList[nKeys - 1]->boneList[boneIdx]->local);
	//	}
	//	if (time <= keyList[0]->keyTime) return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->local);
	//	//if (time >= keyList[nKeys - 1]->keyTime) return XMLoadFloat4x4(&keyList[nKeys - 1]->boneList[boneIdx]->local);
	//	// time�� n�� n+1 Ű������ ���̸� �� ���� ȸ���� �̵� ��ȯ�� ������ ����� ������ ��
	//	// �ش� n�� ã�ƾ� �� ��
	//	int n = 0;
	//	for (int i = 0; i < nKeys; ++i) if (time < keyList[i]->keyTime) { n = i - 1; break; }
	//	// time�� 0�� 1 ���̷� ������ ��(�׷��� ������ �ϴϱ�)
	//	time = (time - keyList[n]->keyTime) / (keyList[n + 1]->keyTime - keyList[n]->keyTime);
	//	// �ٵ� ���°� ���� ���ε� ���� �߸��� ���� �ƴұ��?
	//	// �̵�
	//	XMVECTOR t0 = XMLoadFloat3(&keyList[n]->boneList[boneIdx]->position);
	//	XMVECTOR t1 = XMLoadFloat3(&keyList[n + 1]->boneList[boneIdx]->position);
	//	/*XMVECTOR interpolatedT = XMVectorLerp(t0, t1, time);*/
	//	// ȸ��
	//	XMVECTOR q0 = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&keyList[n]->boneList[boneIdx]->rotation));
	//	XMVECTOR q1 = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&keyList[n + 1]->boneList[boneIdx]->rotation));
	//	/*XMVECTOR interpolatedR = XMQuaternionSlerp(q0, q1, time);*/
	//	// ���ɺ�ȯ
	//	return XMMatrixAffineTransformation(XMVectorSplatOne(), XMVectorZero(), XMQuaternionSlerp(q0, q1, time), XMVectorLerp(t0, t1, time));
	//}





	//XMMATRIX OffsetXtoParentXLocalMatrix(int boneIdx, float time) {
	//	// offset�� toParent�� � Ű���� ������ �Ȱ����״ϱ� [0]����
	//	XMMATRIX temp = XMMatrixMultiply(XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->offset), XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toParent));
	//	return XMMatrixMultiply(temp, InterpolateLocalMatrix(boneIdx, time));
	//}

public:
	Anim();
	~Anim();

public:
	CKey * * keyList;
	int nKeys;

	int nBones;

	bool isLoop = true;
};
