#pragma once

struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4						m_xmf4x4View;
	XMFLOAT4X4						m_xmf4x4Projection;
	XMFLOAT3						m_xmf3Position;
};

class Camera {
public:
	Camera();
	~Camera();

public:
	void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseShaderVariables();
	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	void SetViewportsAndScissorRects(ID3D12GraphicsCommandList *pd3dCommandList);
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);
	void RegenerateViewMatrix();

	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	void SetPosition(float x, float y, float z) { m_xmf3Position.x = x; m_xmf3Position.y = y; m_xmf3Position.z = z;}
	void SetLookAt(const XMFLOAT3 & xmf3LookAt);

	void Update(float fTimeElapsed);

private:
	XMFLOAT3						m_xmf3Position;
	XMFLOAT3						m_xmf3Right;
	XMFLOAT3						m_xmf3Up;
	XMFLOAT3						m_xmf3Look;

	XMFLOAT3						m_xmf3LookAtWorld;

	XMFLOAT4X4						m_xmf4x4View;
	XMFLOAT4X4						m_xmf4x4Projection;

	D3D12_VIEWPORT					m_d3dViewport;
	D3D12_RECT						m_d3dScissorRect;

	ID3D12Resource					*m_pd3dcbCamera = NULL;
	VS_CB_CAMERA_INFO				*m_pcbMappedCamera = NULL;
};