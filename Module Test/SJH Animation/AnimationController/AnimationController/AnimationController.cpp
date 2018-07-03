#include "AnimationController.h"



AnimationController::AnimationController()
{
	m_nBone = 0;
	m_nCurveNode = 0;
	m_pBone = nullptr;
	m_pCurveNode = nullptr;
	m_pMatrix = nullptr;
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
			m_pMatrix = new AnimMatrix[m_nCurveNode];

			for (int i = 0; i < m_nCurveNode; ++i)
			{
				m_pMatrix[i].bone_idx = m_pCurveNode->bone_index;
				XMStoreFloat4x4(&m_pMatrix[i].mat, XMMatrixIdentity());
			}
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

	//cv::Keyframe*	pScale = m_pCurveNode->pScale;
	/*
		Scale�� ��� ����
	*/
	//XMFLOAT4 scale;

	if (cnt % sampleCount == 0)
	{
		std::cout << "timepos: " << time << "\n";
	}
	//	Find current and pred Curve Node to make vector4
	for (int i = 0; i < m_nCurveNode; ++i)
	{
		XMFLOAT3 trans = XMFLOAT3();
		XMFLOAT3 rotate = XMFLOAT3();
		int nTrans = m_pCurveNode[i].nTranslation;
		int nRotate = m_pCurveNode[i].nRotation;
		//int nScale = m_pCurveNode->nScale;
		cv::Keyframe*	pTrans = m_pCurveNode[i].pTranslation;
		cv::Keyframe*	pRotat = m_pCurveNode[i].pRotation;


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
						trans.x = pTrans[0].value.x;
						trans.y = pTrans[0].value.y;
						trans.z = pTrans[0].value.z;
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

					if (b_print_TransInfo&&cnt % sampleCount == 0)
					{
						std::cout << "CurveNode " << i << "\n";
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

		if (0 < nRotate)
		{
			for (int fi = 0; fi < nRotate; ++fi)
			{
				if (time >= pRotat[fi].timePos) {
					if (fi < nRotate - 1)continue;
					else {
						rotate.x = pRotat[0].value.x;
						rotate.y = pRotat[0].value.y;
						rotate.z = pRotat[0].value.z;
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

					if (b_print_RotateInfo&&cnt % sampleCount == 0)
					{
						std::cout << "CurveNode " << i << "\n";
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

		XMFLOAT3 Scaling{ 1,1,1 };
		XMFLOAT4 zero{ 0,0,0 ,1 };
		m_pMatrix[i].mat = Matrix4x4::AffineTransformation(Scaling, zero, rotate, trans);
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