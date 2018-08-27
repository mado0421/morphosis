#include"stdafx.h"
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
			
			m_Bones = new BONES;
			ZeroMemory(m_Bones, sizeof(BONES));
			for (int i = 0; i < m_nBone; ++i)
			{
				m_Bones->bone[i].matrix = m_pBone[i].matrix;
				m_Bones->bone[i].offset = m_pBone[i].offset;
				m_Bones->bone[i].parentIdx = m_pBone[i].parentIdx;
				m_Bones->bone[i].toParent = m_pBone[i].toParent;
			}

			UINT ncbElementBytes = (sizeof(BONES) + 255) & ~255;
			m_pBoneResource = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
				NULL, ncbElementBytes,
				D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
				NULL);
			m_pBoneResource->Map(0, NULL, (void **)&m_pcbMappedBones);

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
	local_time += fElapsedTime/5;
	AnimaionUpdate();
	if (local_time >= endTime)local_time = 0;

	Bone* pBone = m_Bones->bone;
	//	Bone ������Ʈ
	for (int i = 0; i < m_nBone; ++i)
	{
		int paIdx = m_pBone[i].parentIdx;
		if (paIdx != -1) pBone[i].toParent = pBone[paIdx].matrix;
		else pBone[i].toParent = Identity();

		pBone[i].matrix = Identity();
		pBone[i].offset = m_pBone[i].offset;

		pBone[i].matrix = Multiply(pBone[i].matrix, res_matrix[i]);

		//pBone[i].matrix = Multiply(pBone[i].matrix, pBone[i].offset);
		pBone[i].matrix = Multiply(pBone[i].matrix, pBone[i].toParent);

	}


	//	Transpose
	for (int i = 0; i < m_nBone; ++i)
	{
		pBone[i].matrix = Transpose(pBone[i].matrix);
		pBone[i].offset = Transpose(pBone[i].offset);
		//pBone[i].toParent = Transpose(pBone[i].toParent);
	}
}

void AnimationController::AnimaionUpdate()
{
	//	Find current and pred Curve Node to make vector4
	for (int i = 0; i < m_nBone; ++i)
		res_matrix[i] = Identity();

	AllocConsole();
	freopen("CONOUT$", "wt", stdout);

	for (int i = 0; i < m_nCurveNode; ++i)
	{
		XMFLOAT3 trans = XMFLOAT3();
		XMFLOAT3 rotate = XMFLOAT3();
		int nTrans = m_pCurveNode[i].nTranslation;
		int nRotate = m_pCurveNode[i].nRotation;
		//int nScale = m_pCurveNode->nScale;
		cv::Keyframe*	pTrans = m_pCurveNode[i].pTranslation;
		cv::Keyframe*	pRotat = m_pCurveNode[i].pRotation;
		printf("CurveNode [ %d ]\n", m_pCurveNode[i].bone_index);

		if (1 < nTrans)
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
						trans.x = pTrans[nTrans - 1].value.x;
						trans.y = pTrans[nTrans - 1].value.y;
						trans.z = pTrans[nTrans - 1].value.z;
						break;
					}
				}
				/*
				current �� pred�� Interpolation
				*/
				else
				{
					if (fi != 0)
					{

						float t = (pTrans[fi].timePos - local_time) /
							(pTrans[fi].timePos - pTrans[fi - 1].timePos);
						trans.x = t * pTrans[fi - 1].value.x + (1 - t)*pTrans[fi].value.x;
						trans.y = t * pTrans[fi - 1].value.y + (1 - t)*pTrans[fi].value.y;
						trans.z = t * pTrans[fi - 1].value.z + (1 - t)*pTrans[fi].value.z;
						break;
					}
					else
					{
						trans.x = pTrans[0].value.x;
						trans.y = pTrans[0].value.y;
						trans.z = pTrans[0].value.z;
					}
				}
			}
		}
		else if (1 == nTrans)
		{
			trans.x = pTrans[0].value.x;
			trans.y = pTrans[0].value.y;
			trans.z = pTrans[0].value.z;
		}

		if (1 < nRotate)
		{
			for (int fi = 0; fi < nRotate; ++fi)
			{
				if (local_time >= pRotat[fi].timePos) {
					if (fi < nRotate - 1)continue;
					else {
						rotate.x = pRotat[nRotate - 1].value.x;
						rotate.y = pRotat[nRotate - 1].value.y;
						rotate.z = pRotat[nRotate - 1].value.z;
						printf("rv0		(%f, %f, %f)\n", pRotat[0].value.x, pRotat[0].value.y, pRotat[0].value.z);
						break;
					}
				}
				else
				{
					if (fi != 0)
					{
						float t = (pRotat[fi].timePos - local_time) /
							(pRotat[fi].timePos - pRotat[fi - 1].timePos);
						rotate.x = t * pRotat[fi - 1].value.x + (1 - t)*pRotat[fi].value.x;
						rotate.y = t * pRotat[fi - 1].value.y + (1 - t)*pRotat[fi].value.y;
						rotate.z = t * pRotat[fi - 1].value.z + (1 - t)*pRotat[fi].value.z;
						printf("t	%f		", t);
						printf("rv0		(%f, %f, %f)	", pRotat[fi - 1].value.x, pRotat[fi - 1].value.y, pRotat[fi - 1].value.z);
						printf("rv1		(%f, %f, %f)	\n", pRotat[fi].value.x, pRotat[fi].value.y, pRotat[fi].value.z);
						break;
					}
					else
					{
						rotate.x = pRotat[0].value.x;
						rotate.y = pRotat[0].value.y;
						rotate.z = pRotat[0].value.z;
						printf("rv0		(%f, %f, %f)\n", pRotat[0].value.x, pRotat[0].value.y, pRotat[0].value.z);
						break;
					}
				}
			}
		}
		else if (1 == nRotate)
		{
			rotate.x = pRotat[0].value.x;
			rotate.y = pRotat[0].value.y;
			rotate.z = pRotat[0].value.z;

			printf("rv0		(%f, %f, %f)", pRotat[0].value.x, pRotat[0].value.y, pRotat[0].value.z);
		}

		XMFLOAT3 Scaling{ 1,1,1 };
		XMFLOAT4 zero{ 0,0,0 ,1 };
		CheckEpsilon(trans);
		CheckEpsilon(rotate);

		/*printf("Trans  (%f,	%f,	%f)		", trans.x, trans.y, trans.z);
		printf("/	Rotate  (%f, %f, %f\n\n", rotate.x, rotate.y, rotate.z);*/
		printf("/	Rotate  (%f, %f, %f\n\n", rotate.x, rotate.y, rotate.z);
		rotate.x *= 3.14159265358979323846 / 180.0f;
		rotate.y *= 3.14159265358979323846 / 180.0f;
		rotate.z *= 3.14159265358979323846 / 180.0f;

		res_matrix[m_pCurveNode[i].bone_index] = AffineTransformation(Scaling, zero, rotate, trans);

		/*if (m_pCurveNode[i].bone_index == 3)
		{
			if (CheckNan(res_matrix[3]))
			{
				printf("trans %d %d %d\n", trans.x, trans.y, trans.z);
				printf("rotate %d %d %d\n", rotate.x, rotate.y, rotate.z);
			}
		}*/
	}
 }

void AnimationController::BoneUpdate(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedBones, m_Bones, sizeof(BONES));
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

inline XMFLOAT4X4 AnimationController::Inverse(XMFLOAT4X4 & matrix)
{
	XMFLOAT4X4 result;
	
	XMStoreFloat4x4(&result,
		XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&matrix)), XMLoadFloat4x4(&matrix)));
	return result;
}

inline void AnimationController::CheckEpsilon(XMFLOAT3 & v)
{
	if (fabsf(v.x) < FLT_MIN)v.x = 0;
	if (fabsf(v.y) < FLT_MIN)v.y = 0;
	if (fabsf(v.z) < FLT_MIN)v.z = 0;
}

bool AnimationController::CheckNan(XMFLOAT4X4 & matrix)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (_isnan(matrix.m[i][j]))
			{
				printf("Error Nan\n");
				return true;
			}
		}
	}
	return false;
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
