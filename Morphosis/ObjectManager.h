//#pragma once
//#include "Object.h"
//
//class CObject;
//class CPlayer;
//class CProjectile;
//class CUI;
//
//struct LEVELDATA_DESC {
//	~LEVELDATA_DESC() {
//	}
//
//	std::string levelName = "";
//	int nCollisionMaps = 0;
//	int nTeam1SpawnPoint = 0;
//	int nTeam2SpawnPoint = 0;
//	bool isCapturePointExist = false;
//
//	XMFLOAT3 BoundaryBoxPosition;
//	XMFLOAT3 BoundaryBoxScale;
//	XMFLOAT4 BoundaryBoxRotation;
//
//	std::vector<XMFLOAT3> CollisionPosition;
//	std::vector<XMFLOAT3> CollisionScale;
//	std::vector<XMFLOAT4> CollisionRotation;
//
//	std::vector<XMFLOAT3> Team1SpawnPointPosition;
//	std::vector<XMFLOAT4> Team1SpawnPointRotation;
//
//	std::vector<XMFLOAT3> Team2SpawnPointPosition;
//	std::vector<XMFLOAT4> Team2SpawnPointRotation;
//
//	XMFLOAT3 CapturePointPosition;
//	XMFLOAT3 CapturePointScale;
//	XMFLOAT4 CapturePointRotation;
//};
//
//class CObjectManager {
//public:
//	CObjectManager() = delete;
//	CObjectManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, SceneType sceneType)
//		: m_pd3dDevice(pd3dDevice)
//		, m_pd3dCommandList(pd3dCommandList)
//		, m_SceneType(sceneType)
//	{
//
//		CreateObjectData();
//	}
//	~CObjectManager();
//
//public:
//	void Render();
//	void Update(float fTime);
//
//	CObject* GetTarget(int i);
//	ID3D12DescriptorHeap* GetDescriptorHeap() {
//		return m_pd3dCbvSrvDescriptorHeap;
//	}
//	void ProcessInput(UCHAR* pKeysBuffer, XMFLOAT2 mouse);
//
//	Collider* GetCollider(Collider& myCollider, ColliderTag targetTag, bool isMakeAlign = false);
//	void ColliderTrigInit(ColliderTag targetTag);
//
//
//
//private:
//	void LateUpdate(float fTime);
//
//	void CreateDescriptorHeap();
//	void CreateConstantBufferResorce();
//	void CreateConstantBufferView();
//	void CreateTextureResourceView(CTexture* pTexture);
//	void CreateObjectData();
//
//	bool IsCollidable(CObject* src, CObject* dst) {
//		if (!dst->IsAlive()) return false;
//		if (IsAnotherTeam(src, dst)) return true;
//		return false;
//	}
//	bool IsAnotherTeam(CObject* src, CObject* dst) const { return src->GetTeam() != dst->GetTeam(); }
//	//void CollisionCheck();
//
//private:
//	SceneType						m_SceneType;
//
//	ID3D12Device					*m_pd3dDevice = NULL;
//	ID3D12GraphicsCommandList		*m_pd3dCommandList = NULL;
//	ID3D12DescriptorHeap			*m_pd3dCbvSrvDescriptorHeap = NULL;
//
//	/*********************************************************************
//	2019-06-13
//	이거 오브젝트같은거 만들어주려고 할 때만 쓰이는데 굳이 Scene에 둘 필요 없이
//	오브젝트 매니저에 둬서 관리하게 하자.
//	*********************************************************************/
//	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dCbvCPUDescriptorStartHandle;
//	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorStartHandle;
//	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dSrvCPUDescriptorStartHandle;
//	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGPUDescriptorStartHandle;
//
//	/*********************************************************************
//	2019-06-13
//	m_nObjects는 프롭과 플레이어, 투사체의 개수를 합한 값.
//	m_nAnimationMatrix는 m_nObject * g_nAnimBone를 한 값.
//	*********************************************************************/
//	int		m_nObjects = 0;
//
//	ID3D12Resource*		m_pd3dCBPropResource = NULL;
//	ID3D12Resource*		m_pd3dCBPlayersResource = NULL;
//	ID3D12Resource*		m_pd3dCBProjectilesResource = NULL;
//	ID3D12Resource*		m_pd3dCBFloatingUIsResource = NULL;
//	ID3D12Resource*		m_pd3dCBDefaultUIsResource = NULL;
//
//	CB_OBJECT_INFO*		m_pCBMappedPropObjects = NULL;
//	CB_OBJECT_INFO*		m_pCBMappedPlayers = NULL;
//	CB_OBJECT_INFO*		m_pCBMappedProjectiles = NULL;
//	CB_UI_INFO*			m_pCBMappedFloatingUIs = NULL;
//	CB_UI_INFO*			m_pCBMappedDefaultUIs = NULL;
//
//	/*********************************************************************
//	2019-06-15
//	생성한 객체들을 관리할 벡터.
//	*********************************************************************/
//	vector<CObject*>		m_Props;
//	vector<CPlayer*>		m_Players;
//	vector<CProjectile*>	m_Projectiles;
//	vector<CUI*>			m_FloatingUI;
//	vector<CUI*>			m_DefaultUI;
//
//	/*********************************************************************
//	2019-07-01
//	LevelData 관리를 위한 설명 구조체
//	*********************************************************************/
//	LEVELDATA_DESC m_LevelDataDesc;
//};
//
