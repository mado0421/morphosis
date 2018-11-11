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
	//// Offset 행렬을 구하기 위한 재귀함수
	//XMMATRIX GetOffsetMatrix(CBone * p) {
	//	//if (p->parent == NULL) return XMLoadFloat4x4(&p->local);
	//	//XMMATRIX temp = XMMatrixMultiply(GetOffsetMatrix(p->parent), XMLoadFloat4x4(&toParent));
	//	//return  XMMatrixMultiply(temp, XMLoadFloat4x4(&local));
	//}
	//void MakeOffsetMatrix() {
	//	//XMStoreFloat4x4(&offset, XMMatrixMultiply(GetOffsetMatrix(this), XMMatrixMultiply(XMLoadFloat4x4(&toParent), XMLoadFloat4x4(&local))));
	//	//XMStoreFloat4x4(&offset, XMMatrixMultiply(GetOffsetMatrix(this), XMLoadFloat4x4(&local)));
	//}
	//// 로컬 값을 로컬 행렬로
	//// Offset 행렬을 구할 때 쓰임?
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
		
		//부모 주소 줌
		this->parent = parent;

		//toParent 구함
		MakeToParentMatrix(LclTrans, LclRotate);

		//toDresspose 구함
		MakeToDressposeMatrix();
	}
	void MakeToWorldMatrix() {
		XMMATRIX ToWorld;
		if (!parent) ToWorld = XMMatrixMultiply(XMMatrixMultiply(XMLoadFloat4x4(&Local), XMLoadFloat4x4(&toParent)), XMMatrixIdentity());
		else ToWorld = XMMatrixMultiply(XMMatrixMultiply(XMLoadFloat4x4(&Local), XMLoadFloat4x4(&toParent)), XMLoadFloat4x4(&parent->toWorld));
		XMStoreFloat4x4(&toWorld, ToWorld);
	}
	//void Initialize(XMFLOAT3 bindPos, XMFLOAT3 bindRot, CBone*parent) {
	//	//바인드 포즈 넣음
	//	this->Pos = bindPos;
	//	this->Rot = bindRot;
	//	//부모 주소 줌
	//	this->parent = parent;
	//	//toParent 구함
	//	MakeToParentMatrix();
	//	//toDresspose 구함
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
//	// 로컬 행렬
//	XMFLOAT4X4 local;	
//
//	// 이게 아마 모든 부모 변환 다 합친 행렬일 것
//	XMFLOAT4X4 offset;	
//
//	// 로컬 값
//	// 처음 초기화할 때만 쓰자
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

		// 회전
		XMVECTOR q0 = { XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.x), XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.y), XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.z) };
		XMVECTOR q1 = { XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.x), XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.y), XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.z) };
		q0 = XMQuaternionRotationRollPitchYawFromVector(q0);
		q1 = XMQuaternionRotationRollPitchYawFromVector(q1);
		/*XMVECTOR interpolatedR = XMQuaternionSlerp(q0, q1, time);*/

		// 아핀변환
		return XMMatrixAffineTransformation(XMVectorSplatOne(), XMVectorZero(), XMQuaternionSlerp(q0, q1, time), XMVectorLerp(t0, t1, time));

	}

	// 몇 번째 키를 찾을지 정해야 함
	// 보간한 행렬을 가지고 본마다 toWorld를 구해야 하는데?
	// 시간마다 달라지는건 local과 local로 인해 생기는 toWorld 뿐
	// parent->toWorld * toParent * local
	// toParent는 키에 따라 변하지 않음
	// toWorld를 구해 넣으려면 local에 넣어야겠다.
	
	// local을 구해서 keyList[0]->bone[boneIdx]->local에 넣고
	// keyList[0]->bone[boneIdx]의 toWorld를 구하고
	// keyList[0]->bone[boneIdx+1]->local을 넣고
	// keyList[0]->bone[boneIdx+1]의 toWorld를 구하고 반복일 듯
	// 그럴려면 local과 toWorld 행렬을 갖고 있어야 할 듯
	
	// 한 번 돌면서 쭉 toWorld 행렬을 넣고
	// 다른 함수로 쭉 빼내는게 맞을 듯
	// GenerateToWorld()
	// {
	//     for() MakeLocalMatrix(i);
	//     for() MakeToWorldMatrix(i);
	// }
	// /*실 사용 하는 부분(Matrix 하나씩 뺄 때)*/
	// for() GetToWorldMatrix(i);

	// 하기 전에 한 번 불러주세여~~~
	void GenerateToWorld(float time) {
		for (int i = 0; i < nBones; ++i) {
			XMStoreFloat4x4(&keyList[0]->boneList[i]->Local, GetInterpolatedLocalMatrix(i, time));
			keyList[0]->boneList[i]->MakeToWorldMatrix();
		}
	}

	// 이건 행렬마다 매번 부르기
	XMMATRIX GetToWorldMatrix(int boneIdx) {
		return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toWorld);
	}




	//// 몇 번째 본의 로컬 행렬 보간 내용을 주는 함수
	//XMMATRIX InterpolateLocalMatrix(int boneIdx, float time) {
	//	// nBone이 1개면 그냥 바로 반환
	//	if (nKeys == 1) return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->local);
	//	// time이 0과 마지막 키프레임 사이인지 확인
	//	if (time >= keyList[nKeys - 1]->keyTime) {
	//		if (isLoop) time = time - ((int)(time / keyList[nKeys - 1]->keyTime) * keyList[nKeys - 1]->keyTime);
	//		else return XMLoadFloat4x4(&keyList[nKeys - 1]->boneList[boneIdx]->local);
	//	}
	//	if (time <= keyList[0]->keyTime) return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->local);
	//	//if (time >= keyList[nKeys - 1]->keyTime) return XMLoadFloat4x4(&keyList[nKeys - 1]->boneList[boneIdx]->local);
	//	// time이 n과 n+1 키프레임 사이면 그 둘의 회전과 이동 변환을 보간한 행렬을 만들어야 함
	//	// 해당 n을 찾아야 할 것
	//	int n = 0;
	//	for (int i = 0; i < nKeys; ++i) if (time < keyList[i]->keyTime) { n = i - 1; break; }
	//	// time을 0과 1 사이로 만들어야 함(그래야 보간을 하니까)
	//	time = (time - keyList[n]->keyTime) / (keyList[n + 1]->keyTime - keyList[n]->keyTime);
	//	// 근데 쓰는건 로컬 값인데 뭔가 잘못된 것이 아닐까요?
	//	// 이동
	//	XMVECTOR t0 = XMLoadFloat3(&keyList[n]->boneList[boneIdx]->position);
	//	XMVECTOR t1 = XMLoadFloat3(&keyList[n + 1]->boneList[boneIdx]->position);
	//	/*XMVECTOR interpolatedT = XMVectorLerp(t0, t1, time);*/
	//	// 회전
	//	XMVECTOR q0 = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&keyList[n]->boneList[boneIdx]->rotation));
	//	XMVECTOR q1 = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&keyList[n + 1]->boneList[boneIdx]->rotation));
	//	/*XMVECTOR interpolatedR = XMQuaternionSlerp(q0, q1, time);*/
	//	// 아핀변환
	//	return XMMatrixAffineTransformation(XMVectorSplatOne(), XMVectorZero(), XMQuaternionSlerp(q0, q1, time), XMVectorLerp(t0, t1, time));
	//}





	//XMMATRIX OffsetXtoParentXLocalMatrix(int boneIdx, float time) {
	//	// offset과 toParent는 어떤 키든지 본마다 똑같을테니까 [0]으로
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
