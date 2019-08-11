#include "stdafx.h"
#include "ObjectManager.h"
#include "Projectile.h"
#include "Player.h"
#include "UI.h"
#include "Effect.h"
#include "Collider.h"
#include "AnimationController.h"

#include "Texture.h"
#include "AI.h"
#include "Importer.h"

/*********************************************************************
2019-06-17
CObjectManager
*********************************************************************/
CObjectManager::~CObjectManager()
{
	for (int i = 0; i < m_Props.size(); ++i) delete m_Props[i];
	for (int i = 0; i < m_Players.size(); ++i) delete m_Players[i];
	for (int i = 0; i < m_Projectiles.size(); ++i) delete m_Projectiles[i];
	for (int i = 0; i < m_FloatingUI.size(); ++i) delete m_FloatingUI[i];
	for (int i = 0; i < m_DefaultUI.size(); ++i) delete m_DefaultUI[i];

	m_Props.clear();
	m_Players.clear();
	m_Projectiles.clear();
	m_FloatingUI.clear();
	m_DefaultUI.clear();

	for (int i = 0; i < g_vecModel.size(); ++i) delete g_vecModel[i];
	for (int i = 0; i < g_vecAINode.size(); ++i) delete g_vecAINode[i];
	for (int i = 0; i < g_vecTexture.size(); ++i) delete g_vecTexture[i];
	for (int i = 0; i < g_vecAnimCtrl.size(); ++i) delete g_vecAnimCtrl[i];


	//m_LevelDataDesc

	// 얘네는 밖에서 받아옴.
	m_pd3dDevice = NULL;
	m_pd3dCommandList = NULL;

	// 얘는 안에서 만들었음.
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();

	if (m_pd3dCBPropResource) {
		m_pd3dCBPropResource->Unmap(0, nullptr);
		m_pd3dCBPropResource->Release();

		//int  i = 0;
		//while (m_pd3dCBPropResource) {
		//	m_pd3dCBPropResource->Release();
		//	i++;
		//}
	}
	if (m_pd3dCBPlayersResource)		m_pd3dCBPlayersResource->Release();
	if (m_pd3dCBProjectilesResource)	m_pd3dCBProjectilesResource->Release();
	if (m_pd3dCBFloatingUIsResource)m_pd3dCBFloatingUIsResource->Release();
	if (m_pd3dCBDefaultUIsResource)	m_pd3dCBDefaultUIsResource->Release();
}
void CObjectManager::Render()
{
	/*********************************************************************
	2019-06-16
	렌더 하기 전에 위치 행렬 등을 hlsl에서 쓸 수 있게 map한 주소로 복사해줘야 함.
	*********************************************************************/
	UINT ncbElementBytes;
	CB_OBJECT_INFO	*pbMappedcbObject;
	CB_UI_INFO		*pbMappedcbui;


	/*********************************************************************
	2019-06-17
	각 객체의 Render와 Update 함수에서 IsAlive를 체크하고 있기 때문에 hlsl의
	정보를 업데이트 하는 부분에만 IsAlive를 체크하게 추가하였다.
	*********************************************************************/
	ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	for (int i = 0; i < m_Props.size(); i++) {
		if (!m_Props[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedPropObjects + (i * ncbElementBytes));
		memset(pbMappedcbObject, NULL, ncbElementBytes);
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_Props[i]->m_xmf4x4World)));
		//XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4WorldNoTranspose, XMLoadFloat4x4(&Matrix4x4::InverseTranspose(m_Props[i]->m_xmf4x4World)));
	}
	for (int i = 0; i < m_Players.size(); i++) {
		if (!m_Players[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedPlayers + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_Players[i]->m_xmf4x4World)));
		//XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4WorldNoTranspose, XMLoadFloat4x4(&Matrix4x4::InverseTranspose(m_Players[i]->m_xmf4x4World)));
	}
	for (int i = 0; i < m_Projectiles.size(); i++) {
		if (!m_Projectiles[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedProjectiles + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_Projectiles[i]->m_xmf4x4World)));
		//XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4WorldNoTranspose, XMLoadFloat4x4(&Matrix4x4::InverseTranspose(m_Projectiles[i]->m_xmf4x4World)));
	}
	ncbElementBytes = ((sizeof(CB_UI_INFO) + 255) & ~255);
	for (int i = 0; i < m_FloatingUI.size(); i++) {
		pbMappedcbui = (CB_UI_INFO *)((UINT8 *)m_pCBMappedFloatingUIs + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbui->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_FloatingUI[i]->m_xmf4x4World)));
		pbMappedcbui->m_xmf2Size = m_FloatingUI[i]->GetSize();
	}
	for (int i = 0; i < m_DefaultUI.size(); i++) {
		pbMappedcbui = (CB_UI_INFO *)((UINT8 *)m_pCBMappedDefaultUIs + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbui->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_DefaultUI[i]->m_xmf4x4World)));
		pbMappedcbui->m_xmf2Size = m_DefaultUI[i]->GetSize();
	}

	m_pd3dCommandList->SetPipelineState(g_vecPipelineStateObject[static_cast<int>(PSO::DefaultModel)]);
	for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->Render(m_pd3dCommandList);
	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->Render(m_pd3dCommandList);

	m_pd3dCommandList->SetPipelineState(g_vecPipelineStateObject[static_cast<int>(PSO::AnimatedModel)]);
	for (int i = 0; i < m_Players.size(); ++i)		m_Players[i]->Render(m_pd3dCommandList);

	m_pd3dCommandList->SetPipelineState(g_vecPipelineStateObject[static_cast<int>(PSO::FloatingUI)]);
	for (int i = 0; i < m_FloatingUI.size(); ++i)			m_FloatingUI[i]->Render(m_pd3dCommandList);
	// 여기에 PSO 변경하는거 넣어
	m_pd3dCommandList->SetPipelineState(g_vecPipelineStateObject[static_cast<int>(PSO::DefaultUI)]);
	for (int i = 0; i < m_DefaultUI.size(); ++i)			m_DefaultUI[i]->Render(m_pd3dCommandList);

}
void CObjectManager::Update(float fTime)
{
	//m_Props[1]->SetRotation(XMFLOAT3(0, fTime * 20, 0));
	//static float f = 4.0f;
	//static bool turn = false;
	//if (turn) {
	//	f += fTime;
	//	if (f > 4) turn = false;
	//}
	//else {
	//	f -= fTime;
	//	if (f < 0) turn = true;
	//}
	if (SceneType::MAINPLAY == m_SceneType) {

		int hp = m_Players[1]->GetHP();

		//static_cast<CPlayer*>(m_Players[1])->GetHP() / static_cast<float>(g_DefaultHealthPoint);
		m_FloatingUI[0]->SetScale(XMFLOAT2(hp / static_cast<float>(g_DefaultHealthPoint), 0.02f));
		//m_DefaultUI[0]->SetPosition(XMFLOAT3(0, f*0.1,0));
		XMFLOAT3 pos = m_Players[1]->GetPosition();
		pos.y += 50;
		pos.x -= 25;
		m_FloatingUI[0]->SetPosition(pos);

		//Vector3::Multiply(10, m_Players[0]->GetRight());
		//Vector3::Multiply(10, m_Players[0]->GetLook());
		//pos = Vector3::Add( m_Players[0]->GetPosition(), Vector3::Multiply(10, m_Players[0]->GetRight()));
		//pos = Vector3::Add(pos, Vector3::Multiply(10, m_Players[0]->GetLook()));
		//m_FloatingUI[1]->SetPosition(pos);



	}

	for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->Update(fTime);
	for (int i = 0; i < m_Players.size(); ++i)		m_Players[i]->Update(fTime);
	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->Update(fTime);
	for (int i = 0; i < m_FloatingUI.size(); ++i)	m_FloatingUI[i]->Update(fTime);
	for (int i = 0; i < m_DefaultUI.size(); ++i)	m_DefaultUI[i]->Update(fTime);

	LateUpdate(fTime);
}
CObject * CObjectManager::GetTarget(int i)
{
	if (m_Players.size() > i) return m_Players[i];
	else return NULL;
	//return m_Objects[m_nProps + i];
}
void CObjectManager::ProcessInput(UCHAR * pKeysBuffer, XMFLOAT2 mouse)
{
	if (SceneType::MAINPLAY == m_SceneType) {

		if (m_Players[0]->IsAlive()) m_Players[0]->ProcessInput(pKeysBuffer, mouse);

		//static float cameraOffsetZ = 23;
		//static float cameraOffsetY = 27;
		//if (pKeysBuffer[VK_RBUTTON] & 0xF0)
		//	cout << m_Players[0]->GetPosition().x << ", " << m_Players[0]->GetPosition().y << ", " << m_Players[0]->GetPosition().z << "\n";

		//if (pKeysBuffer[KEY::I] & 0xF0) {
		//	cameraOffsetY += 0.05f; m_Players[0]->SetCameraFocus(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
		//}
		//if (pKeysBuffer[KEY::K] & 0xF0) {
		//	cameraOffsetY -= 0.05f; m_Players[0]->SetCameraFocus(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
		//}
		//if (pKeysBuffer[KEY::J] & 0xF0) {
		//	cameraOffsetZ += 0.05f; m_Players[0]->SetCameraFocus(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
		//}
		//if (pKeysBuffer[KEY::L] & 0xF0) {
		//	cameraOffsetZ -= 0.05f; m_Players[0]->SetCameraFocus(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
		//}

		if (pKeysBuffer[VK_LBUTTON] & 0xF0) {
			/*********************************************************************
			2019-06-17
			마우스를 클릭하면 해줘야 하는 것들.

			먼저, IsAlive == false인 투사체를 찾는다.
			찾았으면 그 객체를 초기화하고(위치 변경, 방향 변경 등등)
			IsAlive = true로 변경한다.

			Render와 Update는 IsAlive가 true 일 때만 되게 해야 한다.
			*********************************************************************/
			if (dynamic_cast<CPlayer*>(m_Players[0])->IsShootable()) {
				dynamic_cast<CPlayer*>(m_Players[0])->Shoot();
				auto iter = find_if(m_Projectiles.begin(), m_Projectiles.end(), [](CObject* p) {return !(p->IsAlive()); });
				if (iter != m_Projectiles.end()) {
					dynamic_cast<CProjectile*>((*iter))->Initialize(m_Players[0], "Model_Bullet001_mesh", new EDefaultDamage());
					(*iter)->SetAlive(true);
				}
			}
		}
		if (pKeysBuffer[VK_RBUTTON] & 0xF0) {
			if (dynamic_cast<CPlayer*>(m_Players[0])->IsSkillUseable()) {
				dynamic_cast<CPlayer*>(m_Players[0])->Skill();
				auto iter = find_if(m_Projectiles.begin(), m_Projectiles.end(), [](CObject* p) {return !(p->IsAlive()); });
				if (iter != m_Projectiles.end()) {
					dynamic_cast<CProjectile*>((*iter))->Initialize(m_Players[0], "Model_Crystal_default", new ESlow());
					(*iter)->SetAlive(true);
				}
			}
		}
	}
}
Collider * CObjectManager::GetCollider(Collider & myCollider, ColliderTag targetTag, bool isMakeAlign)
{
	Collider* temp = NULL;
	switch (targetTag)
	{
	case ColliderTag::PROP:
		for (int i = 0; i < m_Props.size(); ++i) {
			temp = m_Props[i]->GetCollisionCollider(myCollider, isMakeAlign);
			if (temp != NULL)
				//return temp;

				if (!temp->m_trigCollided) {
					//temp->m_trigCollided = true;
					return temp;
				}
		}
		break;
	case ColliderTag::PROJECTILE:
		for (int i = 0; i < m_Projectiles.size(); ++i) {
			temp = m_Projectiles[i]->GetCollisionCollider(myCollider, isMakeAlign);
			if (temp != NULL)
				//return temp;

				if (!temp->m_trigCollided) {
					//temp->m_trigCollided = true;
					return temp;
				}
		}
		break;
	case ColliderTag::PLAYER:
		for (int i = 0; i < m_Players.size(); ++i) {
			temp = m_Players[i]->GetCollisionCollider(myCollider, isMakeAlign);
			if (temp != NULL)
				//return temp;

				if (!temp->m_trigCollided) {
					//temp->m_trigCollided = true;
					return temp;
				}
		}
		break;
	default:break;
	}
	return NULL;
}
void CObjectManager::ColliderTrigInit(ColliderTag targetTag)
{
	switch (targetTag)
	{
	case ColliderTag::PROP:			for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->SetColliderTrigOff();		break;
	case ColliderTag::PROJECTILE:	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->SetColliderTrigOff(); break;
	case ColliderTag::PLAYER:		for (int i = 0; i < m_Players.size(); ++i) 		m_Players[i]->SetColliderTrigOff();		break;
	default:break;
	}
}
void CObjectManager::LateUpdate(float fTime)
{
	for (auto projectile = m_Projectiles.begin(); projectile != m_Projectiles.end(); ++projectile)
		for (auto player = m_Players.begin(); player != m_Players.end(); ++player)
			if ((*player)->IsAlive())
				if (IsCollidable((*player), (*projectile)))
					if ((*player)->IsCollide(*(*projectile))) {
						(*projectile)->AddCollisionEffect((*player));
						(*projectile)->Disable();
					}



	for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->LateUpdate(fTime);
	for (int i = 0; i < m_Players.size(); ++i)		m_Players[i]->LateUpdate(fTime);
	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->LateUpdate(fTime);
	for (int i = 0; i < m_FloatingUI.size(); ++i)			m_FloatingUI[i]->LateUpdate(fTime);
	for (int i = 0; i < m_DefaultUI.size(); ++i)			m_DefaultUI[i]->LateUpdate(fTime);
}
void CObjectManager::CreateDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = m_nObjects + 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	HRESULT result = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);
	HRESULT reason = m_pd3dDevice->GetDeviceRemovedReason();

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize * (m_nObjects));
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize * (m_nObjects));
}
void CObjectManager::CreateConstantBufferResorce()
{
	UINT ncbElementBytes;
	ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	m_pd3dCBPropResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * static_cast<int>( m_Props.size() ),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBPropResource)	m_pd3dCBPropResource->Map(0, NULL, (void **)&m_pCBMappedPropObjects);

	m_pd3dCBPlayersResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * static_cast<int>(m_Players.size()),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBPlayersResource) m_pd3dCBPlayersResource->Map(0, NULL, (void **)&m_pCBMappedPlayers);

	m_pd3dCBProjectilesResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * static_cast<int>(m_Projectiles.size()),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBProjectilesResource) m_pd3dCBProjectilesResource->Map(0, NULL, (void **)&m_pCBMappedProjectiles);


	/*
	내가 지금 하던 생각:
	저 m_nUI 같은 변수들은 최대한 vec.size()로 대체하고 미리 값 받아서 vec로 개수 생성하고 그 뒤에 Create() 함수들을 부르는게 낫겠다.

	일단 하던거나 마저 하고.
	*/


	ncbElementBytes = ((sizeof(CB_UI_INFO) + 255) & ~255);
	m_pd3dCBFloatingUIsResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * static_cast<int>(m_FloatingUI.size()),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBFloatingUIsResource) m_pd3dCBFloatingUIsResource->Map(0, NULL, (void **)&m_pCBMappedFloatingUIs);

	m_pd3dCBDefaultUIsResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * static_cast<int>(m_DefaultUI.size()),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBDefaultUIsResource) m_pd3dCBDefaultUIsResource->Map(0, NULL, (void **)&m_pCBMappedDefaultUIs);
}
void CObjectManager::CreateConstantBufferView()
{
	UINT ncbElementBytes;
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress;
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;

	ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	d3dCBVDesc.SizeInBytes = ncbElementBytes;

	/*********************************************************************
	2019-06-17
	굳이 count로 할 필요가 없을 거 같은데??
	저 가상 주소는 어짜피 바로 직전에 받아오잖아.
	*********************************************************************/
	int count = 0;
	if (nullptr != m_pd3dCBPropResource) {
		d3dGpuVirtualAddress = m_pd3dCBPropResource->GetGPUVirtualAddress();
		for (int i = 0; i < m_Props.size(); i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize * count++);
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}
	if (nullptr != m_pd3dCBPlayersResource) {
		d3dGpuVirtualAddress = m_pd3dCBPlayersResource->GetGPUVirtualAddress();
		for (int i = 0; i < m_Players.size(); i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize * count++);
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}
	if (nullptr != m_pd3dCBProjectilesResource) {
		d3dGpuVirtualAddress = m_pd3dCBProjectilesResource->GetGPUVirtualAddress();
		for (int i = 0; i < m_Projectiles.size(); i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize * count++);
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}

	ncbElementBytes = ((sizeof(CB_UI_INFO) + 255) & ~255);
	d3dCBVDesc.SizeInBytes = ncbElementBytes;

	if (nullptr != m_pd3dCBFloatingUIsResource) {
		d3dGpuVirtualAddress = m_pd3dCBFloatingUIsResource->GetGPUVirtualAddress();
		for (int i = 0; i < m_FloatingUI.size(); i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize * count++);
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}
	if (nullptr != m_pd3dCBDefaultUIsResource) {
		d3dGpuVirtualAddress = m_pd3dCBDefaultUIsResource->GetGPUVirtualAddress();
		for (int i = 0; i < m_DefaultUI.size(); i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize * count++);
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}
}
void CObjectManager::CreateTextureResourceView(CTexture * pTexture)
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dSrvCPUDescriptorHandle = m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorStartHandle;
	int nTextureType = pTexture->GetTextureType();
	ID3D12Resource *pShaderResource = pTexture->GetTexture();
	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
	m_pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, d3dSrvCPUDescriptorHandle);
	m_d3dSrvCPUDescriptorStartHandle.ptr += ::g_nCbvSrvDescriptorIncrementSize;

	pTexture->SetRootArgument(g_RootParameterTexture, d3dSrvGPUDescriptorHandle);
	m_d3dSrvGPUDescriptorStartHandle.ptr += ::g_nCbvSrvDescriptorIncrementSize;
}
void CObjectManager::CreateObjectData()
{
	/*********************************************************************
	2019-06-14
	프롭을 생성해야 한다. 레벨 데이터에서 프롭의 종류와 위치 등을 빼와야 함.
	지금은 없으니까 대충 만들자.

	2019-07-01
	이젠 레벨 데이터가 있다. 충돌맵을 만들자.
	*********************************************************************/
	CImporter importer(m_pd3dDevice, m_pd3dCommandList);








	if (SceneType::MAINPLAY == m_SceneType) {



		importer.ImportLevel("LevelData_TestMap", m_LevelDataDesc);

		XMFLOAT3 nodePositions[9] = {
			XMFLOAT3(0,0,0),
			XMFLOAT3(-200,0,88),
			XMFLOAT3(-200,0,230),
			XMFLOAT3(-139,0,257),
			XMFLOAT3(-127,0,341),
			XMFLOAT3(-42,0,387),
			XMFLOAT3(-18,0,297),
			XMFLOAT3(73,0,270),
			XMFLOAT3(56,0,137)
		};



		for (int i = 0; i < 9; ++i) {
			AINode* tempNode = new AINode();
			tempNode->m_xmf3Position = nodePositions[i];
			g_vecAINode.push_back(tempNode);
		}
		for (int i = 0; i < 8; ++i) {
			g_vecAINode[i]->next = g_vecAINode[i + 1];
		}
		g_vecAINode[8]->next = g_vecAINode[0];

		/*********************************************************************
		2019-06-15
		서술자 힙을 생성하기 위해 개수들을 정해준다. 지금은 임의로 하지만 나중에는
		m_nProps는 LevelData에서 읽어오고, 나머지는 Defines.h에서 가져올 것.
		*********************************************************************/
		//int nProps = 1;
		//int nPlayers = 2;
		//int nFloatingUI = 1;
		//int nDefaultUI = 1;
		int nProps = 1;
		int nPlayers = 2;
		int nFloatingUI = 1;
		int nDefaultUI = 1;
		int nProjectiles = nPlayers * g_nProjectilePerPlayer;
		m_nObjects = nProps + nPlayers + nProjectiles + nFloatingUI + nDefaultUI;

		m_Props.resize(nProps);
		m_Players.resize(nPlayers);
		m_Projectiles.resize(nProjectiles);
		m_FloatingUI.resize(nFloatingUI);
		m_DefaultUI.resize(nDefaultUI);

		CreateDescriptorHeap();
		/*********************************************************************
		2019-06-15
		텍스처도 여기서 넣어야 할 것 같음. 텍스처를 먼저 만들어둔다.
		텍스처는 서술자 힙 만들고 나서 해야 되는거 아냐?

		2019-07-21
		텍스처를 전역 벡터로 관리하기 시작.
		*********************************************************************/

		importer.ImportTexture(L"0615_Box_diff",			"Texture_PaperBox");
		importer.ImportTexture(L"character_2_diff_test3",	"Texture_Character");
		importer.ImportTexture(L"0618_LevelTest_diff",		"Texture_Level");
		importer.ImportTexture(L"HealthBar",				"Texture_HPBar");
		importer.ImportTexture(L"crosshair",				"Texture_Crosshair");
		importer.ImportTexture(L"TEX_crystal",				"Texture_Crystal");
		for (int i = 0; i < g_vecTexture.size(); ++i) CreateTextureResourceView(g_vecTexture[i]);

		importer.ImportMesh("0730_LevelTest_mesh",							"Model_Level",		MeshType::DefaulModel);
		importer.ImportMesh("0725_Character_mesh",							"Model_Character",	MeshType::AnimatedModel);
		importer.ImportMesh("0725_PaperBox_NoSpitPerVertexNormal_mesh",		"Model_PaperBox",	MeshType::DefaulModel);

		CreateUIMesh("UI_TEST", m_pd3dDevice, m_pd3dCommandList);
		CreateUIMesh("UI_TEST2", m_pd3dDevice, m_pd3dCommandList);
		CreateUIMesh("UI_Crosshair", m_pd3dDevice, m_pd3dCommandList);

		CreateModel("Model_Level_Box042", "Texture_Level", "Model_Level_1");
		CreateModel("Model_Level_Box045", "Texture_Level", "Model_Level_2");
		CreateModel("Model_Level_Box047", "Texture_Level", "Model_Level_3");

		CreateModel("Model_Character_body", "Texture_Character",	"Model_Character_1");
		CreateModel("Model_Character_jumper", "Texture_Character",	"Model_Character_2");
		CreateModel("Model_Character_mask", "Texture_Character",	"Model_Character_3");

		CreateModel("Model_PaperBox", "Texture_PaperBox", "Model_PaperBox");

		CreateModel("UI_TEST",		"Texture_HPBar",		"UI_TEST");
		CreateModel("UI_Crosshair", "Texture_Crosshair",	"UI_Crosshair");

		CreateAnimCtrl("AnimCtrl_Character");

		importer.ImportAnimClip("0603_CharacterIdle_anim",		"CharacterIdle", true);
		importer.ImportAnimClip("0603_CharacterRun_anim",		"CharacterRun", true);
		importer.ImportAnimClip("0603_CharacterFire_anim",		"CharacterFire", false);
		importer.ImportAnimClip("0603_CharacterStartJump_anim", "CharacterStartJump", false);
		importer.ImportAnimClip("0603_CharacterEndJump_anim",	"CharacterEndJump", false);
		importer.ImportAnimClip("0603_CharacterDied_anim",		"CharacterDied", false);

		AddAnimClipToCtrl("AnimCtrl_Character", "CharacterIdle");
		AddAnimClipToCtrl("AnimCtrl_Character", "CharacterRun");
		AddAnimClipToCtrl("AnimCtrl_Character", "CharacterFire");
		AddAnimClipToCtrl("AnimCtrl_Character", "CharacterStartJump");
		AddAnimClipToCtrl("AnimCtrl_Character", "CharacterEndJump");
		AddAnimClipToCtrl("AnimCtrl_Character", "CharacterDied");

		CreateConstantBufferResorce();
		CreateConstantBufferView();

		int count = 0;
		for (int i = 0; i < m_Props.size(); i++) {
			CObject* obj = new CObject();
			obj->SetMng(this);
			if (0 == i) {
				obj->AddModel("Model_Level_1");
				obj->AddModel("Model_Level_2");
				obj->AddModel("Model_Level_3");
				for (int j = 0; j < m_LevelDataDesc.nCollisionMaps; ++j) {
					obj->AddCollider(
						m_LevelDataDesc.CollisionPosition[j],
						m_LevelDataDesc.CollisionScale[j],
						m_LevelDataDesc.CollisionRotation[j]);
				}
			}

			obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize) * count++);
			m_Props[i] = obj;
		}
		for (int i = 0; i < m_Players.size(); i++) {
			CPlayer* obj = new CPlayer();
			obj->SetMng(this);
			obj->AddModel("Model_Character_1");
			obj->AddModel("Model_Character_2");
			obj->AddModel("Model_Character_3");

			obj->SetAnimCtrl("AnimCtrl_Character");

			obj->SetPosition(0, 100, i * g_fDefaultUnitScale * 3);
			obj->SetSpawnPoint(obj->GetPosition());
			obj->AddCollider(
				XMFLOAT3(0, 8, 0),
				XMFLOAT3(6, 8, 6),
				XMFLOAT4(0, 0, 0, 1)
			);
			//obj->SetCameraFocus(XMFLOAT3(0, 40, 27));
			obj->SetCameraFocus(XMFLOAT3(0, 50, -60));
			//0, 50, -60

			obj->SetTeam((i % 2) + 1);
			obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize) * count++);

			if (i >= 1) obj->m_AIBrain->PowerOn();

			m_Players[i] = obj;
		}
		for (int i = 0; i < m_Projectiles.size(); i++) {
			CProjectile* obj = new CProjectile();
			obj->SetMng(this);
			obj->AddModel("Model_PaperBox");

			obj->SetAlive(false);
			obj->AddCollider(XMFLOAT3(0, 0, 0), 10.0f);
			obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize) * count++);
			m_Projectiles[i] = obj;
		}
		for (int i = 0; i < m_FloatingUI.size(); i++) {
			CUI* obj = new CUI();
			obj->SetMng(this);

			if (0 == i) {
				obj->AddModel("UI_TEST");
				obj->SetPosition(0.0f, 50.0f, 0.0f);
				obj->Initialize(XMFLOAT2(50, 50));
			}


			obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize) * count++);
			m_FloatingUI[i] = obj;
		}
		for (int i = 0; i < m_DefaultUI.size(); i++) {
			CUI* obj = new CUI();
			obj->SetMng(this);
			obj->AddModel("UI_Crosshair");
			obj->SetPosition(0.5f, 0.35f, 0.0f);
			obj->Initialize(XMFLOAT2(50, 50));

			obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize) * count++);
			m_DefaultUI[i] = obj;
		}

	}
	//else if (SceneType::LOBBY == m_SceneType)
	//{
	//	importer.ImportLevel("LevelData_TestMap", m_LevelDataDesc);

	//	XMFLOAT3 nodePositions[9] = {
	//		XMFLOAT3(0,0,0),
	//		XMFLOAT3(-200,0,88),
	//		XMFLOAT3(-200,0,230),
	//		XMFLOAT3(-139,0,257),
	//		XMFLOAT3(-127,0,341),
	//		XMFLOAT3(-42,0,387),
	//		XMFLOAT3(-18,0,297),
	//		XMFLOAT3(73,0,270),
	//		XMFLOAT3(56,0,137)
	//	};

	//	for (int i = 0; i < 9; ++i) {
	//		AINode* tempNode = new AINode();
	//		tempNode->m_xmf3Position = nodePositions[i];
	//		g_vecAINode.push_back(tempNode);
	//	}
	//	for (int i = 0; i < 8; ++i) {
	//		g_vecAINode[i]->next = g_vecAINode[i + 1];
	//	}
	//	g_vecAINode[8]->next = g_vecAINode[0];


	//	/*********************************************************************
	//	2019-06-15
	//	서술자 힙을 생성하기 위해 개수들을 정해준다. 지금은 임의로 하지만 나중에는
	//	m_nProps는 LevelData에서 읽어오고, 나머지는 Defines.h에서 가져올 것.
	//	*********************************************************************/
	//	int nProps = 0;
	//	int nPlayers = 0;
	//	int nFloatingUI = 0;
	//	int nDefaultUI = 1;
	//	int nProjectiles = nPlayers * g_nProjectilePerPlayer;
	//	m_nObjects = nProps + nPlayers + nProjectiles + nFloatingUI + nDefaultUI;

	//	m_Props.resize(nProps);
	//	m_Players.resize(nPlayers);
	//	m_Projectiles.resize(nProjectiles);
	//	m_FloatingUI.resize(nFloatingUI);
	//	m_DefaultUI.resize(nDefaultUI);


	//	CreateDescriptorHeap();
	//	/*********************************************************************
	//	2019-06-15
	//	텍스처도 여기서 넣어야 할 것 같음. 텍스처를 먼저 만들어둔다.
	//	텍스처는 서술자 힙 만들고 나서 해야 되는거 아냐?

	//	2019-07-21
	//	텍스처를 전역 벡터로 관리하기 시작.
	//	*********************************************************************/

	//	g_vecTexture.clear();
	//	importer.ImportTexture(L"TitleImg_Test", "Texture_TitleImg");
	//	for (int i = 0; i < g_vecTexture.size(); ++i) CreateTextureResourceView(g_vecTexture[i]);

	//	importer.ImportModel("", "Texture_TitleImg", ModelType::FloatingUI, "UI_TitleImg");

	//	CreateConstantBufferResorce();
	//	CreateConstantBufferView();

	//	int count = 0;
	//	for (int i = 0; i < m_Props.size(); i++) {

	//	}
	//	for (int i = 0; i < m_Players.size(); i++) {

	//	}
	//	for (int i = 0; i < m_Projectiles.size(); i++) {

	//	}
	//	for (int i = 0; i < m_FloatingUI.size(); i++) {

	//	}
	//	for (int i = 0; i < m_DefaultUI.size(); i++) {
	//		CUI* obj = new CUI();
	//		obj->SetMng(this);
	//		obj->AddModel(GetModelByName("UI_TitleImg"));
	//		obj->SetPosition(0.5, 0.5, 0.0f);
	//		obj->Initialize(XMFLOAT2(1920, 1080));

	//		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::g_nCbvSrvDescriptorIncrementSize) * count++);
	//		m_DefaultUI[i] = obj;
	//	}
	//}

	for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->CreateConstantBufferResource(m_pd3dDevice, m_pd3dCommandList);
	for (int i = 0; i < m_Players.size(); ++i)		m_Players[i]->CreateConstantBufferResource(m_pd3dDevice, m_pd3dCommandList);
	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->CreateConstantBufferResource(m_pd3dDevice, m_pd3dCommandList);
	for (int i = 0; i < m_FloatingUI.size(); ++i)	m_FloatingUI[i]->CreateConstantBufferResource(m_pd3dDevice, m_pd3dCommandList);
	for (int i = 0; i < m_DefaultUI.size(); ++i)	m_DefaultUI[i]->CreateConstantBufferResource(m_pd3dDevice, m_pd3dCommandList);

}