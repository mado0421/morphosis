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
	// 만약 키프레임이 하나 밖에 없으면 맨 첫 번째 ToRoot를 반환
	if (g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.size() == 1)
		return XMLoadFloat4x4(&g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecBone[boneIdx].m_vecToRootTransform.front());

	// 시간을 비교해서 보간할 idx 두 개를 구해야 함.
	// 시간이 맨 앞보다 빠르면 맨 앞을 반환.
	if (l_time <= g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.front())
		return XMLoadFloat4x4(&g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecBone[boneIdx].m_vecToRootTransform.front());
	// 시간이 맨 뒤보다 늦으면 맨 뒤를 반환.
	else if (l_time >= g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.back()) 
		if (false == g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_IsLoop) 
			return XMLoadFloat4x4(&g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecBone[boneIdx].m_vecToRootTransform.back());
	
	// 보간 값 계산
	l_time = GetClampTime(l_time);
	PrevIdx = GetPrevIdx(l_time);

	if (PrevIdx == g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.size() - 1) {
		NextIdx = 0;
	}
	else NextIdx = PrevIdx + 1;

	//int maxIdx = (PrevIdx + 1 > g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime.size()) ? PrevIdx : PrevIdx + 1;
	normalizedTime = GetNormalizedTime(l_time, PrevIdx);

	// 아래 계산한거 안 쓰고 있음.
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
	return -1; // 오류!
}

float AnimCtrl::GetNormalizedTime(float time, int boneIdx)
{
	if (static_cast<float>(g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime[boneIdx + 1] - g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime[boneIdx] != 0))
		return ((time - g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime[boneIdx])
			/ (g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime[boneIdx + 1] - g_vecAnimClip[m_vecClipIdx[m_AnimState]]->m_vecKeyTime[boneIdx]));
	return 0.0f;
}
