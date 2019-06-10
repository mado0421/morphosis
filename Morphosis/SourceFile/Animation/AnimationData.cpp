#include "stdafx.h"
#include "AnimationData.h"
#include "Importer/Importer.h"

void AnimationData::Init(ImportAnimData & animData)
{
	ImportAnimData* tmp = new ImportAnimData(animData);

	m_AnimData.push_back(tmp);
}
void AnimationData::AddAnimData(ImportAnimData * animData)
{
	m_AnimData.push_back(animData);
}
XMMATRIX AnimationData::GetFinalMatrix(int boneIdx, float time)
{
	XMVECTOR det;
	XMMATRIX OffsetInv, ToRootInv;
	det = XMMatrixDeterminant(XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_GlobalTransform));
	OffsetInv = XMMatrixInverse(&det, XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_GlobalTransform));
	ToRootInv = GetInterpolatedToRootMtx(boneIdx, time);

	det = XMMatrixDeterminant(ToRootInv);
	ToRootInv = XMMatrixInverse(&det, ToRootInv);
	return XMMatrixMultiply(OffsetInv, ToRootInv);
}
XMMATRIX AnimationData::GetInterpolatedToRootMtx(int boneIdx, float time)
{
	int PrevIdx = 0;
	float normalizedTime = 0;
	// ���� Ű�������� �ϳ� �ۿ� ������ �� ù ��° ToRoot�� ��ȯ
	if (m_AnimData[m_AnimState]->m_nKeyTime == 1) return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[0]);
	//if (isFurtherThanBack(time)) if (isLoop) time = GetClampTime(time);

	// �ð��� ���ؼ� ������ idx �� ���� ���ؾ� ��.
	// �ð��� �� �պ��� ������ �� ���� ��ȯ.
	if (time < m_AnimData[m_AnimState]->m_KeyTime[0]) return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[0]);
	// �ð��� �� �ں��� ������ �� �ڸ� ��ȯ.
	else if (time > m_AnimData[m_AnimState]->m_KeyTime[m_AnimData[m_AnimState]->m_nKeyTime - 1]) {
		if (false == isLoop) return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[m_AnimData[m_AnimState]->m_nKeyTime - 1]);
	}

	// ���� �� ���
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
