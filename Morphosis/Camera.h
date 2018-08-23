#pragma once
#pragma once

#define ASPECT_RATIO				(float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

class CObject;

struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4						m_xmf4x4View;
	XMFLOAT4X4						m_xmf4x4Projection;
	XMFLOAT3						m_xmf3Position;
};

class CCamera
{
protected:
	XMFLOAT3						m_xmf3Position;
	XMFLOAT3						m_xmf3Right;
	XMFLOAT3						m_xmf3Up;
	XMFLOAT3						m_xmf3Look;

	XMFLOAT3						m_xmf3LookAtWorld;
	XMFLOAT3						m_xmf3Offset;
	float           				m_fTimeLag;

	XMFLOAT4X4						m_xmf4x4View;
	XMFLOAT4X4						m_xmf4x4Projection;

	D3D12_VIEWPORT					m_d3dViewport;
	D3D12_RECT						m_d3dScissorRect;

	ID3D12Resource					*m_pd3dcbCamera = NULL;
	VS_CB_CAMERA_INFO				*m_pcbMappedCamera = NULL;

public:
	CCamera();
	virtual ~CCamera();

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList *pd3dCommandList);
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);
	void RegenerateViewMatrix();

	void SetPosition(XMFLOAT3 xmf3Position)				{ m_xmf3Position = xmf3Position; }
	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld)	{ m_xmf3LookAtWorld = xmf3LookAtWorld; }

	XMFLOAT3& GetPosition()								{ return(m_xmf3Position); }
	XMFLOAT3& GetLookAtPosition()						{ return(m_xmf3LookAtWorld); }

	XMFLOAT3& GetRight()	{ return(m_xmf3Right); }
	XMFLOAT3& GetUp()		{ return(m_xmf3Up); }
	XMFLOAT3& GetLook()		{ return(m_xmf3Look); }

	//	void SetOffset(XMFLOAT3 xmf3Offset) { m_xmf3Offset = xmf3Offset; }
	void SetOffset(XMFLOAT3 xmf3Offset) { m_xmf3Offset = xmf3Offset; m_xmf3Position.x += xmf3Offset.x; m_xmf3Position.y += xmf3Offset.y; m_xmf3Position.z += xmf3Offset.z; }
	XMFLOAT3& GetOffset() { return(m_xmf3Offset); }

	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	float GetTimeLag() { return(m_fTimeLag); }

	XMFLOAT4X4 GetViewMatrix() { return(m_xmf4x4View); }
	XMFLOAT4X4 GetProjectionMatrix() { return(m_xmf4x4Projection); }
	D3D12_VIEWPORT GetViewport() { return(m_d3dViewport); }
	D3D12_RECT GetScissorRect() { return(m_d3dScissorRect); }

	virtual void Move(const XMFLOAT3& xmf3Shift) { m_xmf3Position.x += xmf3Shift.x; m_xmf3Position.y += xmf3Shift.y; m_xmf3Position.z += xmf3Shift.z; }
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) { }
	virtual void Update(float fTimeElapsed) { }
	virtual void SetLookAt(XMFLOAT3& xmf3LookAt) { }

	virtual void SetTarget(void *target) {}
	virtual CObject* GetTarget() { return nullptr; }
};

class CBoardCamera : public CCamera
{
private:

public:
	CBoardCamera();
	~CBoardCamera();

public:
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance);
};

class CFollowCamera : public CCamera
{
private:
	CObject * m_pTarget = NULL;

public:
	CFollowCamera();
	~CFollowCamera();

public:
	virtual void SetTarget(void *target);
	virtual CObject* GetTarget();
	virtual void Update(float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& xmf3LookAt);
};