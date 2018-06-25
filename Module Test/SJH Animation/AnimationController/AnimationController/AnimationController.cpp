#include "AnimationController.h"



AnimationController::AnimationController()
{
	m_nBone = 0;
	m_nCurveNode = 0;
	m_pBone = nullptr;
	m_pCurveNode = nullptr;
}

AnimationController::~AnimationController()
{
}

void AnimationController::Init(FBXDataManager * pFbxDM)
{
	if (nullptr != pFbxDM)
	{
		if (0 < pFbxDM->m_nBone)
		{
			m_nBone = pFbxDM->m_nBone;
			m_pBone = pFbxDM->m_pBone;
		}
		else
		{

		}

		if (0 < pFbxDM->m_nCurveNode)
		{
			m_nCurveNode = pFbxDM->m_nCurveNode;
			m_pCurveNode = pFbxDM->m_pCurveNode;
		}
		else
		{
			//	has no animation
		}
	}
}

/*
	이전 소스 형태
	내용 다시 보충해야함

	각 본들은
	Trans, Rotate, Scale 의 시간에 따른
	변형을 위한 Vector3를 갖고있음
	현재 시간을 포함하는 노드 2개를 찾아
	보간한 수치를 제공해야 함

					  time					
	Node [	|	|	|	|	|	|	...	|	]
					  o	  o
	lerp( index 3 and 4 )

	만들어진 3개의 자료를 갖고 최종 행렬을 만들면 성공

	이전코드

	두개 이상의 애니메이션은 보간 불가

*/
void AnimationController::Update(float fElapsedTime)
{
	time += fElapsedTime;
	int nTrans = m_pCurveNode->nTranslation;
	int nRotate = m_pCurveNode->nRotation;
	//int nScale = m_pCurveNode->nScale;

	cv::Keyframe*	pTrans = m_pCurveNode->pTranslation;
	cv::Keyframe*	pRotat = m_pCurveNode->pRotation;
	//cv::Keyframe*	pScale = m_pCurveNode->pScale;
	/*
		Scale은 사용 안함
	*/

	for (int i = 0; i < m_nCurveNode; ++i)
	{
		if (0 < nTrans)
		{
			for (int fi = 0; nTrans; ++fi)
			{
				
			}
		}

		if (0 < nRotate)
		{
			for (int fi = 0; nRotate; ++fi)
			{

			}
		}

		/*if (0 < nScale)
		{
			for (int fi = 0; nScale; ++fi)
			{

			}
		}*/
	}
}
