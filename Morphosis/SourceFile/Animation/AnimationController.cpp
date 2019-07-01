#include "stdafx.h"
#include "AnimationController.h"
#include "Importer/Importer.h"

void CAnimationController::Init(AnimationClip & animData)
{
	AnimationClip* tmp = new AnimationClip(animData);

	m_AnimData.push_back(tmp);
}
void CAnimationController::AddAnimData(AnimationClip * animData)
{
	m_AnimData.push_back(animData);
}
XMMATRIX CAnimationController::GetFinalMatrix(int boneIdx)
{
	XMVECTOR det;
	XMMATRIX OffsetInv, ToRootInv;
	det = XMMatrixDeterminant(XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_GlobalTransform));
	OffsetInv = XMMatrixInverse(&det, XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_GlobalTransform));
	ToRootInv = GetInterpolatedToRootMtx(boneIdx);

	det = XMMatrixDeterminant(ToRootInv);
	ToRootInv = XMMatrixInverse(&det, ToRootInv);
	return XMMatrixMultiply(OffsetInv, ToRootInv);
}
XMMATRIX CAnimationController::GetInterpolatedToRootMtx(int boneIdx)
{
	int PrevIdx = 0;
	float normalizedTime = 0;
	float time = m_fTime;
	// ���� Ű�������� �ϳ� �ۿ� ������ �� ù ��° ToRoot�� ��ȯ
	if (m_AnimData[m_AnimState]->m_nKeyTime == 1) return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[0]);

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
	int maxIdx = (PrevIdx + 1 > m_AnimData[m_AnimState]->m_nKeyTime) ? PrevIdx : PrevIdx + 1;
	normalizedTime = GetNormalizedTime(time, PrevIdx);

	XMFLOAT4X4 result;

	Matrix4x4::InterpolateMtx(&result, m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[PrevIdx], m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[maxIdx], normalizedTime);

	// return XMLoadFloat4x4(&result);
	return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[PrevIdx]);
}
XMMATRIX CAnimationController::GetOffset(int boneIdx)
{
	return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_GlobalTransform);
}
float CAnimationController::GetEndTime()
{
	return static_cast<float>(m_AnimData[m_AnimState]->m_KeyTime[m_AnimData[m_AnimState]->m_nKeyTime - 1]);
}
void CAnimationController::ChangeAnimClip(const char * animClipName)
{
	/*********************************************************************
	2019-06-16
	���� �ִϸ��̼� ��� �������?
	m_AnimState�� �����ϴ��� ������
	�׷� distance�� ���ؼ� ������ �ǰڴ�.
	*********************************************************************/
	auto iter = find_if(m_AnimData.begin(), m_AnimData.end(), [animClipName](AnimationClip* a) { return a->m_AnimName == animClipName; });
	if (iter != m_AnimData.end()) {
		m_fTime = 0.0f;
		m_AnimState = static_cast<int>( distance(m_AnimData.begin(), iter) );
	}
}
void CAnimationController::Update(float fTime)
{
	m_fTime += fTime;
}
XMFLOAT4X4 CAnimationController::GetPositionOfBone(const char * animClipName)
{
	XMFLOAT4X4 temp = Matrix4x4::Identity();
	for (int i = 0; i < m_AnimData[m_AnimState]->m_nBoneList; ++i) {
		if (m_AnimData[m_AnimState]->m_BoneList[i].m_Name == animClipName)
			XMStoreFloat4x4(&temp, GetFinalMatrix(i));
	}
	
	return temp;
}
bool CAnimationController::IsClipEnd()
{
	return m_fTime >= m_AnimData[m_AnimState]->m_KeyTime[m_AnimData[m_AnimState]->m_nKeyTime - 1];
}
int CAnimationController::GetPrevIdx(float time)
{
	for (int i = 0; i < m_AnimData[m_AnimState]->m_nKeyTime; ++i)
		if (m_AnimData[m_AnimState]->m_KeyTime[i] > time) return i;
	return 0;
}
float CAnimationController::GetNormalizedTime(float time, int boneIdx)
{
	if(static_cast<float>(m_AnimData[m_AnimState]->m_KeyTime[boneIdx + 1] - m_AnimData[m_AnimState]->m_KeyTime[boneIdx] != 0))
		return (time - static_cast<float>(m_AnimData[m_AnimState]->m_KeyTime[boneIdx])) / static_cast<float>(m_AnimData[m_AnimState]->m_KeyTime[boneIdx + 1] - m_AnimData[m_AnimState]->m_KeyTime[boneIdx]);
	return 0.0f;
}
float CAnimationController::GetClampTime(float time)
{
	return (time - ((int)(time / static_cast<float>(m_AnimData[m_AnimState]->m_KeyTime[m_AnimData[m_AnimState]->m_nKeyTime - 1])) * static_cast<float>(m_AnimData[m_AnimState]->m_KeyTime[m_AnimData[m_AnimState]->m_nKeyTime - 1])));
}
