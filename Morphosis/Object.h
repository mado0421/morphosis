#pragma once
//#include "Mesh.h"
//#include "Model.h"
//#include "Camera.h"

//class CObjectManager;
//class CAnimCtrl;
//class Effect;
//class Collider;

struct CB_OBJECT_INFO {
	XMFLOAT4X4	m_xmf4x4World;
	//XMFLOAT4X4	m_xmf4x4WorldNoTranspose;
	//UINT		m_nMaterialIndex;
};
struct CB_UI_INFO {
	XMFLOAT4X4	m_xmf4x4World;
	XMFLOAT2	m_xmf2Size;
};

class CObject {
public:
	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle);
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr);
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle();

	virtual void CreateConstantBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void CreateConstantBufferView(ID3D12Device *pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, int offset);
	virtual void UpdateConstantBuffer(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);
	virtual void LateUpdate(float fTimeElapsed);
	virtual void ProcessInput(UCHAR* pKeysBuffer, XMFLOAT2 mouse);

	virtual void Release();

public:
	const XMFLOAT3	GetPosition();
	const XMFLOAT3	GetLook();
	const XMFLOAT3	GetUp();
	const XMFLOAT3	GetRight();

public:
	XMFLOAT4X4						m_xmf4x4World;

protected:
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;
	CB_OBJECT_INFO					*m_pcbMappedObject	= NULL;
	ID3D12Resource					*m_pd3dcbObject		= NULL;
};

class CUIMesh;
class CUIObject : public CObject {
public:
	CUIObject() :m_xmf2Size(XMFLOAT2(50, 50)) {}

	void CreateUIMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);

	virtual void CreateConstantBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void CreateConstantBufferView(ID3D12Device *pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, int offset);
	virtual void UpdateConstantBuffer(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Release();


protected:
	vector<CUIMesh*>	m_vecMesh;
	XMFLOAT2			m_xmf2Size;
};







//class CObject
//{
//public:
//	// 생성자와 소멸자
//	CObject();
//	~CObject();
//	// GPU 연결하는 함수
//	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle)	{ m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
//	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr)					{ m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
//	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle()									{ return(m_d3dCbvGPUDescriptorHandle); }
//	virtual void SetAnimatedMatrix(float time);
//	virtual void CreateConstantBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
//	virtual void UpdateConstantBuffer(ID3D12GraphicsCommandList *pd3dCommandList);
//
//	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);
//	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
//	virtual void Update(float fTimeElapsed) {}
//	virtual void LateUpdate(float fTimeElapsed) {}
//	virtual void ProcessInput(UCHAR* pKeysBuffer, XMFLOAT2 mouse) {}
//
//	void AddCollisionEffect(CObject* p);
//
//	void SetMng(CObjectManager* mng);
//	void AddCollider(XMFLOAT3 offset, XMFLOAT3 extents, XMFLOAT4 quaternion);
//	void AddCollider(XMFLOAT3 offset, float radius);
//	void SetColliderTrigOff();
//	void SetPosition(float x, float y, float z);
//	void SetCameraFocus(XMFLOAT3 pos);
//	void SetPosition(const XMFLOAT3 xmf3Position);
//	void SetRotation(const XMFLOAT3& angle);
//	void AddModel(const char* name);
//	void SetAnimCtrl(const char* name);
//	void SetTeam(int team)								{ m_Team = team; }
//	void SetAlive(bool state)							{ m_IsAlive = state; }
//	virtual void Enable()								{ SetAlive(true); }
//	virtual void Disable();
//	// 외부 접근 함수
//	Collider* const GetCollisionCollider(Collider& other, bool isMakeAlign);
//	const XMFLOAT3	GetPosition();
//	const XMFLOAT3	GetLook();
//	const XMFLOAT3	GetUp();
//	const XMFLOAT3	GetRight();
//	const XMFLOAT3	GetCameraFocus();
//	const XMFLOAT4	GetQuaternion();
//	const int		GetTeam() const { return m_Team; }
//	// 조건 함수
//	const bool IsCollide(const CObject& other);
//	const bool IsCollide(const Collider& other);
//	const bool IsAlive() const { return m_IsAlive; }
//protected:
//	// 내부 설정 함수
//	void SetLook(XMFLOAT3 look);
//	void SetUp(XMFLOAT3 up);
//	void SetRight(XMFLOAT3 right);
//	virtual void ChangeAnimClip() {}
//
//public:
//	// 월드 변환 행렬
//	XMFLOAT4X4						m_xmf4x4World;
//
//protected:
//	// 다이렉트X 내용
//	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;
//	CB_OBJECT_INFO					*m_pcbMappedObject		= NULL;
//	ID3D12Resource					*m_pd3dcbObject			= NULL;
//	//렌더링
//	XMFLOAT3						m_xmf3CameraFocus;
//	vector<int>						m_vecModelIdx;
//	//이동
//	XMFLOAT3						m_xmf3CollisionOffset;
//	float							m_fHeightVelocity;
//	bool							m_IsGround;
//	//애니메이션
//	int								m_AnimCtrlIdx			= -1;
//	float							m_AnimationTime			= 0;
//	int								m_AnimationState;
//	ID3D12Resource					*m_pd3dcbAnimation		= NULL;
//	XMMATRIX						*m_pcbxmAnimation		= NULL;
//	XMMATRIX						m_a[g_nAnimBone];
//
//	//충돌체
//	std::vector<Collider>			m_Collider;
//	//오브젝트 매니저를 아래서 접근하기 위한 포인터
//	CObjectManager					*m_pObjMng = NULL;
//	// 객체 정보
//	bool							m_IsAlive;
//	int								m_Team;
//
//	std::vector<Effect*>			m_vecEffects;
//	
//};
//
