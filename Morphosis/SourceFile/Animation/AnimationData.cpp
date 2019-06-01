#include "stdafx.h"
#include "AnimationData.h"
#include "Importer/Importer.h"

void AnimationData::Init(ImportAnimData & animData)
{
	m_AnimData = new ImportAnimData(animData);
}

XMMATRIX AnimationData::GetFinalMatrix(int boneIdx, float time)
{
	//return XMMatrixMultiply(XMLoadFloat4x4(&m_bones[boneIdx].m_dressposeInv), XMLoadFloat4x4(&m_bones[boneIdx].m_toWorld));
	return XMMatrixMultiply(XMLoadFloat4x4(&m_AnimData->m_BoneList[boneIdx].m_GlobalTransform), GetInterpolatedToRootMtx(boneIdx, time));

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
	// ���� Ű�������� �ϳ� �ۿ� ������ �� ù ��° ToRoot�� ��ȯ
	if (m_AnimData->m_nKeyTime == 1) return XMLoadFloat4x4(&m_AnimData->m_BoneList[boneIdx].m_pToRootTransforms[0]);
	//if (isFurtherThanBack(time)) if (isLoop) time = GetClampTime(time);

	// �ð��� ���ؼ� ������ idx �� ���� ���ؾ� ��.
	// �ð��� �� �պ��� ������ �� ���� ��ȯ.
	if (time < m_AnimData->m_KeyTime[0]) return XMLoadFloat4x4(&m_AnimData->m_BoneList[boneIdx].m_pToRootTransforms[0]);
	// �ð��� �� �ں��� ������ �� �ڸ� ��ȯ.
	else if (time > m_AnimData->m_KeyTime[m_AnimData->m_nKeyTime - 1]) {
		if (false == isLoop) return XMLoadFloat4x4(&m_AnimData->m_BoneList[boneIdx].m_pToRootTransforms[m_AnimData->m_nKeyTime - 1]);
	}

	// ���� �� ���
	time = GetClampTime(time);
	PrevIdx = GetPrevIdx(time);
	normalizedTime = GetNormalizedTime(time, PrevIdx);

	XMFLOAT4X4 result;

	Matrix4x4::InterpolateMtx(&result, m_AnimData->m_BoneList[boneIdx].m_pToRootTransforms[PrevIdx], m_AnimData->m_BoneList[boneIdx].m_pToRootTransforms[PrevIdx + 1], normalizedTime);

	// return XMLoadFloat4x4(&result);
	return XMLoadFloat4x4(&m_AnimData->m_BoneList[boneIdx].m_pToRootTransforms[PrevIdx]);
}

int AnimationData::GetPrevIdx(float time)
{
	for (int i = 0; i < m_AnimData->m_nKeyTime; ++i)
		if (m_AnimData->m_KeyTime[i] > time) return i;
	return 0;
}

float AnimationData::GetNormalizedTime(float time, int boneIdx)
{
	return (time - static_cast<float>(m_AnimData->m_KeyTime[boneIdx])) / static_cast<float>(m_AnimData->m_KeyTime[boneIdx + 1] - m_AnimData->m_KeyTime[boneIdx]);

}

float AnimationData::GetClampTime(float time)
{
	return (time - ((int)(time / static_cast<float>(m_AnimData->m_KeyTime[m_AnimData->m_nKeyTime - 1])) * static_cast<float>(m_AnimData->m_KeyTime[m_AnimData->m_nKeyTime - 1])));

}
