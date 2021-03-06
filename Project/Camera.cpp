#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
	: m_xmf4x4View(Matrix4x4::Identity())
	, m_xmf4x4Projection(Matrix4x4::Identity())
	, m_d3dViewport({ 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f })
	, m_d3dScissorRect({ 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT })
	, m_xmf3Position(XMFLOAT3(0.0f, 0.0f, 0.0f))
	, m_xmf3Right(XMFLOAT3(1.0f, 0.0f, 0.0f))
	, m_xmf3Look(XMFLOAT3(0.0f, 0.0f, 1.0f))
	, m_xmf3Up(XMFLOAT3(0.0f, 1.0f, 0.0f))
	, m_xmf3LookAtWorld(XMFLOAT3(0.0f, 0.0f, 0.0f))
{
	GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
	SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
}

Camera::~Camera()
{
	ReleaseShaderVariables();
}

void Camera::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbCamera = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbCamera->Map(0, NULL, (void **)&m_pcbMappedCamera);
}

void Camera::ReleaseShaderVariables()
{
	if (m_pd3dcbCamera)
	{
		m_pd3dcbCamera->Unmap(0, NULL);
		m_pd3dcbCamera->Release();
	}
}

void Camera::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	RegenerateViewMatrix();

	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	::memcpy(&m_pcbMappedCamera->m_xmf3Position, &m_xmf3Position, sizeof(XMFLOAT3));

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbCamera->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(static_cast<int>(RootParameter::CAMERA), d3dGpuVirtualAddress);
}

void Camera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

void Camera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX	= float(xTopLeft);
	m_d3dViewport.TopLeftY	= float(yTopLeft);
	m_d3dViewport.Width		= float(nWidth);
	m_d3dViewport.Height	= float(nHeight);
	m_d3dViewport.MinDepth	= fMinZ;
	m_d3dViewport.MaxDepth	= fMaxZ;
}

void Camera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left	= xLeft;
	m_d3dScissorRect.top	= yTop;
	m_d3dScissorRect.right	= xRight;
	m_d3dScissorRect.bottom = yBottom;
}

void Camera::GenerateViewMatrix()
{
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, m_xmf3LookAtWorld, m_xmf3Up);
}

void Camera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf3Position		= xmf3Position;
	m_xmf3LookAtWorld	= xmf3LookAt;
	m_xmf3Up			= xmf3Up;

	GenerateViewMatrix();
}

void Camera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}

void Camera::RegenerateViewMatrix()
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

void Camera::SetLookAt(const XMFLOAT3 & xmf3LookAt)
{
	XMFLOAT3 up = XMFLOAT3(0, 1, 0);
	//if(m_pTarget) m_pTarget->GetUp();
	//else up = XMFLOAT3(0, 1, 0);
	//여기서 포지션이랑 LookAt이랑 같으면 EyeDir이 0, 0, 0이라고 에러 뜸

	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, up);
	m_xmf3Right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_xmf3Up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_xmf3Look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}

void Camera::Update(float fTimeElapsed)
{
}
