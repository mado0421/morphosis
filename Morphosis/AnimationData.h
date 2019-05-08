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

struct AnimationVertex {
public:
	AnimationVertex() {}
	AnimationVertex(Float3 p) :pos(p) {}

public:
	Float3	pos;
	Int4	boneIdx;
	Float4	weight;
};

class AnimationMesh {
public:
	AnimationMesh() {}
	AnimationMesh(const char* name) { this->name = name; }

public:
	std::string						name;
	std::vector<AnimationVertex>	controlPoints;
	std::vector<int>				polygonVertexIndex;
};

class AnimationBone {
public:
	AnimationBone() = default;

	void MakeToWorldMatrix() {
		XMMATRIX ToWorld;
		if (m_parent) {

			XMMATRIX tmp = XMMatrixMultiply(XMLoadFloat4x4(&m_parent->m_toWorld), XMLoadFloat4x4(&m_toParent));
			ToWorld = XMMatrixMultiply(tmp, XMLoadFloat4x4(&m_Local));


			//ToWorld = XMMatrixMultiply(XMLoadFloat4x4(&m_Local), XMMatrixMultiply(XMLoadFloat4x4(&m_toParent), XMLoadFloat4x4(&m_parent->m_toWorld)));
		}
		else {
			ToWorld = XMMatrixMultiply(XMLoadFloat4x4(&m_toParent), XMLoadFloat4x4(&m_Local));


			//ToWorld = XMMatrixMultiply(XMLoadFloat4x4(&m_Local), XMLoadFloat4x4(&m_toParent));
		}
		XMStoreFloat4x4(&m_toWorld, ToWorld);
	}

	void Init() {
		MakeToParentMatrix();
		MakeDressposeInverseMatrix();
	}

private:
	void MakeToParentMatrix() {
		///*무조건 m_translation, m_rotation이 있어야 함.*/
		///Matrix4x4::ToTransform(&m_toParent, m_translation, Vector4::QuatFromAngle(m_rotation));

		///위의 방식은 회전 먼저 하고 이동하기 때문에 처벌당했다. 유감을 표하도록 하지.
		///XMMATRIX xmmtxTranslation = XMMatrixTranslation(m_translation.x, m_translation.y, m_translation.z);
		///XMMATRIX xmmtxRotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_rotation.x), XMConvertToRadians(m_rotation.y), XMConvertToRadians(m_rotation.z));
		///XMMATRIX xmmtxResult = XMMatrixMultiply(xmmtxTranslation, xmmtxRotation);
		///XMMATRIX xmmtxResult = XMMatrixMultiply(xmmtxRotation, xmmtxTranslation);
		///XMStoreFloat4x4(&m_toParent, xmmtxResult);
		//XMMATRIX xmmtxTranslation = XMMatrixTranslation(m_translation.x, m_translation.y, m_translation.z);
		//XMMATRIX xmmtxRotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_rotation.x), XMConvertToRadians(m_rotation.y), XMConvertToRadians(m_rotation.z));
		////XMMATRIX xmmtxResult = XMMatrixMultiply(xmmtxTranslation, xmmtxRotation);
		//XMMATRIX xmmtxResult = XMMatrixMultiply(xmmtxRotation, xmmtxTranslation); //<-이 순서로 해야 TR이 나옴
		//XMStoreFloat4x4(&m_toParent, xmmtxResult);

		XMMATRIX xmmtxTranslation = XMMatrixTranslation(m_translation.x, m_translation.y, m_translation.z);
		XMMATRIX xmmtxRotation = Matrix4x4::MakeFromXYZAngle(m_rotation);
		XMStoreFloat4x4(&m_toParent, XMMatrixMultiply(xmmtxTranslation, xmmtxRotation));
	}
	void MakeDressposeInverseMatrix() {
		XMMATRIX dressposeInv;
		XMVECTOR det;
		if (m_parent) {
			det = XMMatrixDeterminant(XMLoadFloat4x4(&m_toParent));
			//dressposeInv = XMMatrixMultiply(XMLoadFloat4x4(&m_parent->m_dressposeInv), XMMatrixInverse(&det, XMLoadFloat4x4(&m_toParent)));
			dressposeInv = XMMatrixMultiply(XMMatrixInverse(&det, XMLoadFloat4x4(&m_toParent)), XMLoadFloat4x4(&m_parent->m_dressposeInv));
		}
		else {
			det = XMMatrixDeterminant(XMLoadFloat4x4(&m_toParent));
			dressposeInv = XMMatrixInverse(&det, XMLoadFloat4x4(&m_toParent));
		}
		XMStoreFloat4x4(&m_dressposeInv, dressposeInv);
	}

public:
	AnimationBone*	m_parent;
	XMFLOAT4X4		m_Local;
	XMFLOAT3		m_translation;
	XMFLOAT3		m_rotation;

	XMFLOAT4X4		m_dressposeInv;
	XMFLOAT4X4		m_toWorld;
	XMFLOAT4X4		m_toParent;

public:
	int				m_parentIdx;
};

class AnimationKey {
public:

	static bool CompareByKeyTime(const AnimationKey& k1, const AnimationKey& k2) {
		return (k1.m_keytime < k2.m_keytime);
	}

public:
	std::vector<int>			m_boneIdx;
	std::vector<XMFLOAT3>		m_translations;
	std::vector<XMFLOAT3>		m_rotations;

	std::vector<AnimationBone*>	m_pBones;
	float						m_keytime;
};

struct BoneReferenceInfo {
	std::set<int> idxOfKeySet;
};

class AnimationData {
public:
	void		Init(std::vector<AnimationBone> bones, std::vector<AnimationKey> keys) {

		m_bones.resize(bones.size());
		for (int i = 0; i < m_bones.size(); ++i) {
			if (-1 != bones[i].m_parentIdx) {
				m_bones[i].m_parent = &m_bones[bones[i].m_parentIdx];
			}
			else m_bones[i].m_parent = NULL;
			m_bones[i].m_translation	= bones[i].m_translation;
			m_bones[i].m_rotation		= bones[i].m_rotation;
		}
		for (int i = 0; i < m_bones.size(); ++i) {
			m_bones[i].Init();
		}

		m_boneReferenceInfo.resize(bones.size());

		m_keys.resize(keys.size());
		for (int i = 0; i < m_keys.size(); ++i) {

			m_keys[i].m_keytime =				keys[i].m_keytime;
			m_keys[i].m_pBones.resize(			keys[i].m_pBones.size()			);
			m_keys[i].m_boneIdx.resize(			keys[i].m_boneIdx.size()		);
			m_keys[i].m_translations.resize(	keys[i].m_translations.size()	);
			m_keys[i].m_rotations.resize(		keys[i].m_rotations.size()		);

			for (int j = 0; j < m_keys[i].m_boneIdx.size(); ++j) {

				m_keys[i].m_pBones[j]		= &m_bones[keys[i].m_boneIdx[j]];
				m_keys[i].m_boneIdx[j]		= keys[i].m_boneIdx[j];
				m_keys[i].m_translations[j] = keys[i].m_translations[j];
				m_keys[i].m_rotations[j]	= keys[i].m_rotations[j];

				m_boneReferenceInfo[keys[i].m_boneIdx[j]].idxOfKeySet.insert(i);
			}
		}
	}

	/*각 bone들의 toWorld 변환행렬을 생성해서 넣어주는 함수*/
	void		GenerateToWorldMatrix(float time) {
		for (int i = 0; i < m_bones.size(); ++i) {
			XMStoreFloat4x4(&m_bones[i].m_Local, GetInterpolatedLocalMatrix(i, time));
			m_bones[i].MakeToWorldMatrix();
		}
	}
	XMMATRIX	GetFinalMatrix(int boneIdx) {
		//XMMATRIX tmp = XMMatrixMultiply(XMLoadFloat4x4(&m_bones[boneIdx].m_dressposeInv), XMLoadFloat4x4(&m_bones[boneIdx].m_toWorld));
		//return XMMatrixMultiply(tmp, compensatingMatrix);
		return XMMatrixMultiply(XMLoadFloat4x4(&m_bones[boneIdx].m_toWorld), XMLoadFloat4x4(&m_bones[boneIdx].m_dressposeInv));
	}


private:
	XMMATRIX	GetInterpolatedLocalMatrix(int boneIdx, float time) {
		int curKeyIdx, nextKeyIdx;
		int boneIdxInCurKey;
		int boneIdxInNextKey;




		if (m_boneReferenceInfo[boneIdx].idxOfKeySet.size() == 0) { 
			return XMMatrixIdentity(); 
		}
		if (isFurtherThanBack(time)) if (isLoop) time = GetClampTime(time);

		GetKeyCurIdxNextIdxFromRefInfo(boneIdx, time, curKeyIdx, nextKeyIdx);
		boneIdxInCurKey = GetBoneIdxInKey(boneIdx, curKeyIdx);
		boneIdxInNextKey = GetBoneIdxInKey(boneIdx, nextKeyIdx);

		if (curKeyIdx == nextKeyIdx || m_keys.size() == 1 || isFurtherThanFront(time)) {
			XMFLOAT4X4 tmp;
			Matrix4x4::ToTransform(
				&tmp, 
				Vector3::Subtract(m_keys[curKeyIdx].m_translations[boneIdxInCurKey], m_bones[boneIdx].m_translation), 
				Vector4::QuatFromAngle(Vector3::Subtract(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey], m_bones[boneIdx].m_rotation)));
			return XMLoadFloat4x4(&tmp);
		}
		///if (m_keys.size() == 1 || isFurtherThanFront(time)) {
		///	XMFLOAT4X4 tmp;
		///	Matrix4x4::ToTransform(&tmp, m_keys.front().m_translations[boneIdxInCurKey], Vector4::QuatFromAngle(m_keys.front().m_rotations[boneIdxInCurKey]));
		///	return XMLoadFloat4x4(&tmp);
		///}

		//time = GetNormalizedTimeByTwoKeys(time, curKeyIdx, nextKeyIdx);

		/*
		//XMFLOAT4	xmf4BaseQuat;
		//XMStoreFloat4(&xmf4BaseQuat,
		//	XMQuaternionRotationRollPitchYaw(
		//		XMConvertToRadians(m_bones[boneIdx].m_rotation.x),
		//		XMConvertToRadians(m_bones[boneIdx].m_rotation.y),
		//		XMConvertToRadians(m_bones[boneIdx].m_rotation.z)
		//	)
		//);

		//XMFLOAT4	xmf4PrevQuat;
		//XMStoreFloat4(&xmf4PrevQuat,
		//	XMQuaternionRotationRollPitchYaw(
		//		XMConvertToRadians(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey].x),
		//		XMConvertToRadians(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey].y),
		//		XMConvertToRadians(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey].z)
		//	)
		//);

		//XMFLOAT4	xmf4NextQuat;
		//XMStoreFloat4(&xmf4NextQuat,
		//	XMQuaternionRotationRollPitchYaw(
		//		XMConvertToRadians(m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey].x),
		//		XMConvertToRadians(m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey].y),
		//		XMConvertToRadians(m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey].z)
		//	)
		//);

		//XMFLOAT4X4	xmf4x4BaseMtx;
		//XMFLOAT4X4	xmf4x4PrevMtx;
		//XMFLOAT4X4	xmf4x4NextMtx;

		//Matrix4x4::ToTransform(&xmf4x4BaseMtx, m_bones[boneIdx].m_translation, xmf4BaseQuat);
		//Matrix4x4::ToTransform(&xmf4x4PrevMtx, m_keys[curKeyIdx].m_translations[boneIdxInCurKey], xmf4PrevQuat);
		//Matrix4x4::ToTransform(&xmf4x4NextMtx, m_keys[nextKeyIdx].m_translations[boneIdxInNextKey], xmf4NextQuat);

		//XMMATRIX	xmmtxInterpolated;
		//Matrix4x4::InterpolateMtx(&xmmtxInterpolated, XMLoadFloat4x4(&xmf4x4PrevMtx), XMLoadFloat4x4(&xmf4x4NextMtx), static_cast<float>(time));

		//XMVECTOR	xmvDet = XMMatrixDeterminant(xmmtxInterpolated);
		//XMMATRIX	xmmtxInterpolatedInv = XMMatrixInverse(&xmvDet, xmmtxInterpolated);
		//XMMATRIX	xmmtxBase = XMLoadFloat4x4(&xmf4x4BaseMtx);
		////XMFLOAT4X4	xmf4x4Result;
		//return XMMatrixMultiply(xmmtxBase, xmmtxInterpolatedInv);

		//=====================================================================================================
		// 거의 다 했다!
		//=====================================================================================================

		//XMVECTOR xmvBoneRotationQuat 
		//	= XMQuaternionRotationRollPitchYaw(
		//		XMConvertToRadians(m_bones[boneIdx].m_rotation.x), 
		//		XMConvertToRadians(m_bones[boneIdx].m_rotation.y), 
		//		XMConvertToRadians(m_bones[boneIdx].m_rotation.z));
		//XMFLOAT4 xmf4BoneRotationQuat;
		//XMStoreFloat4(&xmf4BoneRotationQuat, xmvBoneRotationQuat);

		//XMVECTOR xmvPrevRotationTransformed = XMVector3Rotate(XMLoadFloat3(&m_keys[curKeyIdx].m_rotations[boneIdxInCurKey]), xmvBoneRotationQuat);
		//XMVECTOR xmvNextRotationTransformed = XMVector3Rotate(XMLoadFloat3(&m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey]), xmvBoneRotationQuat);
		//XMFLOAT4 xmf4PrevQuat;
		//XMFLOAT4 xmf4NextQuat;
		//XMStoreFloat4(&xmf4PrevQuat, xmvPrevRotationTransformed);
		//XMStoreFloat4(&xmf4NextQuat, xmvNextRotationTransformed);

		//XMFLOAT4X4 xmf4x4BoneMtx;
		//XMFLOAT4X4 xmf4x4PrevMtx;
		//XMFLOAT4X4 xmf4x4NextMtx;

		//Matrix4x4::ToTransform(&xmf4x4BoneMtx, m_bones[boneIdx].m_translation, xmf4BoneRotationQuat);
		//Matrix4x4::ToTransform(&xmf4x4PrevMtx, m_keys[curKeyIdx].m_translations[boneIdxInCurKey], xmf4PrevQuat);
		//Matrix4x4::ToTransform(&xmf4x4NextMtx, m_keys[nextKeyIdx].m_translations[boneIdxInNextKey], xmf4NextQuat);

		//XMMATRIX xmmtxInterpolated;
		//Matrix4x4::InterpolateMtx(&xmmtxInterpolated, XMLoadFloat4x4(&xmf4x4PrevMtx), XMLoadFloat4x4(&xmf4x4NextMtx), static_cast<float>(time));

		//XMVECTOR xmvDet = XMMatrixDeterminant(XMLoadFloat4x4(&  xmf4x4BoneMtx));
		//XMMATRIX xmmtxBoneInv = XMMatrixInverse(&xmvDet, XMLoadFloat4x4(&xmf4x4BoneMtx));
		////XMMATRIX xmmtxBase = XMLoadFloat4x4(&xmf4x4BaseMtx);
		//return XMMatrixMultiply(xmmtxBoneInv, xmmtxInterpolated);

		//=====================================================================================================
		// 최종본이 될 것.
		//=====================================================================================================
		//먼저 이전 키랑 다음 키 보간을 먼저 하자.
		//XMFLOAT3 xmf3LclTranslation 
		//	= Vector3::Lerp(
		//		m_keys[curKeyIdx].m_translations[boneIdxInCurKey],
		//		m_keys[nextKeyIdx].m_translations[boneIdxInNextKey],
		//		time);

		//XMFLOAT4 xmf4PrevRotationQuat;
		//XMFLOAT4 xmf4NextRotationQuat;
		//XMStoreFloat4(&xmf4PrevRotationQuat,
		//	XMQuaternionRotationRollPitchYaw(
		//		XMConvertToRadians(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey].x),
		//		XMConvertToRadians(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey].y),
		//		XMConvertToRadians(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey].z)));
		//XMStoreFloat4(&xmf4NextRotationQuat,
		//	XMQuaternionRotationRollPitchYaw(
		//		XMConvertToRadians(m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey].x),
		//		XMConvertToRadians(m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey].y),
		//		XMConvertToRadians(m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey].z)));

		//XMFLOAT4 xmf4LclRotationQuat
		//	= Vector4::QuatSlerp(
		//		xmf4PrevRotationQuat,
		//		xmf4NextRotationQuat,
		//		time);

		//XMFLOAT3 xmf3LclRotationAngle;
		//quat_2_euler_d3d(xmf4LclRotationQuat, xmf3LclRotationAngle.x, xmf3LclRotationAngle.y, xmf3LclRotationAngle.z);
		//xmf3LclRotationAngle.x = XMConvertToDegrees(xmf3LclRotationAngle.x);
		//xmf3LclRotationAngle.y = XMConvertToDegrees(xmf3LclRotationAngle.y);
		//xmf3LclRotationAngle.z = XMConvertToDegrees(xmf3LclRotationAngle.z);

		////이젠 bone의 기본 값들을 가지고 계산할 차례
		////먼저 회전각으로 쿼터니언을 만들고 회전시키자.
		//XMVECTOR xmvBoneRotationQuat
		//	= XMQuaternionRotationRollPitchYaw(
		//		XMConvertToRadians(m_bones[boneIdx].m_rotation.x),
		//		XMConvertToRadians(m_bones[boneIdx].m_rotation.y),
		//		XMConvertToRadians(m_bones[boneIdx].m_rotation.z));

		//XMVECTOR xmvLclRotationTransformed = XMVector3Rotate(XMLoadFloat3(&xmf3LclRotationAngle), xmvBoneRotationQuat);
		////translation도 회전시켜줘야 할까? 일단 해주자.
		//XMVECTOR xmvLclTranslationTransformed = XMVector3Rotate(XMLoadFloat3(&xmf3LclTranslation), xmvBoneRotationQuat);

		////키 값으로 만든 행렬
		//XMStoreFloat3(&xmf3LclTranslation, xmvLclTranslationTransformed);
		//XMStoreFloat3(&xmf3LclRotationAngle, xmvLclRotationTransformed);
		//XMStoreFloat4(&xmf4LclRotationQuat,
		//	XMQuaternionRotationRollPitchYaw(
		//		XMConvertToRadians(xmf3LclRotationAngle.x),
		//		XMConvertToRadians(xmf3LclRotationAngle.y),
		//		XMConvertToRadians(xmf3LclRotationAngle.z)));
		//XMFLOAT4X4 xmf4x4LclTransformMtx;
		//Matrix4x4::ToTransform(&xmf4x4LclTransformMtx, xmf3LclTranslation, xmf4LclRotationQuat);

		////bone값으로 만든 행렬과 그것의 역행렬
		//XMFLOAT4 xmf4BoneRotationQuat;
		//XMStoreFloat4(&xmf4BoneRotationQuat, xmvBoneRotationQuat);
		//XMFLOAT4X4 xmf4x4BoneMtx;
		//Matrix4x4::ToTransform(&xmf4x4BoneMtx, m_bones[boneIdx].m_translation, xmf4BoneRotationQuat);

		//XMVECTOR xmvBoneDet = XMMatrixDeterminant(XMLoadFloat4x4(&xmf4x4BoneMtx));
		//XMMATRIX xmmtxBoneInvMtx = XMMatrixInverse(&xmvBoneDet, XMLoadFloat4x4(&xmf4x4BoneMtx));
		//XMMATRIX xmmtxLclTransformMtx = XMLoadFloat4x4(&xmf4x4LclTransformMtx);
		////XMMATRIX xmmtxResult = XMMatrixMultiply(xmmtxBoneInvMtx, xmmtxLclTransformMtx);

		//return XMMatrixMultiply(xmmtxBoneInvMtx, xmmtxLclTransformMtx);
		*/
		//=====================================================================================================
		// 응 아니야
		//=====================================================================================================
		//if (boneIdx != 11 && boneIdx != 12 && boneIdx != 13) {
		//	return XMMatrixIdentity();
		//}

		//return XMMatrixIdentity();



		//XMFLOAT3 xmf3R1 = m_keys[curKeyIdx].m_rotations[boneIdxInCurKey];
		//XMFLOAT3 xmf3R2 = m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey];
		//XMFLOAT3 xmf3B = m_bones[boneIdx].m_rotation;

		//XMFLOAT3 r0 = xmf3R1;
		//XMFLOAT3 r1 = xmf3R2;
		//r0.x *= -1;
		//r0.y *= -1;
		//r0.z *= -1;
		//r1.x *= -1;
		//r1.y *= -1;
		//r1.z *= -1;

		////float time = 0.5;
		//XMFLOAT3 r = Vector3::Lerp(r0, r1, time);
		//XMFLOAT4X4 mtx;
		//XMStoreFloat4x4(&mtx, Matrix4x4::MakeFromXYZAngle(r));

		//r = Vector3::AngleFromMtx(mtx);

		////cout << "(" << r.x << ", " << r.y << ", " << r.z << ")\n";

		//return XMLoadFloat4x4(&mtx);









		//XMFLOAT3 xmf3OriginalRotation = Vector3::Lerp(xmf3R1, xmf3R2, time);
		//XMFLOAT3 xmf3BoneRotation = xmf3B;
		//
		//XMVECTOR xmvBoneRotationQuat
		//= XMQuaternionRotationRollPitchYaw(
		//	XMConvertToRadians(xmf3BoneRotation.x),
		//	XMConvertToRadians(xmf3BoneRotation.y),
		//	XMConvertToRadians(xmf3BoneRotation.z));
		//
		//XMVECTOR xmvOriginalRotationTransformed = XMVector3Rotate(XMLoadFloat3(&xmf3OriginalRotation), xmvBoneRotationQuat);
		//XMFLOAT3 xmf3OriginalRotationResult;
		//XMStoreFloat3(&xmf3OriginalRotationResult, xmvOriginalRotationTransformed);
		//
		////cout << fixed << setprecision(2)
		////<< "Prev is		(" << setw(7) << xmf3OriginalRotation.x
		////<< ", " << setw(7) << xmf3OriginalRotation.y
		////<< ", " << setw(7) << xmf3OriginalRotation.z << ")\n";
		////cout << fixed << setprecision(2)
		////<< "Result is	(" << setw(7) << xmf3OriginalRotationResult.x
		////<< ", " << setw(7) << xmf3OriginalRotationResult.y
		////<< ", " << setw(7) << xmf3OriginalRotationResult.z << ")\n";
		//
		////return XMMatrixRotationRollPitchYawFromVector(xmvOriginalRotationTransformed);


		//XMMATRIX xmmtxBoneRotationMtx
		//= XMMatrixRotationRollPitchYaw(
		//	XMConvertToRadians(xmf3BoneRotation.x),
		//	XMConvertToRadians(xmf3BoneRotation.y),
		//	XMConvertToRadians(xmf3BoneRotation.z));
		//XMVECTOR xmvBoneRotationMtxDet = XMMatrixDeterminant(xmmtxBoneRotationMtx);
		//XMMATRIX xmmtxBoneRotationInvMtx = XMMatrixInverse(&xmvBoneRotationMtxDet, xmmtxBoneRotationMtx);
		////XMMATRIX xmmtxOriginalRotationMtx
		////= XMMatrixRotationRollPitchYaw(
		////	XMConvertToRadians(xmf3OriginalRotation.x),
		////	XMConvertToRadians(xmf3OriginalRotation.y),
		////	XMConvertToRadians(xmf3OriginalRotation.z));
		//XMMATRIX xmmtxOriginalRotationMtx
		//	= XMMatrixRotationRollPitchYaw(
		//		XMConvertToRadians(xmf3OriginalRotationResult.x),
		//		XMConvertToRadians(xmf3OriginalRotationResult.y),
		//		XMConvertToRadians(xmf3OriginalRotationResult.z));
		////XMMATRIX xmmtxResult = XMMatrixMultiply(xmmtxBoneRotationInvMtx, xmmtxOriginalRotationMtx);
		//XMMATRIX xmmtxResult = XMMatrixMultiply(xmmtxBoneRotationInvMtx, xmmtxOriginalRotationMtx);
		//XMFLOAT4X4 xmf4x4Result;
		//XMStoreFloat4x4(&xmf4x4Result, xmmtxResult);
		//XMFLOAT4 xmf4Result;
		//xmf4Result = Vector4::QuatFromMtx(xmf4x4Result);
		//XMFLOAT3 xmf3Result;
		////quat_2_euler_d3d(xmf4Result, xmf3Result.x, xmf3Result.y, xmf3Result.z);
		//xmf3Result = Vector3::AngleFromQuat(xmf4Result);
		//
		////xmf3Result.x =XMConvertToDegrees(xmf3Result.x);
		////xmf3Result.y =XMConvertToDegrees(xmf3Result.y);
		////xmf3Result.z =XMConvertToDegrees(xmf3Result.z);


		////cout << fixed << setprecision(2)
		////<< "Angle is	(" << setw(7) << XMConvertToDegrees(xmf3Result.x)
		////<< ", " << setw(7) << XMConvertToDegrees(xmf3Result.y)
		////<< ", " << setw(7) << XMConvertToDegrees(xmf3Result.z) << ")\n";

		//return xmmtxResult;

		//=====================================================================================================
		// 제발
		//=====================================================================================================
		//XMFLOAT3 xmf3R1 = m_keys[curKeyIdx].m_rotations[boneIdxInCurKey];
		//XMFLOAT3 xmf3R2 = m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey];
		//XMFLOAT3 xmf3B = m_bones[boneIdx].m_rotation;

		//XMFLOAT3 xmf3OriginalRotation = Vector3::Lerp(xmf3R1, xmf3R2, time);
		//XMFLOAT3 xmf3BoneRotation = xmf3B;
		//XMVECTOR xmvBoneRotationQuat = Vector4::QuatFromXYZAngle(xmf3BoneRotation);

		//XMVECTOR xmvOriginalRotationTransformed = XMVector3Rotate(XMLoadFloat3(&xmf3OriginalRotation), xmvBoneRotationQuat);
		//XMFLOAT3 xmf3OriginalRotationResult;
		//XMStoreFloat3(&xmf3OriginalRotationResult, xmvOriginalRotationTransformed);

		//XMMATRIX xmmtxBoneRotationMtx;

		//xmmtxBoneRotationMtx = Matrix4x4::MakeFromXYZAngle(xmf3BoneRotation);

		//XMVECTOR xmvBoneRotationMtxDet = XMMatrixDeterminant(xmmtxBoneRotationMtx);
		//XMMATRIX xmmtxBoneRotationInvMtx = XMMatrixInverse(&xmvBoneRotationMtxDet, xmmtxBoneRotationMtx);
		//XMMATRIX xmmtxOriginalRotationMtx;
		//xmmtxOriginalRotationMtx = Matrix4x4::MakeFromXYZAngle(xmf3OriginalRotationResult);

		//XMMATRIX xmmtxResult = XMMatrixMultiply(xmmtxOriginalRotationMtx, xmmtxBoneRotationInvMtx);

		//XMFLOAT4X4 xmf4x4Result;
		//XMStoreFloat4x4(&xmf4x4Result, xmmtxResult);
		//XMFLOAT4 xmf4Result;
		//xmf4Result = Vector4::QuatFromMtx(xmf4x4Result);
		//XMFLOAT3 xmf3Result;
		//xmf3Result = Vector3::AngleFromQuat(xmf4Result);

		//return xmmtxResult;

		/******************************************************************************************************
		마지막이야~
		******************************************************************************************************/

//return XMMatrixIdentity();
//XMFLOAT4X4 result;

XMFLOAT3 p = { 0, 0, 5 * time };
return Matrix4x4::MakeFromXYZAngle(p);

//XMStoreFloat4x4(&result, Matrix4x4::MakeFromXYZAngle(p));

//PrintM(result);

		XMFLOAT3 xmf3R1 = m_keys[curKeyIdx].m_rotations[boneIdxInCurKey];
		XMFLOAT3 xmf3R2 = m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey];
		XMFLOAT3 xmf3B = m_bones[boneIdx].m_rotation;


		XMFLOAT3 r0 = Vector3::ScalarProduct(xmf3R1, -1, false);
		XMFLOAT3 r1 = Vector3::ScalarProduct(xmf3R2, -1, false);
		XMFLOAT3 b0 = Vector3::ScalarProduct(xmf3B, -1, false);

		//time = 0;
		XMFLOAT3 r2 = Vector3::Lerp(r0, r1, time);

		XMMATRIX r = Matrix4x4::MakeFromXYZAngle(r2);
		XMMATRIX b = Matrix4x4::MakeFromXYZAngle(b0);
		XMFLOAT4X4 result;

		//XMStoreFloat4x4(&result, r);
		//PrintM(result);

		//XMStoreFloat4x4(&result, b);
		//PrintM(result);

		XMVECTOR d = XMMatrixDeterminant(b);
		b = XMMatrixInverse(&d, b);

		XMStoreFloat4x4(&result, XMMatrixMultiply(b, r));
		//PrintM(result);

		return XMMatrixMultiply(b, r);









		///XMFLOAT3 xmf3tmp;
		

		///XMFLOAT4X4	xmf4x4m1;
		//XMFLOAT3	xmf3t1 = Vector3::Subtract(m_keys[curKeyIdx].m_translations[boneIdxInCurKey], m_bones[boneIdx].m_translation);
		//XMFLOAT3	xmf3r1 = Vector3::Subtract(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey], m_bones[boneIdx].m_rotation);
		///XMVECTOR	xmvR1 = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(xmf3r1.x), XMConvertToRadians(xmf3r1.y), XMConvertToRadians(xmf3r1.z));
		///XMFLOAT4	xmf4r1;
		///xmf3tmp = Vector3::Subtract(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey], m_bones[boneIdx].m_rotation);
		///XMStoreFloat4(&xmf4r1, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(xmf3tmp.x), XMConvertToRadians(xmf3tmp.y), XMConvertToRadians(xmf3tmp.z)));
		///Matrix4x4::ToTransform(&xmf4x4m1, xmf3t1, xmf4r1);

		///XMFLOAT4X4	xmf4x4m2;
		//XMFLOAT3	xmf3t2 = Vector3::Subtract(m_keys[nextKeyIdx].m_translations[boneIdxInNextKey], m_bones[boneIdx].m_translation);
		//XMFLOAT3	xmf3r2 = Vector3::Subtract(m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey], m_bones[boneIdx].m_rotation);
		///XMVECTOR	xmvR2 = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(xmf3r2.x), XMConvertToRadians(xmf3r2.y), XMConvertToRadians(xmf3r2.z));
		///XMFLOAT4	xmf4r2;
		///xmf3tmp = Vector3::Subtract(m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey], m_bones[boneIdx].m_rotation);
		///XMStoreFloat4(&xmf4r2, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(xmf3tmp.x), XMConvertToRadians(xmf3tmp.y), XMConvertToRadians(xmf3tmp.z)));
		///Matrix4x4::ToTransform(&xmf4x4m2, xmf3t2, xmf4r2);

		///XMMATRIX m1 = XMLoadFloat4x4(&xmf4x4m1);
		///XMMATRIX m2 = XMLoadFloat4x4(&xmf4x4m2);

		//XMFLOAT3 xmf3resultT;
		//XMFLOAT3 xmf3resultR;

		//XMFLOAT4	xmf4resultRQ;
		//xmf3resultT = Vector3::Subtract(xmf3t2, Vector3::Lerp(xmf3t1, xmf3t2, time));
		//xmf3resultT = Vector3::Add(m_keys[curKeyIdx].m_translations[boneIdxInCurKey], xmf3resultT);
		//xmf3resultR = Vector3::Subtract(xmf3r2, Vector3::Lerp(xmf3r1, xmf3r2, time));
		//xmf3resultR = Vector3::Add(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey], xmf3resultR);
		//XMStoreFloat4(&xmf4resultRQ, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(xmf3resultR.x), XMConvertToRadians(xmf3resultR.y), XMConvertToRadians(xmf3resultR.z)));
		///XMQuaternionSlerp(xmvR1, )


		///XMMATRIX result;
		//XMFLOAT4X4	xmf4x4result;
		//Matrix4x4::ToTransform(&xmf4x4result, xmf3resultT, xmf4resultRQ);
		//return XMLoadFloat4x4(&xmf4x4result);

		///xmf3tmp		= Vector3::Subtract(m_keys[curKeyIdx].m_translations[boneIdxInCurKey], m_bones[boneIdx].m_translation);
		///XMVECTOR t0 = XMLoadFloat3(&xmf3tmp);
		///xmf3tmp		= Vector3::Subtract(m_keys[nextKeyIdx].m_translations[boneIdxInNextKey], m_bones[boneIdx].m_translation);
		///XMVECTOR t1 = XMLoadFloat3(&xmf3tmp);
		///XMVECTOR t0 = XMLoadFloat3(&m_keys[curKeyIdx].m_translations[boneIdxInCurKey]);
		///XMVECTOR t1 = XMLoadFloat3(&m_keys[nextKeyIdx].m_translations[boneIdxInNextKey]);
		///xmf3tmp		= Vector3::Subtract(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey], m_bones[boneIdx].m_rotation);
		///XMVECTOR q0 = { XMConvertToRadians(xmf3tmp.x), XMConvertToRadians(xmf3tmp.y), XMConvertToRadians(xmf3tmp.z) };
		///xmf3tmp		= Vector3::Subtract(m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey], m_bones[boneIdx].m_rotation);
		///XMVECTOR q1 = { XMConvertToRadians(xmf3tmp.x), XMConvertToRadians(xmf3tmp.y), XMConvertToRadians(xmf3tmp.z) };
		///q0 = XMQuaternionRotationRollPitchYawFromVector(q0);
		///q1 = XMQuaternionRotationRollPitchYawFromVector(q1);
		///XMMATRIX m0 = XMMatrixRotationQuaternion(q0);
		///XMMATRIX m1 = XMMatrixRotationQuaternion(q1);






		///Matrix4x4::InterpolateMtx(
		///	&result
		///	, m1
		///	, m2
		///	, time);
		///return result;
	}
	bool		isFurtherThanBack(float time) { return (time >= m_keys.back().m_keytime); }
	bool		isFurtherThanFront(float time) { return (time <= m_keys.front().m_keytime); }
	float		GetClampTime(float time) { return (time - ((int)(time / m_keys.back().m_keytime) * m_keys.back().m_keytime)); }
	int			GetKeyIdxByTime(float time) {
		for (auto p = m_keys.cbegin(); p != m_keys.cend(); ++p) 
			if (time < p->m_keytime) return distance(m_keys.cbegin(), p); 
	}
	int			GetBoneIdxInKey(int boneIdx, int keyIdx) {
		for (auto p = m_keys[keyIdx].m_boneIdx.cbegin(); p != m_keys[keyIdx].m_boneIdx.cend(); ++p)
			if (*p == boneIdx) return distance(m_keys[keyIdx].m_boneIdx.cbegin(), p);
	}
	void		GetNormalizedTime(float& time, int keyIdx) {
		time = (time - m_keys[keyIdx].m_keytime) / (m_keys[keyIdx + 1].m_keytime - m_keys[keyIdx].m_keytime);
	}
	int			GetKeyIdxByTimeFromRefInfo(int boneIdx, float time) {
		for (auto p = m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin(); p != m_boneReferenceInfo[boneIdx].idxOfKeySet.cend(); ++p)
			if (time < m_keys[(*p)].m_keytime) return (*p);
	}
	void		GetKeyCurIdxNextIdxFromRefInfo(int boneIdx, float time, int& curIdx, int& nextIdx) {
		if (m_boneReferenceInfo[boneIdx].idxOfKeySet.size() == 1) {
			curIdx = *m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin();
			nextIdx = *m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin();
			return;
		}

		if (m_keys[*m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin()].m_keytime >= time) {
			curIdx = *m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin();
			nextIdx = *m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin();
			return;
		}
		for (auto p = m_boneReferenceInfo[boneIdx].idxOfKeySet.cbegin(); p != m_boneReferenceInfo[boneIdx].idxOfKeySet.cend(); ++p) {
			if (time < m_keys[(*p)].m_keytime) {
				nextIdx = *p; p--;
				curIdx = *p;
				//if (p == m_boneReferenceInfo[boneIdx].idxOfKeySet.cend())	nextIdx = curIdx;
				//else														nextIdx = *p;
				return;
			}
		}
		curIdx = *m_boneReferenceInfo[boneIdx].idxOfKeySet.crbegin();
		nextIdx = curIdx;
		/* for Error Check */
		//curIdx = -1;
		//nextIdx = -1;
	}
	float		GetNormalizedTimeByTwoKeys(float time, int curKeyIdx, int nextKeyIdx) {
		return (time - m_keys[curKeyIdx].m_keytime) / (m_keys[nextKeyIdx].m_keytime - m_keys[curKeyIdx].m_keytime);
	}

public:
	AnimationData() = default;


public:
	std::vector<AnimationKey>		m_keys;
	std::vector<AnimationBone>		m_bones;
	std::vector<BoneReferenceInfo>	m_boneReferenceInfo;

	bool							isLoop = true;
	XMMATRIX						compensatingMatrix = {
	0, 0, -1, 0,
	0, -1, 0, 0,
	1, 0, 0, 0,
	0, 0, 0, 1
	};
};


//class Anim {
//public:	// 이것만 보여주면 됨
//	// 하기 전에 한 번 불러주세여~~~
//	void GenerateToWorld(float time) {
//		for (int i = 0; i < nBones; ++i) {
//			XMStoreFloat4x4(&keyList[0]->boneList[i]->Local, GetInterpolatedLocalMatrix(i, time));
//			//if (i < 2) XMStoreFloat4x4(&keyList[0]->boneList[i]->Local, XMMatrixIdentity());
//			keyList[0]->boneList[i]->MakeToWorldMatrix();
//		}
//	}
//
//	// 이건 행렬마다 매번 부르기
//	XMMATRIX GetFinalMatrix(int boneIdx) {
//#ifdef TEST_
//		//return XMMatrixIdentity();
//		//return XMMatrixIdentity();
//		//if(boneIdx!=0) return XMMatrixIdentity();
//		//else return XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toDressposeInverse);
//
//#endif
//
//
//		XMMATRIX finalMatrix = XMMatrixMultiply(
//			XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toDressposeInverse),
//			XMLoadFloat4x4(&keyList[0]->boneList[boneIdx]->toWorld));
//
//#ifdef TEST_
//		return XMMatrixMultiply(finalMatrix, a);
//#endif
//		return finalMatrix;
//	}
//
//private:	// 이건 이 클래스 안에서만 쓸거야
//	XMMATRIX GetInterpolatedLocalMatrix(int boneIdx, float time) {
//		if (nKeys == 1) return keyList[0]->boneList[boneIdx]->GetLocalMatrix();
//
//		if (IsFurtherThanBack(time)) {
//			if (isLoop) time = ClampTime(time);
//			else		return keyList[nKeys - 1]->boneList[boneIdx]->GetLocalMatrix();
//		}
//		if (IsFurtherThanFront(time)) return keyList[0]->boneList[boneIdx]->GetLocalMatrix();
//
//		int n = 0;
//		for (int i = 0; i < nKeys; ++i) if (time < keyList[i]->keyTime) { n = i - 1; break; }
//
//		time = (time - keyList[n]->keyTime) / (keyList[n + 1]->keyTime - keyList[n]->keyTime);
//
//		XMVECTOR t0 = XMLoadFloat3(&keyList[n]->boneList[boneIdx]->Pos);
//		XMVECTOR t1 = XMLoadFloat3(&keyList[n + 1]->boneList[boneIdx]->Pos);
//
//		//XMVECTOR t0 = { 0,0,0,1 };
//		//XMVECTOR t1 = { 0,0,0,1 };
//		/*XMVECTOR interpolatedT = XMVectorLerp(t0, t1, time);*/
//
//		// 회전
//		XMVECTOR q0 = { XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.x), XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.y), XMConvertToRadians(keyList[n]->boneList[boneIdx]->Rot.z) };
//		XMVECTOR q1 = { XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.x), XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.y), XMConvertToRadians(keyList[n + 1]->boneList[boneIdx]->Rot.z) };
//		q0 = XMQuaternionRotationRollPitchYawFromVector(q0);
//		q1 = XMQuaternionRotationRollPitchYawFromVector(q1);
//		/*XMVECTOR interpolatedR = XMQuaternionSlerp(q0, q1, time);*/
//		XMFLOAT3 rotateOriginTemp;
//		XMVECTOR rotateOrigin;
//		if (boneIdx != 0) {
//			rotateOriginTemp = XMFLOAT3(
//				keyList[0]->boneList[boneIdx - 1]->toWorld._41,
//				keyList[0]->boneList[boneIdx - 1]->toWorld._42,
//				keyList[0]->boneList[boneIdx - 1]->toWorld._43
//			);
//			rotateOrigin = XMLoadFloat3(&rotateOriginTemp);
//		}
//		else {
//			rotateOriginTemp = XMFLOAT3(
//				keyList[0]->boneList[boneIdx]->toDresspose._41,
//				keyList[0]->boneList[boneIdx]->toDresspose._42,
//				keyList[0]->boneList[boneIdx]->toDresspose._43
//			);
//			rotateOrigin = XMLoadFloat3(&rotateOriginTemp);
//		}
//
//		// 아핀변환
//		return XMMatrixAffineTransformation(XMVectorSplatOne(), rotateOrigin, XMQuaternionSlerp(q0, q1, time), XMVectorLerp(t0, t1, time));
//
//	}
//	// helper
//	bool IsFurtherThanFront(float time)
//	{
//		return (time <= keyList[0]->keyTime);
//	}
//	bool IsFurtherThanBack(float time)
//	{
//		return (time >= keyList[nKeys - 1]->keyTime);
//	}
//	float ClampTime(float time)
//	{
//		return (time - ((int)(time / keyList[nKeys - 1]->keyTime) * keyList[nKeys - 1]->keyTime));
//	}
//
//public:
//	Anim();
//	~Anim();
//
//public:
//	CKey * * keyList;
//	int nKeys;
//
//	int nBones;
//
//	bool isLoop = true;
//
//	// y축으로 -90도 회전 후 y축 반전!
//	XMMATRIX a = {
//		0, 0, -1, 0,
//		0, -1, 0, 0,
//		1, 0, 0, 0,
//		0, 0, 0, 1
//	};
//};
