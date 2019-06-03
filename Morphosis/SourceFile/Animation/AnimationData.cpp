#include "stdafx.h"
#include "AnimationData.h"
#include "Importer/Importer.h"

void AnimationData::Init(ImportAnimData & animData)
{
	//m_AnimData = new ImportAnimData(animData);

	ImportAnimData* tmp = new ImportAnimData(animData);

	m_AnimData.push_back(tmp);
}

void AnimationData::AddAnimData(ImportAnimData * animData)
{
	m_AnimData.push_back(animData);
}

XMMATRIX AnimationData::GetFinalMatrix(int boneIdx, float time)
{
	///return XMMatrixMultiply(XMLoadFloat4x4(&m_bones[boneIdx].m_dressposeInv), XMLoadFloat4x4(&m_bones[boneIdx].m_toWorld));
	XMVECTOR det;
	XMMATRIX OffsetInv, ToRootInv;
	det = XMMatrixDeterminant(XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_GlobalTransform));
	OffsetInv = XMMatrixInverse(&det, XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_GlobalTransform));

	///XMFLOAT3 pos = Vector3::PosFromMtx(m_AnimData->m_BoneList[boneIdx].m_GlobalTransform);
	///OffsetInv = XMMatrixTranslation(-pos.x, -pos.y, -pos.z);
	///OffsetInv = XMMatrixTranslation(0, 1000, 0);
	///OffsetInv = XMMatrixRotationRollPitchYaw(10, 10, 10);


	ToRootInv = GetInterpolatedToRootMtx(boneIdx, time);

	det = XMMatrixDeterminant(ToRootInv);
	ToRootInv = XMMatrixInverse(&det, ToRootInv);

	///return OffsetInv;
	///return XMMatrixIdentity();
	return XMMatrixMultiply(OffsetInv, ToRootInv);
	///return XMMatrixMultiply(ToRootInv, OffsetInv);

	///return XMMatrixMultiply(XMLoadFloat4x4(&m_AnimData->m_BoneList[boneIdx].m_GlobalTransform), GetInterpolatedToRootMtx(boneIdx, time));

}

XMMATRIX AnimationData::GetInterpolatedLocalMatrix(int boneIdx, float time)
{
//	int curKeyIdx, nextKeyIdx;
//	__int64 boneIdxInCurKey;
//	__int64 boneIdxInNextKey;
//	__int64 boneIdxInFirstKey;
//
//
//	if (m_boneReferenceInfo[boneIdx].idxOfKeySet.size() == 0) { return XMMatrixIdentity(); }
//	if (isFurtherThanBack(time)) if (isLoop) time = GetClampTime(time);
//
//	GetKeyCurIdxNextIdxFromRefInfo(boneIdx, time, curKeyIdx, nextKeyIdx);
//	int firstKeyIdx = GetKeyFirstIdxFromRefInfo(boneIdx);
//	boneIdxInCurKey = GetBoneIdxInKey(boneIdx, curKeyIdx);
//	boneIdxInNextKey = GetBoneIdxInKey(boneIdx, nextKeyIdx);
//	boneIdxInFirstKey = GetBoneIdxInKey(boneIdx, firstKeyIdx);
//
//
//	if (curKeyIdx == nextKeyIdx || m_keys.size() == 1 || isFurtherThanFront(time)) {
//		XMFLOAT4X4 tmp;
//		Matrix4x4::ToTransform(
//			&tmp,
//			Vector3::Subtract(m_keys[curKeyIdx].m_translations[boneIdxInCurKey], m_bones[boneIdx].m_translation),
//			Vector4::QuatFromAngle(Vector3::Subtract(m_keys[curKeyIdx].m_rotations[boneIdxInCurKey], m_bones[boneIdx].m_rotation)));
//		return XMLoadFloat4x4(&tmp);
//	}
//
//	//XMFLOAT3 xmf3B = m_bones[boneIdx].m_rotation;
//	XMFLOAT3 xmf3R1 = m_keys[curKeyIdx].m_rotations[boneIdxInCurKey];
//	XMFLOAT3 xmf3R2 = m_keys[nextKeyIdx].m_rotations[boneIdxInNextKey];
//	XMFLOAT3 xmf3B = m_keys[firstKeyIdx].m_rotations[boneIdxInFirstKey];
	return XMMatrixIdentity();
}

XMMATRIX AnimationData::GetInterpolatedToRootMtx(int boneIdx, float time)
{
	int PrevIdx = 0;
	float normalizedTime = 0;
	// 만약 키프레임이 하나 밖에 없으면 맨 첫 번째 ToRoot를 반환
	if (m_AnimData[m_AnimState]->m_nKeyTime == 1) return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[0]);
	//if (isFurtherThanBack(time)) if (isLoop) time = GetClampTime(time);

	// 시간을 비교해서 보간할 idx 두 개를 구해야 함.
	// 시간이 맨 앞보다 빠르면 맨 앞을 반환.
	if (time < m_AnimData[m_AnimState]->m_KeyTime[0]) return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[0]);
	// 시간이 맨 뒤보다 늦으면 맨 뒤를 반환.
	else if (time > m_AnimData[m_AnimState]->m_KeyTime[m_AnimData[m_AnimState]->m_nKeyTime - 1]) {
		if (false == isLoop) return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[m_AnimData[m_AnimState]->m_nKeyTime - 1]);
	}

	// 보간 값 계산
	time = GetClampTime(time);
	PrevIdx = GetPrevIdx(time);
	normalizedTime = GetNormalizedTime(time, PrevIdx);

	XMFLOAT4X4 result;

	Matrix4x4::InterpolateMtx(&result, m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[PrevIdx], m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[PrevIdx + 1], normalizedTime);

	// return XMLoadFloat4x4(&result);
	return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[PrevIdx]);
}

XMMATRIX AnimationData::GetOffset(int boneIdx)
{
	return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_GlobalTransform);
}

float AnimationData::GetEndTime()
{
	return m_AnimData[m_AnimState]->m_KeyTime[m_AnimData[m_AnimState]->m_nKeyTime - 1];
}

int AnimationData::GetPrevIdx(float time)
{
	for (int i = 0; i < m_AnimData[m_AnimState]->m_nKeyTime; ++i)
		if (m_AnimData[m_AnimState]->m_KeyTime[i] > time) return i;
	return 0;
}

float AnimationData::GetNormalizedTime(float time, int boneIdx)
{
	return (time - static_cast<float>(m_AnimData[m_AnimState]->m_KeyTime[boneIdx])) / static_cast<float>(m_AnimData[m_AnimState]->m_KeyTime[boneIdx + 1] - m_AnimData[m_AnimState]->m_KeyTime[boneIdx]);

}

float AnimationData::GetClampTime(float time)
{
	return (time - ((int)(time / static_cast<float>(m_AnimData[m_AnimState]->m_KeyTime[m_AnimData[m_AnimState]->m_nKeyTime - 1])) * static_cast<float>(m_AnimData[m_AnimState]->m_KeyTime[m_AnimData[m_AnimState]->m_nKeyTime - 1])));

}
