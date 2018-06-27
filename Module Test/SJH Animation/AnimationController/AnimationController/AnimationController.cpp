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

		endTime = 3.0f;
		/*
			�ӽ�, ���� ���߿� �����Ͽ� ����ϴ� �ִϸ��̼� ������ CuveSet���� EndTIme�� ���ԵǾ� �־
			������� ���� ����
		*/
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
	static int cnt = 0;
	static const int sampleCount = 100;
	cnt++;


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
	XMFLOAT3 trans = XMFLOAT3();
	XMFLOAT3 rotate = XMFLOAT3();
	//XMFLOAT4 scale;

	if (cnt % sampleCount == 0)
	{
		std::cout << "timepos: " << time << "\n";
	}
	//	Find current and pred Curve Node to make vector4
	for (int i = 0; i < m_nCurveNode; ++i)
	{
		if (0 < nTrans)
		{
			for (int fi = 0; fi < nTrans; ++fi)
			{
				if (time >= pTrans[fi].timePos) {
					if (fi < nTrans - 1)continue;
					else {
						/*
							���� �ð��� ������ keyFrame�� Timepos���� ũ����
							EndTime�� �ƴҶ��� ó��
						*/
						trans.x = pTrans[fi].value.x;
						trans.y = pTrans[fi].value.y;
						trans.z = pTrans[fi].value.z;
						break;
					}
				}
				/*
					current �� pred�� Interpolation
				*/
				else
				{
					float t = (pTrans[fi].timePos - time) /
						(pTrans[fi].timePos - pTrans[fi - 1].timePos);
					trans.x = t * pTrans[fi - 1].value.x + (1 - t)*pTrans[fi].value.x;
					trans.y = t * pTrans[fi - 1].value.y + (1 - t)*pTrans[fi].value.y;
					trans.z = t * pTrans[fi - 1].value.z + (1 - t)*pTrans[fi].value.z;

					if (cnt % sampleCount == 0)
					{
						std::cout << "CurveNode "<<i<<"\n";
						std::cout << "===============================================================\n";
						std::cout << "t = " << t << "\n";
						std::cout << "pred (" << pTrans[fi - 1].value.x << ", " << pTrans[fi - 1].value.y << ", " << pTrans[fi - 1].value.z << ")\n";
						std::cout << "curr (" << pTrans[fi].value.x << ", " << pTrans[fi].value.y << ", " << pTrans[fi].value.z << ")\n";
						std::cout << "Trans (" << trans.x << ", " << trans.y << ", " << trans.z << ")\n";
						std::cout << "===============================================================\n";
					}
					break;
				}

			}
		}
	}

	if (0 < nRotate)
	{
		for (int fi = 0; fi < nRotate; ++fi)
		{
			if (time >= pRotat[fi].timePos) {
				if (fi > 0 || fi < nRotate - 1)continue;
				else {
					rotate.x = pRotat[fi].value.x;
					rotate.y = pRotat[fi].value.y;
					rotate.z = pRotat[fi].value.z;
					break;
				}
			}
			else
			{
				float t = (pRotat[fi].timePos - time) /
					(pRotat[fi].timePos - pRotat[fi - 1].timePos);
				rotate.x = t * pRotat[fi - 1].value.x + (1 - t)*pRotat[fi].value.x;
				rotate.y = t * pRotat[fi - 1].value.y + (1 - t)*pRotat[fi].value.y;
				rotate.z = t * pRotat[fi - 1].value.z + (1 - t)*pRotat[fi].value.z;

				if (cnt % sampleCount == 0)
				{
					std::cout << "===============================================================\n";
					std::cout << "t = " << t << "\n";
					std::cout << "pred (" << pRotat[fi - 1].value.x << ", " << pRotat[fi - 1].value.y << ", " << pRotat[fi - 1].value.z << ")\n";
					std::cout << "curr (" << pRotat[fi].value.x << ", " << pRotat[fi].value.y << ", " << pRotat[fi].value.z << ")\n";
					std::cout << "Rotate (" << rotate.x << ", " << rotate.y << ", " << rotate.z << ")\n";
					std::cout << "===============================================================\n";
				}
				break;
			}
		}
	}
}


	

/*
end time�� 153953860000�� �ִϸ��̼��� ������ FBX����
CurveNode�� TimePos��
... , 92372316000, 107767702000
... , 153953860000
�� ���� ���� �ð��� ������ ���� �������� ���� ��찡 �ִ�

time 107767702000 �� �����ϰ� �ִ� ���� �ش� �ð��� �Ѿ��
EndTime�� �� �� ���� ������ ��常�� ���� �����ؾ� �ϰ�

time 153953860000 �� �����ϰ� �ִ� ���� �ִϸ��̼��� ���ᰡ �Ǵ� �ð��̸�
�ش� Curve �ڸ� ���� ������� �����Ƿ� ��� ���� ���̴�.

*/