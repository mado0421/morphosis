#include "stdafx.h"
#include "Object.h"
#include "Animation/AnimationController.h"
#include "Mesh/Mesh.h"
#include "Importer/Importer.h"

/*********************************************************************
2019-06-17
CObject
*********************************************************************/
CObject::CObject()
{
	SetPosition(XMFLOAT3(0, 0, 0));
	m_xmf4x4World			= Matrix4x4::Identity();
	m_IsAlive				= true;
	m_xmf3CollisionOffset	= XMFLOAT3(0, 0, 0);
	m_xmf3CameraTargetOffset= XMFLOAT3(0, 0, 0);
	m_AnimationState		= static_cast<int>(0);
	m_Team					= TEAM_DEFAULT;
	m_Tag					= Tag::Prop;
}
CObject::~CObject()
{
}
void CObject::SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(g_RootParameterObject, m_d3dCbvGPUDescriptorHandle);
}
void CObject::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, bool isDebug)
{
	if (!m_IsAlive) return;
	if (!m_ModelList.empty()) {
		for (auto p = m_ModelList.begin(); p != m_ModelList.end(); ++p) {
			if (m_AnimationController) p->SetAnimatedMatrix(m_AnimationController);
			pd3dCommandList->SetGraphicsRootDescriptorTable(g_RootParameterObject, m_d3dCbvGPUDescriptorHandle);
			p->Render(pd3dCommandList, isDebug);
		}
	}
}
void CObject::Update(float fTimeElapsed)
{
	if (!m_IsAlive) return;

	/*********************************************************************
	2019-06-17
	�̵� ��Ʈ
	*********************************************************************/


	/*********************************************************************
	2019-06-17
	�ִϸ��̼� ��Ʈ
	*********************************************************************/
	if (m_AnimationController) m_AnimationController->Update(fTimeElapsed);
}
void CObject::LateUpdate(float fTimeElapsed)
{
	if (!m_IsAlive) return;
}
void CObject::AddAnimClip(AnimationClip * animClip)
{
	/*********************************************************************
	2019-06-16
	�ִϸ��̼� ��Ʈ�ѷ��� ������ �����ϰ� �߰����ش�.
	*********************************************************************/
	if (NULL == m_AnimationController)
		m_AnimationController = new CAnimationController();
	m_AnimationController->AddAnimData(animClip);
}
void CObject::AddCollider(BoundingOrientedBox box)
{
	m_CollisionBox.push_back(box);
}
void CObject::AddCollider(BoundingSphere sphere)
{
	m_CollisionSphere.push_back(sphere);
}
void CObject::ChangeAnimClip(const char * animClipName)
{
	m_AnimationController->ChangeAnimClip(animClipName);
}
void CObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}
void CObject::SetPosition(const XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}
XMFLOAT3 const CObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));

}
XMFLOAT3 const CObject::GetLook()
{
	XMFLOAT3 vector = { m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33 };
	return Vector3::Normalize(vector);
}
XMFLOAT3 const CObject::GetUp()
{
	XMFLOAT3 vector = { m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23 };
	return Vector3::Normalize(vector);
}
XMFLOAT3 const CObject::GetRight()
{
	XMFLOAT3 vector = { m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13 };
	return Vector3::Normalize(vector);
}
XMMATRIX const CObject::GetAnimationMatrix(int boneIdx)
{
	if (m_AnimationController) return m_AnimationController->GetFinalMatrix(boneIdx);
	else return XMMatrixIdentity();
}
int const CObject::GetNumAnimationBone()
{
	if (m_AnimationController) {
		if (!m_AnimationController->m_AnimData.empty())
			return m_AnimationController->m_AnimData.front()->m_nBoneList;
	}
	return 0;
}
void CObject::SetLook(XMFLOAT3 look)
{
	m_xmf4x4World._31 = look.x;
	m_xmf4x4World._32 = look.y;
	m_xmf4x4World._33 = look.z;
}
void CObject::SetUp(XMFLOAT3 up)
{
	m_xmf4x4World._21 = up.x;
	m_xmf4x4World._22 = up.y;
	m_xmf4x4World._23 = up.z;
}
void CObject::SetRight(XMFLOAT3 right)
{
	m_xmf4x4World._11 = right.x;
	m_xmf4x4World._12 = right.y;
	m_xmf4x4World._13 = right.z;
}
void CObject::SetCameraTargetOffset(XMFLOAT3 pos)
{
	m_xmf3CameraTargetOffset = pos;
}
XMFLOAT3 const CObject::GetCameraTargetPos()
{
	return Vector3::Add(GetPosition(), m_xmf3CameraTargetOffset);
	//return m_xmf3CameraTargetOffset;
}
const bool CObject::IsCollide(CObject* other)
{
	/*********************************************************************
	2019-06-18
	other�� �浹ü��� �� �浹ü���� ���� ���ؾ� ��.
	*********************************************************************/
	for (auto otb = other->m_CollisionBox.cbegin(); otb != other->m_CollisionBox.cend(); ++otb) {
		for (auto myb = m_CollisionBox.cbegin(); myb != m_CollisionBox.cend(); ++myb)		if (otb->Intersects(*myb)) return true;
		for (auto mys = m_CollisionSphere.cbegin(); mys != m_CollisionSphere.cend(); ++mys) if (otb->Intersects(*mys)) return true;
	}
	for (auto ots = other->m_CollisionSphere.cbegin(); ots != other->m_CollisionSphere.cend(); ++ots) {
		for (auto myb = m_CollisionBox.cbegin(); myb != m_CollisionBox.cend(); ++myb)		if (ots->Intersects(*myb)) return true;
		for (auto mys = m_CollisionSphere.cbegin(); mys != m_CollisionSphere.cend(); ++mys) if (ots->Intersects(*mys)) return true;
	}

	return false;
}
void CObject::ProcessInput(UCHAR * pKeysBuffer)
{
}

void CObject::AddCollideInfo(CObject * obj)
{
	m_CollideInfo.push(obj);
}

/*********************************************************************
2019-06-17
CPlayer
*********************************************************************/
CPlayer::CPlayer() : CObject()
{
	m_fSpeed					= g_PlayerDefaultSpeed;
	m_fRemainingTimeOfFire		= 0.0f;
	m_fRPM						= (1 / static_cast<float>(g_DefaultRPM)) * 60.0f;
	m_Tag						= Tag::Player;
	m_xmf3Move					= XMFLOAT3(0, 0, 0);
	m_xmf3SpawnPoint			= GetPosition();
	m_HealthPoint				= g_DefaultHealthPoint;
	m_fRemainingTimeOfRespawn	= 0.0f;
}
void CPlayer::Update(float fTimeElapsed)
{
	if (!m_IsAlive) {
		m_fRemainingTimeOfRespawn -= fTimeElapsed;
		if (m_fRemainingTimeOfRespawn <= 0.0f) {
			CPlayer::Enable();
		}
		return;
	}
	

	/*********************************************************************
	2019-06-18
	���� Ÿ�̸� ��Ʈ
	*********************************************************************/
	m_fRemainingTimeOfFire -= fTimeElapsed;

	/*********************************************************************
	2019-06-17
	�̵� ��Ʈ
	*********************************************************************/
	XMFLOAT3 xmf3Right = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	XMFLOAT3 xmf3Up = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	XMFLOAT3 xmf3Look = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	m_xmf3Move = Move(fTimeElapsed);

	float rotationAngle = Rotate(fTimeElapsed);
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(rotationAngle));
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	xmf3Right = Vector3::TransformNormal(xmf3Right, xmmtxRotate);

	xmf3Look = Vector3::Normalize(xmf3Look);
	xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;		m_xmf4x4World._33 = xmf3Look.z;
	m_xmf4x4World._41 += m_xmf3Move.x;
	m_xmf4x4World._42 += m_xmf3Move.y;
	m_xmf4x4World._43 += m_xmf3Move.z;

	
	XMStoreFloat3(&m_xmf3CameraTargetOffset, XMVector3Rotate(XMLoadFloat3(&m_xmf3CameraTargetOffset), XMQuaternionRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(rotationAngle))));

	/*********************************************************************
	2019-06-18
	�浹ü ��Ʈ
	*********************************************************************/
	for (auto myb = m_CollisionBox.begin(); myb != m_CollisionBox.end(); ++myb)
		myb->Center = Vector3::Add(myb->Center, m_xmf3Move);
	for (auto mys = m_CollisionSphere.begin(); mys != m_CollisionSphere.end(); ++mys)
		mys->Center = Vector3::Add(mys->Center, m_xmf3Move);

	/*********************************************************************
	2019-06-17
	�ִϸ��̼� ��Ʈ
	*********************************************************************/
	if (m_AnimationController) m_AnimationController->Update(fTimeElapsed);

	if (static_cast<int>(AnimationState::IDLE) == m_AnimationState) {
		if (IsMoving()) {
			m_AnimationState = static_cast<int>(AnimationState::RUN);
			m_AnimationController->ChangeAnimClip("PlayerRun");
		}
	}
	else {
		if (!IsMoving()) {
			m_AnimationState = static_cast<int>(AnimationState::IDLE);
			m_AnimationController->ChangeAnimClip("PlayerIdle");
		}
	}

	TriggerOff();
}
void CPlayer::LateUpdate(float fTimeElapsed)
{
	if (!m_IsAlive) return;

	bool alreadyRestore = false;	// �̵��� ��ȿ ó�� �ߴ��� ����
	while (!m_CollideInfo.empty()) {
		if (!alreadyRestore && Tag::Prop == m_CollideInfo.front()->m_Tag) {
			alreadyRestore = true;
			m_xmf4x4World._41 -= m_xmf3Move.x;
			m_xmf4x4World._42 -= m_xmf3Move.y;
			m_xmf4x4World._43 -= m_xmf3Move.z;
			for (auto myb = m_CollisionBox.begin(); myb != m_CollisionBox.end(); ++myb)
				myb->Center = Vector3::Subtract(myb->Center, m_xmf3Move);
			for (auto mys = m_CollisionSphere.begin(); mys != m_CollisionSphere.end(); ++mys)
				mys->Center = Vector3::Subtract(mys->Center, m_xmf3Move);
		}
		if (!alreadyRestore && Tag::Player == m_CollideInfo.front()->m_Tag) {
			alreadyRestore = true;
			m_xmf4x4World._41 -= m_xmf3Move.x;
			m_xmf4x4World._42 -= m_xmf3Move.y;
			m_xmf4x4World._43 -= m_xmf3Move.z;
			for (auto myb = m_CollisionBox.begin(); myb != m_CollisionBox.end(); ++myb)
				myb->Center = Vector3::Subtract(myb->Center, m_xmf3Move);
			for (auto mys = m_CollisionSphere.begin(); mys != m_CollisionSphere.end(); ++mys)
				mys->Center = Vector3::Subtract(mys->Center, m_xmf3Move);
		}
		if (Tag::Projectile == m_CollideInfo.front()->m_Tag) {
			dynamic_cast<CProjectile*>(m_CollideInfo.front())->Damage(this);
		}

		m_CollideInfo.pop();
	}

	if (m_HealthPoint <= 0) CPlayer::Disable();

}
void CPlayer::ProcessInput(UCHAR * pKeysBuffer)
{
	if (pKeysBuffer[KEY::W] & 0xF0) m_trigInput[static_cast<int>(Move::W)] = true;
	if (pKeysBuffer[KEY::A] & 0xF0) m_trigInput[static_cast<int>(Move::A)] = true;
	if (pKeysBuffer[KEY::S] & 0xF0) m_trigInput[static_cast<int>(Move::S)] = true;
	if (pKeysBuffer[KEY::D] & 0xF0) m_trigInput[static_cast<int>(Move::D)] = true;
	if (pKeysBuffer[KEY::Q] & 0xF0) m_trigInput[static_cast<int>(Move::Q)] = true;
	if (pKeysBuffer[KEY::E] & 0xF0) m_trigInput[static_cast<int>(Move::E)] = true;
}
void CPlayer::Enable()
{
	m_HealthPoint = g_DefaultHealthPoint;
	SetPosition(m_xmf3SpawnPoint);
	for (auto myb = m_CollisionBox.begin(); myb != m_CollisionBox.end(); ++myb)
		myb->Center = GetPosition();
	for (auto mys = m_CollisionSphere.begin(); mys != m_CollisionSphere.end(); ++mys)
		mys->Center = GetPosition();
	CObject::Enable();
}
void CPlayer::Disable()
{
	m_fRemainingTimeOfRespawn = g_RespawnDelay;
	CObject::Disable();
}
void CPlayer::Shoot()
{
	m_fRemainingTimeOfFire = m_fRPM;
}
bool CPlayer::IsShootable()
{
	return m_fRemainingTimeOfFire <= 0;
}
void CPlayer::TriggerOff()
{
	for (int i = 0; i < static_cast<int>(Move::count); i++) m_trigInput[i] = false;
}
XMFLOAT3 CPlayer::Move(float fTimeElapsed)
{
	XMFLOAT3 temp(0, 0, 0);
	if (m_trigInput[static_cast<int>(Move::W)]) { temp = Vector3::Add(temp, Vector3::Normalize(GetLook())); }
	if (m_trigInput[static_cast<int>(Move::A)]) { temp = Vector3::Add(temp, Vector3::Normalize(Vector3::Multiply(-1, GetRight()))); }
	if (m_trigInput[static_cast<int>(Move::S)]) { temp = Vector3::Add(temp, Vector3::Normalize(Vector3::Multiply(-1, GetLook()))); }
	if (m_trigInput[static_cast<int>(Move::D)]) { temp = Vector3::Add(temp, Vector3::Normalize(GetRight())); }
	temp = Vector3::Normalize(temp);
	return Vector3::Multiply(fTimeElapsed * m_fSpeed, temp);
}
float CPlayer::Rotate(float fTimeElapsed)
{
	float temp = 0;
	if (m_trigInput[static_cast<int>(Move::Q)]) temp += fTimeElapsed * m_fSpeed * -1;
	if (m_trigInput[static_cast<int>(Move::E)]) temp += fTimeElapsed * m_fSpeed;

	return temp;
}

/*********************************************************************
2019-06-17
CProjectile
*********************************************************************/
CProjectile::CProjectile() : CObject()
{
	m_IsAlive		= false;
	m_xmf3Direction = XMFLOAT3(0, 0, 0);
	m_fSpeed		= g_ProjectileDefaultSpeed;
	m_fLifeTime		= g_DefaultProjectileLifeTime;
	m_Tag			= Tag::Projectile;
	m_BaseDamage	= g_DefaultDamage;
}

void CProjectile::Initialize(CObject * obj)
{
	/*********************************************************************
	2019-06-17
	���⼭ ����� �ϴ� ����
	���ڷ� �޾ƿ� ������Ʈ�κ��� ��ġ�� ����, ���� �޾ƾ� �Ѵٴ� ��.

	�� ����ü�� ȸ���ϸ鼭 �����;� �ϸ� ��. �׷��� ��. ������ ��.

	�浹ü ��ġ�� �ٲ���� ��.
	*********************************************************************/
	SetPosition(obj->GetPosition());
	for (auto myb = m_CollisionBox.begin(); myb != m_CollisionBox.end(); ++myb)
		myb->Center = obj->GetPosition();
	for (auto mys = m_CollisionSphere.begin(); mys != m_CollisionSphere.end(); ++mys)
		mys->Center = obj->GetPosition();

	/*********************************************************************
	2019-06-17
	�������� �޽��� ����
	*********************************************************************/
	SetLook(obj->GetLook());
	SetUp(obj->GetUp());
	SetRight(obj->GetRight());

	SetTeam(obj->GetTeam());

	m_xmf3Direction		= Vector3::Normalize(obj->GetLook());
	m_fLifeTime			= g_DefaultProjectileLifeTime;
}

void CProjectile::Update(float fTimeElapsed)
{
	if (!m_IsAlive) return;
	/*********************************************************************
	2019-06-18
	LifeTime ����
	*********************************************************************/
	m_fLifeTime -= fTimeElapsed;
	if (IsExpired()) { m_IsAlive = false; return; }

	XMFLOAT3 xmf3Move = m_xmf3Direction;
	xmf3Move = Vector3::Multiply(m_fSpeed, xmf3Move);
	xmf3Move = Vector3::Multiply(fTimeElapsed, xmf3Move);

	m_xmf4x4World._41 += xmf3Move.x;
	m_xmf4x4World._42 += xmf3Move.y;
	m_xmf4x4World._43 += xmf3Move.z;

	if (m_AnimationController) m_AnimationController->Update(fTimeElapsed);

	/*********************************************************************
	2019-06-18
	�浹ü ��Ʈ
	*********************************************************************/
	for (auto myb = m_CollisionBox.begin(); myb != m_CollisionBox.end(); ++myb)
		myb->Center = Vector3::Add(myb->Center, xmf3Move);
	for (auto mys = m_CollisionSphere.begin(); mys != m_CollisionSphere.end(); ++mys)
		mys->Center = Vector3::Add(mys->Center, xmf3Move);
}

void CProjectile::LateUpdate(float fTimeElapsed)
{
	if (!m_IsAlive) return;

}

void CProjectile::Damage(CObject* obj)
{
	dynamic_cast<CPlayer*>(obj)->TakeDamage(static_cast<int>(m_BaseDamage));
}

/*********************************************************************
2019-06-17
CObjectManager
*********************************************************************/
CObjectManager::~CObjectManager()
{
	for (int i = 0; i < m_Props.size(); ++i) delete m_Props[i];
	m_Props.erase(m_Props.begin(), m_Props.end());

	for (int i = 0; i < m_Projectiles.size(); ++i) delete m_Projectiles[i];
	m_Projectiles.erase(m_Projectiles.begin(), m_Projectiles.end());

	for (int i = 0; i < m_Players.size(); ++i) delete m_Players[i];
	m_Players.erase(m_Players.begin(), m_Players.end());

	for (int i = 0; i < m_TextureList.size(); ++i) delete m_TextureList[i];
	m_TextureList.erase(m_TextureList.begin(), m_TextureList.end());
}
void CObjectManager::Render()
{
	/*********************************************************************
	2019-06-16
	���� �ϱ� ���� ��ġ ��� ���� hlsl���� �� �� �ְ� map�� �ּҷ� ��������� ��.
	*********************************************************************/
	UINT ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);
	CB_OBJECT_INFO	*pbMappedcbObject;


	/*********************************************************************
	2019-06-17
	�� ��ü�� Render�� Update �Լ����� IsAlive�� üũ�ϰ� �ֱ� ������ hlsl��
	������ ������Ʈ �ϴ� �κп��� IsAlive�� üũ�ϰ� �߰��Ͽ���.
	*********************************************************************/
	for (unsigned int i = 0; i < m_nProps; i++) {
		if (!m_Props[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedPropObjects + (i * ncbElementBytes));
		memset(pbMappedcbObject, NULL, ncbElementBytes);
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_Props[i]->m_xmf4x4World)));
	}
	for (unsigned int i = 0; i < m_nPlayers; i++) {
		if (!m_Players[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedPlayers + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_Players[i]->m_xmf4x4World)));
	}
	for (unsigned int i = 0; i < m_nProjectiles; i++) {
		if (!m_Projectiles[i]->IsAlive()) continue;
		pbMappedcbObject = (CB_OBJECT_INFO *)((UINT8 *)m_pCBMappedProjectiles + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_Projectiles[i]->m_xmf4x4World)));
	}

	m_pd3dCommandList->SetPipelineState(m_PSO[1]);
	for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->Render(m_pd3dCommandList);
	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->Render(m_pd3dCommandList);

	m_pd3dCommandList->SetPipelineState(m_PSO[0]);
	for (int i = 0; i < m_Players.size(); ++i)		m_Players[i]->Render(m_pd3dCommandList);
}
void CObjectManager::Update(float fTime)
{
	for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->Update(fTime);
	for (int i = 0; i < m_Players.size(); ++i)		m_Players[i]->Update(fTime);
	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->Update(fTime);

	LateUpdate(fTime);
}
void CObjectManager::ProcessInput(UCHAR * pKeysBuffer)
{
	if (m_Players[0]->IsAlive()) m_Players[0]->ProcessInput(pKeysBuffer);

	if (pKeysBuffer[VK_LBUTTON] & 0xF0) {
		/*********************************************************************
		2019-06-17
		���콺�� Ŭ���ϸ� ����� �ϴ� �͵�.

		����, IsAlive == false�� ����ü�� ã�´�.
		ã������ �� ��ü�� �ʱ�ȭ�ϰ�(��ġ ����, ���� ���� ���)
		IsAlive = true�� �����Ѵ�.

		Render�� Update�� IsAlive�� true �� ���� �ǰ� �ؾ� �Ѵ�.
		*********************************************************************/
		if (dynamic_cast<CPlayer*>(m_Players[0])->IsShootable()) {
			dynamic_cast<CPlayer*>(m_Players[0])->Shoot();
			auto iter = find_if(m_Projectiles.begin(), m_Projectiles.end(), [](CObject* p) {return !(p->IsAlive()); });
			if (iter != m_Projectiles.end()) {
				dynamic_cast<CProjectile*>((*iter))->Initialize(m_Players[0]);
				(*iter)->SetAlive(true);
			}
		}
	}
}
void CObjectManager::LateUpdate(float fTime)
{
	CollisionCheck();

	for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->LateUpdate(fTime);
	for (int i = 0; i < m_Players.size(); ++i)		m_Players[i]->LateUpdate(fTime);
	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->LateUpdate(fTime);
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
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (m_nObjects));
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (m_nObjects));
}
void CObjectManager::CreateConstantBufferResorce()
{
	UINT ncbElementBytes;
	ncbElementBytes = ((sizeof(CB_OBJECT_INFO) + 255) & ~255);

	m_pd3dCBPropResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_nProps,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBPropResource)	m_pd3dCBPropResource->Map(0, NULL, (void **)&m_pCBMappedPropObjects);

	m_pd3dCBPlayersResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_nPlayers,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBPlayersResource) m_pd3dCBPlayersResource->Map(0, NULL, (void **)&m_pCBMappedPlayers);

	m_pd3dCBProjectilesResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_nProjectiles,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBProjectilesResource) m_pd3dCBProjectilesResource->Map(0, NULL, (void **)&m_pCBMappedProjectiles);
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
	���� count�� �� �ʿ䰡 ���� �� ������??
	�� ���� �ּҴ� ��¥�� �ٷ� ������ �޾ƿ��ݾ�.
	*********************************************************************/
	if (nullptr != m_pd3dCBPropResource) {
		d3dGpuVirtualAddress = m_pd3dCBPropResource->GetGPUVirtualAddress();
		for (unsigned int i = 0; i < m_nProps; i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i);
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}
	if (nullptr != m_pd3dCBPlayersResource) {
		d3dGpuVirtualAddress = m_pd3dCBPlayersResource->GetGPUVirtualAddress();
		for (unsigned int i = 0; i < m_nPlayers; i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (i + m_nProps));
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}
	if (nullptr != m_pd3dCBProjectilesResource) {
		d3dGpuVirtualAddress = m_pd3dCBProjectilesResource->GetGPUVirtualAddress();
		for (unsigned int i = 0; i < m_nProjectiles; i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * (i + m_nProps + m_nPlayers));
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
	m_d3dSrvCPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	pTexture->SetRootArgument(g_RootParameterTexture, d3dSrvGPUDescriptorHandle);
	m_d3dSrvGPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
}
void CObjectManager::CreateObjectData()
{
	/*********************************************************************
	2019-06-14
	������ �����ؾ� �Ѵ�. ���� �����Ϳ��� ������ ������ ��ġ ���� ���;� ��.
	������ �����ϱ� ���� ������.
	*********************************************************************/



	/*********************************************************************
	2019-06-15
	������ ���� �����ϱ� ���� �������� �����ش�. ������ ���Ƿ� ������ ���߿���
	m_nProps�� LevelData���� �о����, �������� Defines.h���� ������ ��.
	*********************************************************************/
	CImporter importer(m_pd3dDevice, m_pd3dCommandList);
	m_nProps = 10;
	m_nPlayers = 1;
	m_nProjectiles = m_nPlayers * g_NumProjectilePerPlayer;
	m_nObjects = m_nProps + m_nPlayers + m_nProjectiles;

	CreateDescriptorHeap();
	/*********************************************************************
	2019-06-15
	�ؽ�ó�� ���⼭ �־�� �� �� ����. �ؽ�ó�� ���� �����д�.
	�ؽ�ó�� ������ �� ����� ���� �ؾ� �Ǵ°� �Ƴ�?
	*********************************************************************/
	int nTexture = 2;
	wstring fileNames[2];
	fileNames[0] = LASSETPATH;
	fileNames[0] += L"0615_Box_diff.dds";
	fileNames[1] = LASSETPATH;
	fileNames[1] += L"character_2_diff_test3.dds";
	for (int i = 0; i < nTexture; ++i) {
		CTexture* texture = new CTexture(RESOURCE_TEXTURE2D);
		texture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, fileNames[i].c_str());
		CreateTextureResourceView(texture);
		m_TextureList.push_back(texture);
	}

	/*********************************************************************
	2019-06-15
	�ؽ�ó�� ���⼭ �־�� �� �� ����. �ؽ�ó�� ���� �����д�.
	�ؽ�ó�� ������ �� ����� ���� �ؾ� �Ǵ°� �Ƴ�?
	*********************************************************************/
	CreateConstantBufferResorce();
	CreateConstantBufferView();

	int count = 0;
	for (unsigned int i = 0; i < m_nProps; i++) {
		CObject* obj = new CObject();

		importer.ImportModel("0615_Box", m_TextureList[0], obj);
		obj->SetPosition(0, 0, i * 64.0f);
		obj->AddCollider(BoundingSphere(obj->GetPosition(), 10.0f));
		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);
		m_Props.push_back(obj);
	}
	for (unsigned int i = 0; i < m_nPlayers; i++) {
		CPlayer* obj = new CPlayer();

		importer.ImportModel("0603_CharacterIdle", m_TextureList[1], obj);
		importer.ImportAnimClip("0603_CharacterIdle", obj);
		importer.ImportAnimClip("0603_CharacterRun", obj);
		importer.ImportAnimClip("0603_CharacterFire", obj);
		importer.ImportAnimClip("0603_CharacterStartJump", obj);
		importer.ImportAnimClip("0603_CharacterEndJump", obj);
		importer.ImportAnimClip("0603_CharacterDied", obj);
		obj->SetPosition(100, 0, i * 64.0f);
		obj->SetSpawnPoint(obj->GetPosition());
		obj->AddCollider(BoundingSphere(obj->GetPosition(), 10.0f));
		obj->SetTeam((i % 2) + 1);
		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);
		obj->SetCameraTargetOffset(XMFLOAT3(0, 47, -21));
		m_Players.push_back(obj);
	}
	for (unsigned int i = 0; i < m_nProjectiles; i++) {
		CProjectile* obj = new CProjectile();

		/*********************************************************************
		2019-06-17
		����ü�� ���� �̸� �����ΰ� IsAlive�� false�� �صд�.
		*********************************************************************/
		importer.ImportModel("0615_Box", m_TextureList[0], obj);
		obj->SetAlive(false);
		obj->AddCollider(BoundingSphere(XMFLOAT3(0,0,0), 10.0f));
		obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);
		m_Projectiles.push_back(obj);
	}
}

void CObjectManager::CollisionCheck()
{
	/*********************************************************************
	2019-06-18
	�浹�˻� ǥ
	������������������������������������������������������������������������������������������������������
	��		   ��    Prop    ��   Player   �� Projectile ��
	������������������������������������������������������������������������������������������������������
	��Prop	   ��			��	  O		 ��	   O	  ��
	������������������������������������������������������������������������������������������������������
	��Player	   ��	 O	    ��	  O		 ��	   O	  ��
	������������������������������������������������������������������������������������������������������
	��Projectile��	 O	    ��	  O		 ��			  ��
	������������������������������������������������������������������������������������������������������
	�����ó�� ���ƿ��� ����ź�� ������ ���� ���߰� �� �Ÿ� ��...

	�÷��̾�� �� ������ ����ü�� �����̶� �ϸ� �� �� ����.
	�浹�ϸ�? ���⼭ ó�� �� ��?

	�浸���� �� ó����:
	- �÷��̾ ���Ӱ� �浹�ϸ� �̵� ��ȿ. (�����̵�� ���߿� �����ϱ�)
	- �÷��̾ �÷��̾�� �浹�ϸ� �̵� ��ȿ. (���� ��ġ��� ���߿� �����ϱ�)
	- �÷��̾ ������Ÿ�ϰ� �浹�ϸ� ������Ÿ���� ȿ���� �÷��̾�� ����.
	- ������Ÿ���� ���Ӱ� �浹�ϸ� ������Ÿ�� ��Ȱ��ȭ.

	�̵� ��ȿ�� �Ϸ��� �÷��̾��� Ʈ���Ÿ� �������� �ǳ�?
	�ٵ� �浹üũ�� �̵� �Ŀ� �˻��ؾ� �ϴµ�?
	�׷� LateUpdate()�� ������! �浹�˻縦 �ϰ� �浹������ �� ��ü���� Ʈ���Ÿ� ���ְ�
	LateUpdate()�� �ϰ� �ϱ�.
	������ �浹�ߴ��� �׳� �������ֱ�� ��.
	*********************************************************************/
	for (auto player = m_Players.begin(); player != m_Players.end(); ++player) {
		// Ȱ��ȭ�� �ָ� ó���� ��.
		if ((*player)->IsAlive()) {

			for (auto prop = m_Props.begin(); prop != m_Props.end(); ++prop)
				if (IsCollidable((*player), (*prop)))
					if ((*player)->IsCollide((*prop)))
						(*player)->AddCollideInfo((*prop));

			for (auto otherPlayer = m_Players.begin(); otherPlayer != m_Players.end(); ++otherPlayer)
				if (IsCollidable((*player), (*otherPlayer)))
					if ((*player)->IsCollide((*otherPlayer))) {
						(*player)->AddCollideInfo((*otherPlayer));
						(*otherPlayer)->AddCollideInfo((*player));
					}
			for (auto projectile = m_Projectiles.begin(); projectile != m_Projectiles.end(); ++projectile)
				if (IsCollidable((*player), (*projectile)))
					if ((*player)->IsCollide((*projectile))) {
						(*player)->AddCollideInfo((*projectile));
						(*projectile)->Disable();
					}
		}
	}
	for (auto projectile = m_Projectiles.begin(); projectile != m_Projectiles.end(); ++projectile) {
		// Ȱ��ȭ�� �ָ� ó���� ��.
		if ((*projectile)->IsAlive()) {
			for (auto prop = m_Props.begin(); prop != m_Props.end(); ++prop)
				if (IsCollidable((*projectile), (*prop)))
					if ((*projectile)->IsCollide((*prop))) {
						(*projectile)->Disable();
						break;
					}

		}
	}
}
