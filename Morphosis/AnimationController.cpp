#include "stdafx.h"
#include "AnimationController.h"
#include "Importer.h"

CAnimationController::~CAnimationController()
{
	for (int i = 0; i < m_AnimData.size(); ++i) delete m_AnimData[i];
	MemoryClear(m_AnimData);
}

void CAnimationController::AddAnimData(AnimationClip * animData)
{
	m_AnimData.push_back(animData);
}
XMMATRIX CAnimationController::GetFinalMatrix(int boneIdx, float time)
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
XMMATRIX CAnimationController::GetInterpolatedToRootMtx(int boneIdx, float time)
{
	int PrevIdx = 0;
	float normalizedTime = 0;
	float l_time = time;
	// 만약 키프레임이 하나 밖에 없으면 맨 첫 번째 ToRoot를 반환
	if (m_AnimData[m_AnimState]->m_nKeyTime == 1) return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[0]);

	// 시간을 비교해서 보간할 idx 두 개를 구해야 함.
	// 시간이 맨 앞보다 빠르면 맨 앞을 반환.
	if (l_time < m_AnimData[m_AnimState]->m_KeyTime[0]) return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[0]);
	// 시간이 맨 뒤보다 늦으면 맨 뒤를 반환.
	else if (l_time > m_AnimData[m_AnimState]->m_KeyTime[m_AnimData[m_AnimState]->m_nKeyTime - 1]) {
		if (false == m_AnimData[m_AnimState]->m_IsLoop) return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[m_AnimData[m_AnimState]->m_nKeyTime - 1]);
	}

	// 보간 값 계산
	l_time = GetClampTime(l_time);
	PrevIdx = GetPrevIdx(l_time);
	int maxIdx = (PrevIdx + 1 > m_AnimData[m_AnimState]->m_nKeyTime) ? PrevIdx : PrevIdx + 1;
	normalizedTime = GetNormalizedTime(l_time, PrevIdx);

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
	auto iter = find_if(m_AnimData.begin(), m_AnimData.end(), [animClipName](AnimationClip* a) { return a->m_AnimName == animClipName; });
	if (iter != m_AnimData.end()) {
		m_AnimState = static_cast<int>( distance(m_AnimData.begin(), iter) );
	}
}
//XMFLOAT4X4 CAnimationController::GetPositionOfBone(const char * animClipName, float time)
//{
//	XMFLOAT4X4 temp = Matrix4x4::Identity();
//	if (m_AnimData.empty()) return temp;
//
//	for (int i = 0; i < m_AnimData[m_AnimState]->m_nBoneList; ++i) {
//		if (m_AnimData[m_AnimState]->m_BoneList[i].m_Name == animClipName)
//			XMStoreFloat4x4(&temp, GetFinalMatrix(i, time));
//	}
//	
//	return temp;
//}
bool CAnimationController::IsClipEnd(float time)
{
	return time >= m_AnimData[m_AnimState]->m_KeyTime[m_AnimData[m_AnimState]->m_nKeyTime - 1];
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

AnimationClip::AnimationClip(const AnimationClip & a)
	: m_AnimName(a.m_AnimName)
	, m_nBoneList(a.m_nBoneList)
	, m_nKeyTime(a.m_nKeyTime)
	, m_BoneList(NULL)
	, m_KeyTime(NULL)
	, m_IsLoop(false)
{

	m_BoneList = new ImportBone[m_nBoneList];
	for (int i = 0; i < m_nBoneList; ++i)
		m_BoneList[i] = a.m_BoneList[i];


	m_KeyTime = new double[m_nKeyTime];
	for (int i = 0; i < m_nKeyTime; ++i)
		m_KeyTime[i] = a.m_KeyTime[i];
}

AnimationClip::~AnimationClip()
{

	if (m_BoneList) {

		//for (int i = 0; i < m_nBoneList; ++i) {
		//	delete[] m_BoneList[i].m_pToRootTransforms;
		//}
		delete[] m_BoneList;
	}
	if (m_KeyTime) delete[] m_KeyTime;
}

void AnimationClip::ImportFile(const char * fileName)
{
	std::ifstream in;

	in.open(fileName, std::ios::in | std::ios::binary);
	char AnimName[32];
	in.read((char*)AnimName, sizeof(AnimName));
	m_AnimName = AnimName;

	m_nKeyTime = 0;
	in.read((char*)&m_nKeyTime, sizeof(int));
	m_KeyTime = new double[m_nKeyTime];
	in.read((char*)m_KeyTime, sizeof(double) * m_nKeyTime);

	in.read((char*)&m_nBoneList, sizeof(int));
	m_BoneList = new ImportBone[m_nBoneList];

	for (int i = 0; i < m_nBoneList; ++i) {
		char name[32];
		in.read((char*)name, sizeof(name));
		m_BoneList[i].m_Name = name;

		in.read((char*)&m_BoneList[i].m_GlobalTransform, sizeof(XMFLOAT4X4));
	}

	for (int i = 0; i < m_nBoneList; ++i) {
		m_BoneList[i].m_nKeyframe = m_nKeyTime;
		m_BoneList[i].m_pToRootTransforms = new XMFLOAT4X4[m_nKeyTime];
		for (int j = 0; j < m_nKeyTime; ++j) {
			in.read((char*)&m_BoneList[i].m_pToRootTransforms[j], sizeof(XMFLOAT4X4));
		}
	}



	in.close();
}

void CHumanoidAnimCtrl::AddAnimData(AnimationClip * animData)
{
	m_AnimData.push_back(animData);
	if (m_vecRotationAngle.empty()) {
		m_vecRotationAngle.resize(animData->m_nBoneList);
	}
}

XMMATRIX CHumanoidAnimCtrl::GetFinalMatrix(int boneIdx, float time)
{
	XMVECTOR det;
	XMMATRIX OffsetInv, ToRootInv;
	det = XMMatrixDeterminant(XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_GlobalTransform));
	OffsetInv = XMMatrixInverse(&det, XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_GlobalTransform));
	ToRootInv = GetInterpolatedToRootMtx(boneIdx, time);

	det = XMMatrixDeterminant(ToRootInv);
	ToRootInv = XMMatrixInverse(&det, ToRootInv);

	//XMMatrixRotationRollPitchYawDegree(m_XAxisRotationAngle, 0, 0);
	//if (m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_Name == "Bip001 Spine2") {
	//	//return XMMatrixMultiply(OffsetInv, XMMatrixMultiply(ToRootInv, XMMatrixRotationRollPitchYawDegree(m_XAxisRotationAngle, 0, 0)));
	//	return XMMatrixMultiply(OffsetInv, XMMatrixMultiply(XMMatrixRotationRollPitchYawDegree(30, 0, 0), ToRootInv));

	//}

	return XMMatrixMultiply(OffsetInv, ToRootInv);
}

XMMATRIX CHumanoidAnimCtrl::GetInterpolatedToRootMtx(int boneIdx, float time)
{
	int PrevIdx = 0;
	float normalizedTime = 0;
	float l_time = time;
	// 만약 키프레임이 하나 밖에 없으면 맨 첫 번째 ToRoot를 반환
	if (m_AnimData[m_AnimState]->m_nKeyTime == 1) return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[0]);

	// 시간을 비교해서 보간할 idx 두 개를 구해야 함.
	// 시간이 맨 앞보다 빠르면 맨 앞을 반환.
	if (l_time < m_AnimData[m_AnimState]->m_KeyTime[0]) return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[0]);
	// 시간이 맨 뒤보다 늦으면 맨 뒤를 반환.
	else if (l_time > m_AnimData[m_AnimState]->m_KeyTime[m_AnimData[m_AnimState]->m_nKeyTime - 1]) {
		if (false == m_AnimData[m_AnimState]->m_IsLoop) return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[m_AnimData[m_AnimState]->m_nKeyTime - 1]);
	}

	// 보간 값 계산
	l_time = GetClampTime(l_time);
	PrevIdx = GetPrevIdx(l_time);
	int maxIdx = (PrevIdx + 1 > m_AnimData[m_AnimState]->m_nKeyTime) ? 0 : PrevIdx + 1;
	if (PrevIdx + 1 > m_AnimData[m_AnimState]->m_nKeyTime) {
		normalizedTime = GetNormalizedTime(l_time, 0);

	}
	else {
		normalizedTime = GetNormalizedTime(l_time, PrevIdx);

	}

	XMFLOAT4X4 result;
	normalizedTime = GetNormalizedTime(l_time, PrevIdx);

	Matrix4x4::InterpolateMtx(&result, m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[PrevIdx], m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[maxIdx], normalizedTime);

	//if (m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_Name == "Bip001 Spine2") {
	//	//return XMMatrixMultiply(OffsetInv, XMMatrixMultiply(ToRootInv, XMMatrixRotationRollPitchYawDegree(m_XAxisRotationAngle, 0, 0)));
	//	//return XMMatrixMultiply(OffsetInv, XMMatrixMultiply(XMMatrixRotationRollPitchYawDegree(30, 0, 0), ToRootInv));

	//	return XMMatrixMultiply(XMLoadFloat4x4(&result), XMMatrixRotationRollPitchYawDegree(0, 0, 90));

	//	//return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[PrevIdx]);

	//}


	 //return XMLoadFloat4x4(&result);
	return XMLoadFloat4x4(&m_AnimData[m_AnimState]->m_BoneList[boneIdx].m_pToRootTransforms[PrevIdx]);
}
