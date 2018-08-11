#include "AnimationController.h"



AnimationController::AnimationController()
{
	m_nBone = 0;
	m_nCurveNode = 0;
	m_pCurveNode = nullptr;
	res_matrix = nullptr;
	m_pBoneResource = nullptr;
}

AnimationController::~AnimationController()
{
}

void AnimationController::Init(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,FBXDataManager * pFbxDM)
{
	if (nullptr != pFbxDM)
	{
		if (0 < pFbxDM->m_nBone)
		{
			m_nBone = pFbxDM->m_nBone;
			assert(m_nBone <= MAXBONE&& "Bone�� ������ ���ġ�� �ʰ���");
			m_pBone = pFbxDM->m_pBone;
			
			ZeroMemory(&m_Bones, sizeof(BONES));

			UINT ncbElementBytes = (sizeof(BONES) + 255) & ~255;
			m_pBoneResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
				NULL, ncbElementBytes,
				D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
				NULL);
			m_pBoneResource->Map(0, NULL, (void **)&m_Bones);

			res_matrix = new XMFLOAT4X4[m_nBone];
			for (int i = 0; i < m_nBone; ++i){
				XMStoreFloat4x4(&res_matrix[i], XMMatrixIdentity());
			}
		}
		else
		{
			printf("ERROR\n Bone�� ���� Fbx ������ AnimationController�� �ʿ�ġ ����\n");
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

					  local_time					
	Node [	|	|	|	|	|	|	...	|	]
					  o	  o
	lerp( index 3 and 4 )

	������� 3���� �ڷḦ ���� ���� ����� ����� ����

	�����ڵ�

	�ΰ� �̻��� �ִϸ��̼��� ���� �Ұ�

*/
void AnimationController::Update(float fElapsedTime ,ID3D12GraphicsCommandList *pd3dCommandList)
{
	local_time += fElapsedTime;
	AnimaionUpdate();
	BoneUpdate(pd3dCommandList);
}

void AnimationController::AnimaionUpdate()
{
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
				if (local_time >= pTrans[fi].timePos) {
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
					float t = (pTrans[fi].timePos - local_time) /
						(pTrans[fi].timePos - pTrans[fi - 1].timePos);
					trans.x = t * pTrans[fi - 1].value.x + (1 - t)*pTrans[fi].value.x;
					trans.y = t * pTrans[fi - 1].value.y + (1 - t)*pTrans[fi].value.y;
					trans.z = t * pTrans[fi - 1].value.z + (1 - t)*pTrans[fi].value.z;
					break;
				}

			}
		}

		if (0 < nRotate)
		{
			for (int fi = 0; fi < nRotate; ++fi)
			{
				if (local_time >= pRotat[fi].timePos) {
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
					float t = (pRotat[fi].timePos - local_time) /
						(pRotat[fi].timePos - pRotat[fi - 1].timePos);
					rotate.x = t * pRotat[fi - 1].value.x + (1 - t)*pRotat[fi].value.x;
					rotate.y = t * pRotat[fi - 1].value.y + (1 - t)*pRotat[fi].value.y;
					rotate.z = t * pRotat[fi - 1].value.z + (1 - t)*pRotat[fi].value.z;
					break;
				}
			}
		}

		XMFLOAT3 Scaling{ 1,1,1 };
		XMFLOAT4 zero{ 0,0,0 ,1 };
		res_matrix[i] = AffineTransformation(Scaling, zero, rotate, trans);
	}
}

void AnimationController::BoneUpdate(ID3D12GraphicsCommandList *pd3dCommandList)
{
	Bone* pBone = m_Bones.bone;
	//	Bone ������Ʈ
	for (int i = 0; i < m_nBone; ++i)
	{
		/*
			to hierachy transform
			first local transformation in local coordinate
			second transfrom coordinate into parent's
		*/
		pBone[i].matrix = res_matrix[i];
		pBone[i].matrix = Multiply(pBone[i].matrix, pBone[i].toParent);
		pBone[i].matrix = Transpose(res_matrix[i]);
	}
	
	//	Resource ���
	pd3dCommandList->SetGraphicsRootConstantBufferView(
		ROOT_PARAMETER_BONES, m_pBoneResource->GetGPUVirtualAddress());
}

inline XMFLOAT4X4 AnimationController::AffineTransformation(XMFLOAT3& Scaling, XMFLOAT4& RotationOrigin,
	XMFLOAT3& Rotation, XMFLOAT3& Translation)
{
	XMFLOAT4X4 xmmtx4x4Result;

	XMStoreFloat4x4(&xmmtx4x4Result,
		XMMatrixAffineTransformation(
			XMLoadFloat3(&Scaling),
			XMLoadFloat4(&RotationOrigin),
			XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&Rotation)),
			XMLoadFloat3(&Translation))
	);
	return(xmmtx4x4Result);
}

inline XMFLOAT4X4 AnimationController::Transpose(XMFLOAT4X4& xmmtx4x4Matrix)
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
	return(xmmtx4x4Result);
}

inline XMFLOAT4X4 AnimationController::Identity()
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
	return(xmmtx4x4Result);
}

inline XMFLOAT4X4 AnimationController::Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));
	return(xmmtx4x4Result);
}

/*
end local_time�� 153953860000�� �ִϸ��̼��� ������ FBX����
CurveNode�� TimePos��
... , 92372316000, 107767702000
... , 153953860000
�� ���� ���� �ð��� ������ ���� �������� ���� ��찡 �ִ�

local_time 107767702000 �� �����ϰ� �ִ� ���� �ش� �ð��� �Ѿ��
EndTime�� �� �� ���� ������ ��常�� ���� �����ؾ� �ϰ�

local_time 153953860000 �� �����ϰ� �ִ� ���� �ִϸ��̼��� ���ᰡ �Ǵ� �ð��̸�
�ش� Curve �ڸ� ���� ������� �����Ƿ� ��� ���� ���̴�.

*/