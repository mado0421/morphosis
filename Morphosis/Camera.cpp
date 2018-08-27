#include "stdafx.h"
#include "Camera.h"
#include "Object.h"


CCamera::CCamera()
{
	m_xmf4x4View		= Matrix4x4::Identity();
	m_xmf4x4Projection	= Matrix4x4::Identity();
	m_d3dViewport		= { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_d3dScissorRect	= { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
	m_xmf3Position		= XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right			= XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Look			= XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Up			= XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Offset		= XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fTimeLag			= 0.0f;
	m_xmf3LookAtWorld	= XMFLOAT3(0.0f, 0.0f, 0.0f);
}

CCamera::~CCamera()
{
}

void CCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX	= float(xTopLeft);
	m_d3dViewport.TopLeftY	= float(yTopLeft);
	m_d3dViewport.Width		= float(nWidth);
	m_d3dViewport.Height	= float(nHeight);
	m_d3dViewport.MinDepth	= fMinZ;
	m_d3dViewport.MaxDepth	= fMaxZ;
}

void CCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	//	XMMATRIX xmmtxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	//	XMStoreFloat4x4(&m_xmf4x4Projection, xmmtxProjection);
}

void CCamera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf3LookAtWorld = xmf3LookAt;
	m_xmf3Up = xmf3Up;

	GenerateViewMatrix();
}

void CCamera::GenerateViewMatrix()
{
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, m_xmf3LookAtWorld, m_xmf3Up);
}

void CCamera::RegenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;
	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);
}

void CCamera::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255); //256�� ���
	m_pd3dcbCamera = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbCamera->Map(0, NULL, (void **)&m_pcbMappedCamera);
}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	RegenerateViewMatrix();

	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	::memcpy(&m_pcbMappedCamera->m_xmf3Position, &m_xmf3Position, sizeof(XMFLOAT3));

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbCamera->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(RootParameter::CAMERA, d3dGpuVirtualAddress);
}

void CCamera::ReleaseShaderVariables()
{
	if (m_pd3dcbCamera)
	{
		m_pd3dcbCamera->Unmap(0, NULL);
		m_pd3dcbCamera->Release();
	}
}

void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

CFollowCamera::CFollowCamera() : CCamera()
{
	SetTimeLag(0.1f);
	SetOffset(XMFLOAT3(0.0f, 40.0f, -100.0f));	// �÷��̾� ���� ���̿� ���� ���� �ʿ� ����
												// �� �κ��� ���߿� ���ߵǴϱ� �������
	GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);

	SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
}

CFollowCamera::~CFollowCamera()
{
}

void CFollowCamera::SetTarget(void * target)
{
	m_pTarget = (CObject*)target;
	XMFLOAT3 pos = m_pTarget->GetPosition();
	/*�������� ���ϰ� �������� �־� �������� ���� ��Ų �ڿ� LookAt�� ���� ������ 
	���� ��ġ�� �ٶ󺸷��� ���� ��ġ�鼭 �ٶ󺸴� ���� ���Ͱ� (0, 0, 0)�� �Ǳ� ������
	������ ����*/
//	SetPosition(pos);
	SetLookAt(pos);
}

CObject * CFollowCamera::GetTarget()
{
	return m_pTarget;
}

void CFollowCamera::Update(float fTimeElapsed)
{
	if (m_pTarget)
	{
		/*
		ī�޶� ���� ��ġ���� Ÿ���� ��ġ�� �̵��ϰ� �ϰ� �ʹ�.
		���� ī�޶��� ��ġ�� ���ϴ� ����� ī�޶� ��ġ += ī�޶� ������ ��
		���� ī�޶� ��ġ���� ���� ������ ���� ���� �̵��� ��Ų �� �ٽ� ������ ���� ���ؾ� ��.
		*/

		/*
		ī�޶� ���� ������ ȸ����Ű���� Ÿ���� �� ���Ϳ� ī�޶� ���� ������ �� ���͸� ���� ��
		������ �Ͽ� ���հ��� ���ϰ� ������ �Ͽ� ȸ�� ������� ���� �ڿ� ������ ��

		y�� �������� ���� �缭 �װɷ� ȸ����Ű��
		x�൵ �ؾ��ϳ� z���� �� �ʿ� ����

		*/
		m_xmf3Position.x -= m_xmf3Offset.x;
		m_xmf3Position.y -= m_xmf3Offset.y;
		m_xmf3Position.z -= m_xmf3Offset.z;

		XMFLOAT3 targetLook = m_pTarget->GetLook();
		XMFLOAT3 targetRight = m_pTarget->GetRight();
		XMFLOAT3 targetUp = m_pTarget->GetUp();
		XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

		XMFLOAT3 a = Vector3::CrossProduct(up, targetLook);
		XMFLOAT3 b = Vector3::CrossProduct(up, m_xmf3Look);

		XMFLOAT3 A = XMFLOAT3(7, 3, -2);

		XMVECTOR AA = XMLoadFloat3(&a);
		XMVECTOR BB = XMLoadFloat3(&b);

		XMStoreFloat3(&A, XMVector3Cross(AA, BB));

		if (A.y < 0) {
			printf("right\n");

		}
		else
		{
			printf("left\n");
			targetUp = Vector3::ScalarProduct(targetUp, -1);
		}



		XMVECTOR tempTargetLook = XMLoadFloat3(&a);
		XMVECTOR temp = XMLoadFloat3(&b);
		XMStoreFloat3(&a, XMVector3AngleBetweenNormals(tempTargetLook, temp));
		float fInnerAngle = a.x;

//		if (Vector3::CrossProduct(targetLook, temp).y < 0) fInnerAngle *= -1;
		XMMATRIX xmmtxRotate;
		xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&targetUp), fInnerAngle * fTimeElapsed * CAM_ROTATE_SPEED);

		if (fabs(fInnerAngle) >= 0.05) {


			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
			m_xmf3Offset = Vector3::TransformNormal(m_xmf3Offset, xmmtxRotate);
		}
		else
		{
			m_xmf3Look = targetLook;
			m_xmf3Right = targetRight;
		}

		/*���� ȸ������ �̵� ���� �غ���.*/
		XMFLOAT3 targetPos = m_pTarget->GetPosition();
		if (!IsZero(targetPos.x - m_xmf3Position.x)) {
			/*���� �Ÿ� �̻��̸� ������ 0.7�踸ŭ, ���� �Ÿ� ���ϸ� �� ������ ��ü*/
			m_xmf3Position.x += (targetPos.x - m_xmf3Position.x) * CAM_MOVE_SPEED * fTimeElapsed;
			if (fabs(m_xmf3Position.x - targetPos.x) < 0.2f) m_xmf3Position.x = targetPos.x;
		}

		if (!IsZero(targetPos.y - m_xmf3Position.y)) {
			/*���� �Ÿ� �̻��̸� ������ 0.7�踸ŭ, ���� �Ÿ� ���ϸ� �� ������ ��ü*/
			m_xmf3Position.y += (targetPos.y - m_xmf3Position.y) * CAM_MOVE_SPEED * fTimeElapsed;
			if (fabs(m_xmf3Position.y - targetPos.y) < 0.2f) m_xmf3Position.y = targetPos.y;
		}

		if (!IsZero(targetPos.z - m_xmf3Position.z)) {
			/*���� �Ÿ� �̻��̸� ������ 0.7�踸ŭ, ���� �Ÿ� ���ϸ� �� ������ ��ü*/
			m_xmf3Position.z += (targetPos.z - m_xmf3Position.z) * CAM_MOVE_SPEED * fTimeElapsed;
			if (fabs(m_xmf3Position.z - targetPos.z) < 0.2f) m_xmf3Position.z = targetPos.z;
		}
		targetPos = m_xmf3Position;

		m_xmf3Position.x += m_xmf3Offset.x;
		m_xmf3Position.y += m_xmf3Offset.y;
		m_xmf3Position.z += m_xmf3Offset.z;

		SetLookAt(targetPos);

		//XMFLOAT4X4 xmf4x4Rotate = Matrix4x4::Identity();
		//XMFLOAT3 xmf3Right = m_pTarget->GetRight();
		//XMFLOAT3 xmf3Up = m_pTarget->GetUp();
		//XMFLOAT3 xmf3Look = m_pTarget->GetLook();
		////		printf("curLook is %f, %f, %f\n", xmf3Look.x, xmf3Look.y, xmf3Look.z);
		//xmf4x4Rotate._11 = xmf3Right.x; xmf4x4Rotate._21 = xmf3Up.x; xmf4x4Rotate._31 = xmf3Look.x;
		//xmf4x4Rotate._12 = xmf3Right.y; xmf4x4Rotate._22 = xmf3Up.y; xmf4x4Rotate._32 = xmf3Look.y;
		//xmf4x4Rotate._13 = xmf3Right.z; xmf4x4Rotate._23 = xmf3Up.z; xmf4x4Rotate._33 = xmf3Look.z;
		//XMFLOAT3 xmf3Offset;
		//XMStoreFloat3(&xmf3Offset, XMVector3TransformCoord(XMLoadFloat3(&m_xmf3Offset), XMLoadFloat4x4(&xmf4x4Rotate)));
		////XMFLOAT3 xmf3Offset = Vector3::TransformCoord(m_xmf3Offset, &XMLoadFloat4x4(&xmf4x4Rotate));
		//XMFLOAT3 xmf3Position = Vector3::Add(m_pTarget->GetPosition(), xmf3Offset);
		//XMFLOAT3 xmf3Direction = Vector3::Subtract(xmf3Position, m_xmf3Position);
		//float fLength = Vector3::Length(xmf3Direction);
		//xmf3Direction = Vector3::Normalize(xmf3Direction);
		//float fTimeLagScale = (m_fTimeLag) ? fTimeElapsed * (1.0f / m_fTimeLag) : 1.0f;
		//float fDistance = fLength * fTimeLagScale;
		//if (fDistance > fLength) fDistance = fLength;
		//if (fLength < 0.01f) fDistance = fLength;
		//if (fDistance > 0)
		//{
		//	m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Direction, fDistance);
		//	SetLookAt(xmf3LookAt);
		//}
	}
}

void CFollowCamera::SetLookAt(XMFLOAT3 & xmf3LookAt)
{
	XMFLOAT3 up = m_pTarget->GetUp();
	//���⼭ �������̶� LookAt�̶� ������ EyeDir�� 0, 0, 0�̶�� ���� ��

	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, up);
	m_xmf3Right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_xmf3Up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_xmf3Look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}

CBoardCamera::CBoardCamera() : CCamera()
{
	SetTimeLag(0.0f);
	SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
	GenerateProjectionMatrix(1.01f, 5000.0f);
	//���� �� �ϸ� ���� �������� �� �� ���� �ʾ�?
	//�ƴ�..
	//���� �ϰ� �⺻����̶� �־���� �� �� ������?
	//����..
	SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
}

CBoardCamera::~CBoardCamera()
{
}

void CBoardCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(60.0f), ASPECT_RATIO, fNearPlaneDistance, fFarPlaneDistance);

	//	m_xmf4x4Projection = Matrix4x4::OrthographicLH(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, fNearPlaneDistance, fFarPlaneDistance);
}