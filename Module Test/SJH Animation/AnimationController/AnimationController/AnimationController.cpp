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
	���� �ҽ� ����
	���� �ٽ� �����ؾ���

	�� ������
	Trans, Rotate, Scale �� �ð��� ����
	������ ���� Vector3�� ��������
	���� �ð��� �����ϴ� ��� 2���� ã��
	������ ��ġ�� �����ؾ� ��

					  time					
	Node [	|	|	|	|	|	|	...	|	]
					  o	  o
	lerp( index 3 and 4 )

	������� 3���� �ڷḦ ���� ���� ����� ����� ����

	�����ڵ�

	�ΰ� �̻��� �ִϸ��̼��� ���� �Ұ�

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
		Scale�� ��� ����
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
