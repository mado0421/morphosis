#include "stdafx.h"
#include "AnimCtrl.h"

AnimCtrl::AnimCtrl()
{
}

void AnimCtrl::AddAnimClip(const char * animName)
{
	for (int i = 0; i < g_vecAnimClip.size(); ++i) if (g_vecAnimClip[i]->m_AnimName == animName) m_vecClipIdx.push_back(i);
}

int AnimCtrl::GetNumOfBone()
{
	return g_vecAnimClip[m_vecClipIdx[0]]->m_vecBone.size();
}

XMMATRIX AnimCtrl::GetAnimationMatrix(int boneIdx, float time)
{
	XMVECTOR det;
	XMMATRIX OffsetInv, ToRootInv;
	
	det = XMMatrixDeterminant(XMLoadFloat4x4(&g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecBone[boneIdx].m_xmf4x4GlobalTransform));
	OffsetInv = XMMatrixInverse(&det, XMLoadFloat4x4(&g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecBone[boneIdx].m_xmf4x4GlobalTransform));
	ToRootInv = GetInterpolatedToRootMtx(boneIdx, time);

	det = XMMatrixDeterminant(ToRootInv);
	ToRootInv = XMMatrixInverse(&det, ToRootInv);
	return XMMatrixMultiply(OffsetInv, ToRootInv);
}

XMMATRIX AnimCtrl::GetInterpolatedToRootMtx(int boneIdx, float time)
{
	int PrevIdx = 0;
	int NextIdx = 0;
	float normalizedTime = 0;
	float l_time = time;
	// ���� Ű�������� �ϳ� �ۿ� ������ �� ù ��° ToRoot�� ��ȯ
	if (g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.size() == 1)
		return XMLoadFloat4x4(&g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecBone[boneIdx].m_vecToRootTransform.front());

	// �ð��� ���ؼ� ������ idx �� ���� ���ؾ� ��.
	// �ð��� �� �պ��� ������ �� ���� ��ȯ.
	if (l_time <= g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.front())
		return XMLoadFloat4x4(&g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecBone[boneIdx].m_vecToRootTransform.front());
	// �ð��� �� �ں��� ������ �� �ڸ� ��ȯ.
	else if (l_time >= g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.back()) 
		if (false == g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_IsLoop) 
			return XMLoadFloat4x4(&g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecBone[boneIdx].m_vecToRootTransform.back());
	
	// ���� �� ���
	l_time = GetClampTime(l_time);
	PrevIdx = GetPrevIdx(l_time);

	if (PrevIdx == g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.size() - 1) {
		NextIdx = 0;
	}
	else NextIdx = PrevIdx + 1;

	//int maxIdx = (PrevIdx + 1 > g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.size()) ? PrevIdx : PrevIdx + 1;
	normalizedTime = GetNormalizedTime(l_time, PrevIdx);

	// �Ʒ� ����Ѱ� �� ���� ����.
	XMFLOAT4X4 result;

	Matrix4x4::InterpolateMtx(&result, 
		g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecBone[boneIdx].m_vecToRootTransform[PrevIdx], 
		g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecBone[boneIdx].m_vecToRootTransform[NextIdx],
		normalizedTime);

	 return XMLoadFloat4x4(&result);
	//return XMLoadFloat4x4(&g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecBone[boneIdx].m_vecToRootTransform[PrevIdx]);
}

float AnimCtrl::GetClampTime(float time)
{
	return (time - ((int)(time / static_cast<double>(g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.back()))
		* static_cast<double>(g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.back())));

}

int AnimCtrl::GetPrevIdx(float time)
{
	if (time <= g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.front()) return g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.size()-1;
	for (int i = 0; i < g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.size(); ++i)
		if (time < g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime[i]) return i - 1;
	return -1; // ����!
}

float AnimCtrl::GetNormalizedTime(float time, int boneIdx)
{
	if (static_cast<float>(g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime[boneIdx + 1] - g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime[boneIdx] != 0))
		return ((time - g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime[boneIdx])
			/ (g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime[boneIdx + 1] - g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime[boneIdx]));
	return 0.0f;
}
