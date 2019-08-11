#pragma once
class CFramework;
class CCamera;
class CObjectManager;

struct CB_OBJECT_INFO;

// Scene 자체는 안 쓸 예정이니까 추상클래스로?
// 인터페이스?
class CScene
{
public:
	CScene() {
		cout << "CScene 생성 - " << sizeof(CScene) << "\n";
	}
	~CScene();
protected:
	ID3D12RootSignature				*m_pd3dGraphicsRootSignature	= NULL;
	ID3D12Device					*m_pd3dDevice					= NULL;
	ID3D12GraphicsCommandList		*m_pd3dCommandList				= NULL;
	ID3D12DescriptorHeap			*m_pd3dCbvSrvDescriptorHeap		= NULL;

	POINT							m_ptOldCursorPos;
	CCamera							*m_pCamera = NULL;
	CFramework						*m_pFramework = NULL;

public:
	// Scene의 기본적인 함수들
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList) = 0;
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Update(float fTimeElapsed);
	virtual void ProcessInput(UCHAR * pKeysBuffer) = 0;
	virtual void Release();

	// 초기화 함수
	virtual ID3D12RootSignature *CreateRootSignature(ID3D12Device *pd3dDevice);

protected:
	CObjectManager* m_ObjMng = NULL;
};

class CSceneMainPlay : public CScene {
public:
	CSceneMainPlay() {
		cout << "CSceneMainPlay 생성 - " << sizeof(CSceneMainPlay) << "\n";

	}
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);
	virtual void ProcessInput(UCHAR * pKeysBuffer);
};

class CSceneLobby : public CScene {
public:
	CSceneLobby() {
		cout << "CSceneLobby 생성 - " << sizeof(CSceneLobby) << "\n";
	}
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);
	virtual void ProcessInput(UCHAR * pKeysBuffer);
};