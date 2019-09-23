#include "stdafx.h"
#include "Object.h"
#include "AnimationController.h"
#include "Mesh.h"
#include "Importer.h"
#include "Texture.h"
#include "Effect.h"
#include "AI.h"
#include "PSO.h"
#include "Framework.h"

PlayerStats			g_player0Info;
std::queue<Request> g_queueRequest;

/*********************************************************************
2019-06-17
CObject
*********************************************************************/
CObject::CObject()
{
	SetPosition(XMFLOAT3(0, 0, 0));
	m_xmf4x4World = Matrix4x4::Identity();
	m_IsAlive = true;
	m_xmf3CollisionOffset = XMFLOAT3(0, 0, 0);
	m_xmf3CameraTargetOffset = XMFLOAT3(0, 0, 0);
	m_AnimationState = static_cast<int>(0);
	m_Team = TEAM_DEFAULT;
	m_fHeightVelocity = 0.0f;
	m_IsGround = false;
}
CObject::~CObject()
{
	// stl container clear
	m_Collider.clear();
	m_ModelList.clear();
	std::queue<Collider*> empty;
	std::swap(m_CollideInfo, empty);

	// uploadAddress delete
	if (m_pd3dcbAnimation) {
		m_pd3dcbAnimation->Unmap(0, nullptr);
		m_pd3dcbAnimation->Release();
		memset(m_pcbxmAnimation, NULL, (((sizeof(XMMATRIX) * g_nAnimBone) + 255) & ~255));
	}
	if (m_pd3dcbObject) {
		m_pd3dcbObject->Unmap(0, nullptr);
		m_pd3dcbObject->Release();
		memset(m_pcbMappedObject, NULL, ((sizeof(CB_OBJECT_INFO) + 255) & ~255));
	}
	//if(m_pcbxmAnimation) delete m_pcbxmAnimation;
	//if(m_pcbMappedObject) delete m_pcbMappedObject;

	// ID3D12Resource release
	if(m_pd3dcbAnimation) m_pd3dcbAnimation->Release();
	if(m_pd3dcbObject) m_pd3dcbObject->Release();

	// 얘는 나중에 전역 벡터 클리어하면서 지울 것이므로 여기서는 그냥 NULL로 해줌
	m_AnimationController = NULL;
}
void CObject::AddCollisionEffect(CObject * p)
{

}
void CObject::SetMng(CObjectManager * mng)
{
	m_pObjMng = mng;
}
void CObject::SetAnimatedMatrix(CAnimationController * a, float time)
{
	for (int i = 0; i < a->m_AnimData[0]->m_nBoneList; ++i) {
		m_a[i] = XMMatrixTranspose(a->GetFinalMatrix(i, time));
	}
}
void CObject::CreateConstantBufferResource(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
}
void CObject::UpdateConstantBuffer(ID3D12GraphicsCommandList * pd3dCommandList)
{
	if (m_pd3dcbAnimation)
	{
		D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneOffsetsGpuVirtualAddress = m_pd3dcbAnimation->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(g_RootParameterAnimation, d3dcbBoneOffsetsGpuVirtualAddress);

		for (int i = 0; i < g_nAnimBone; i++)
		{
			m_pcbxmAnimation[i] = m_a[i];
		}
	}
}
void CObject::SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(g_RootParameterObject, m_d3dCbvGPUDescriptorHandle);
}
void CObject::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, bool isDebug)
{

	if (!m_IsAlive) return;
	if (!m_ModelList.empty()) {
		if (m_AnimationController) {
			ChangeAnimClip();
			SetAnimatedMatrix(m_AnimationController, m_AnimationTime);
		}
		UpdateConstantBuffer(pd3dCommandList);
		for (auto p = m_ModelList.begin(); p != m_ModelList.end(); ++p) {

			pd3dCommandList->SetGraphicsRootDescriptorTable(g_RootParameterObject, m_d3dCbvGPUDescriptorHandle);
			(*p)->Render(pd3dCommandList, isDebug);
		}
	}
}
void CObject::AddCollider(XMFLOAT3 offset, XMFLOAT3 extents, XMFLOAT4 quaternion, bool trig)
{
	m_Collider.emplace_back(offset, extents, quaternion, trig);
	m_Collider[m_Collider.size() - 1].Update(GetPosition(), GetQuaternion());
}
void CObject::AddCollider(XMFLOAT3 offset, float radius, bool trig)
{
	m_Collider.emplace_back(offset, radius, trig);
	m_Collider[m_Collider.size() - 1].Update(GetPosition(), GetQuaternion());
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
	if (m_AnimationController) return m_AnimationController->GetFinalMatrix(boneIdx, m_AnimationTime);
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
void CObject::SetRotation(const XMFLOAT3& angle)
{
	XMMATRIX xmmtxRotate = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(angle.x), XMConvertToRadians(angle.y), XMConvertToRadians(angle.z)));
	XMFLOAT3 xmf3Right = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	XMFLOAT3 xmf3Up = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	XMFLOAT3 xmf3Look = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);

	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	xmf3Right = Vector3::TransformNormal(xmf3Right, xmmtxRotate);

	xmf3Look = Vector3::Normalize(xmf3Look);
	xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;		m_xmf4x4World._33 = xmf3Look.z;

	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());

}
XMFLOAT3 const CObject::GetCameraTargetPos()
{
	return Vector3::Add(GetPosition(), m_xmf3CameraTargetOffset);
	//return m_xmf3CameraTargetOffset;
}
const XMFLOAT4 CObject::GetQuaternion()
{
	return Vector4::QuatFromMtx(m_xmf4x4World);
}
const bool CObject::IsCollide(const CObject& other, bool trig)
{
	/*********************************************************************
	2019-06-18
	other의 충돌체들과 내 충돌체들을 전부 비교해야 함.
	*********************************************************************/
	if (trig) {
		for (int i = 0; i < m_Collider.size(); ++i)
			for (int j = 0; j < other.m_Collider.size(); ++j)
				if (m_Collider[i].TriggerCheck(other.m_Collider[j])) return true;
	}
	else {

		for (int i = 0; i < m_Collider.size(); ++i)
			for (int j = 0; j < other.m_Collider.size(); ++j)
				if (m_Collider[i].IsCollide(other.m_Collider[j])) return true;
	}

	return false;
}
const bool CObject::IsCollide(const Collider & other, bool trig)
{
	if (trig) {
		for (int i = 0; i < m_Collider.size(); ++i)
			if (m_Collider[i].TriggerCheck(other)) return true;
	}
	else {
	for (int i = 0; i < m_Collider.size(); ++i)
		if (m_Collider[i].IsCollide(other)) return true;
	}

	return false;
}
Collider * const CObject::GetCollisionCollider(Collider& other, bool isMakeAlign)
{
	for (int i = 0; i < m_Collider.size(); ++i) {
		if (isMakeAlign)
			other.SetOrientation(m_Collider[i].GetOrientation());
		if (m_Collider[i].IsCollide(other)) return &m_Collider[i];
	}


	return NULL;
}

/*********************************************************************
2019-06-17
CPlayer
*********************************************************************/
CPlayer::CPlayer() : CObject()
{
	//m_ComprehensiveGroundCollider	= Collider(XMFLOAT3(0, 0, 0), g_fDefaultUnitScale / 2.0f, XMFLOAT3(0, 0, 0), ColliderTag::GROUNDCHECK);
	//m_DetailedGroundCollider		= Collider(XMFLOAT3(0, 0, 0), g_fDefaultUnitScale / 4.0f, XMFLOAT3(0, 0, 0), ColliderTag::GROUNDCHECK);
	m_fSpeed = g_fDefaultPlayerSpeed;
	m_fRemainingTimeOfFire = 0.0f;
	m_fRPM = (1 / static_cast<float>(g_DefaultRPM)) * 60.0f;
	m_xmf3Move = XMFLOAT3(0, 0, 0);
	m_xmf3SpawnPoint = GetPosition();
	m_HealthPoint = g_DefaultHealthPoint;
	m_fRemainingTimeOfRespawn = 0.0f;
	m_rotationInput = XMFLOAT2(0.0f, 0.0f);
	for (int i = 0; i < static_cast<int>(Move::count); ++i) m_trigInput[i] = false;
	m_AIBrain = new CTinMan(new CMoveBehavior(), this);
}
CPlayer::~CPlayer()
{
	delete m_AIBrain;
}
void CPlayer::CreateConstantBufferResource(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	if (m_AnimationController) {

		UINT ncbElementBytes = (((sizeof(XMMATRIX) * g_nAnimBone) + 255) & ~255); //256의 배수
		m_pd3dcbAnimation = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_pd3dcbAnimation->Map(0, NULL, (void **)&m_pcbxmAnimation);
	}
}
void CPlayer::Update(float fTimeElapsed)
{
	/* 죽은 상태에서 해줘야 하는 것:
		- 리스폰까지 남은 시간을 흐른 시간만큼 감소
		- 만약 리스폰까지 남은 시간이 0보다 작다면 플레이어를 다시 소생*/
	m_AnimationTime += fTimeElapsed;


	if (m_IsDied) {
		m_fRemainingTimeOfRespawn -= fTimeElapsed;
		if (m_fRemainingTimeOfRespawn <= 0.0f) {
			m_HealthPoint = g_DefaultHealthPoint;
			SetPosition(m_xmf3SpawnPoint);
			m_fHeightVelocity = 0;
			m_AIBrain->ChangeBehavior(new CMoveBehavior());
			m_IsDied = false;
		}
		return;
	}

	if (stuntime > 0) {
		stuntime -= fTimeElapsed;
	}
	else {
		stuntime = 0;
		m_AIBrain->Update();
	}

	if (dottime > 0) {
		dottime -= fTimeElapsed;
		m_HealthPoint -= 5 * fTimeElapsed;
	}
	else {
		dottime = 0;
	}









	/* 안 죽은 상태에서 해줘야 하는 것:
		- 공격가능까지 남은 시간을 흐른 시간만큼 감소
		- AnimTime을 흐른 시간만큼 증가
		- AnimState를 조건에 따라 관리*/
	if (m_fRemainingTimeOfFire > 0) m_fRemainingTimeOfFire -= fTimeElapsed;
	if (m_fRemainingTimeOfSkill1 > 0) m_fRemainingTimeOfSkill1 -= fTimeElapsed;
	if (m_fRemainingTimeOfSlow > 0) m_fRemainingTimeOfSlow -= fTimeElapsed;
	else m_fSlowFactor = 1.0f;

	if (m_AnimationController) {
		//m_AnimationTime += fTimeElapsed;

		if (static_cast<int>(AnimationState::IDLE) == m_AnimationState) {
			if (IsMoving()) {
				m_AnimationState = static_cast<int>(AnimationState::RUN);
				m_AnimationTime = 0;
			}
		}
		else if (static_cast<int>(AnimationState::RUN) == m_AnimationState) {
			if (!IsMoving()) {
				m_AnimationState = static_cast<int>(AnimationState::IDLE);
				m_AnimationTime = 0;
			}
		}
		else if (static_cast<int>(AnimationState::FIRE) == m_AnimationState) {
			//m_AnimationController->ChangeAnimClip("PlayerFire");
			if (m_AnimationController->IsClipEnd(m_AnimationTime)) {
				m_AnimationState = static_cast<int>(AnimationState::IDLE);
				m_AnimationTime = 0;
			}
		}
		else if (static_cast<int>(AnimationState::ENDJUMP) == m_AnimationState) {
			//m_AnimationController->ChangeAnimClip("PlayerFire");
			if (m_AnimationController->IsClipEnd(m_AnimationTime)) {
				m_AnimationState = static_cast<int>(AnimationState::IDLE);
				m_AnimationTime = 0;
			}
		}
		else if (static_cast<int>(AnimationState::DIE) == m_AnimationState) {
			if (!m_IsDied) {
				m_AnimationState = static_cast<int>(AnimationState::STARTJUMP);
				m_AnimationTime = 0;
			}
		}
	}
}
void CPlayer::LateUpdate(float fTimeElapsed)
{
	bool Falling = false;
	if (!m_IsAlive) return;

	m_fHeightVelocity -= fTimeElapsed * g_Gravity;
	if (m_fHeightVelocity < -1.5) {
		m_fHeightVelocity = -1.5;
		Falling = true;
	}

	//if(m_trigInput[static_cast<int>(Move::SPACE)]) m_fHeightVelocity += 
	if (m_IsOnGround && m_trigInput[static_cast<int>(Move::SPACE)]) {
		m_AnimationState = static_cast<int>(AnimationState::STARTJUMP);
		m_AnimationTime = 0;

		m_fHeightVelocity = 3;
	}
	m_IsOnGround = false;

	// Queue Clear
	std::queue<Collider*> empty;
	std::swap(m_CollideInfo, empty);

	// 진행 방향 구함
	m_xmf3Move = Move(fTimeElapsed);
	float radius = g_fDefaultUnitScale / 2.0f;

	/* For Test */
	int s = 0;


	while (true) {
		Collider DetailedGroundCollider(XMFLOAT3(0, 0, 0), radius);
		DetailedGroundCollider.Update(Vector3::Add(XMFLOAT3(0, 8, 0),  Vector3::Add(GetPosition(), m_xmf3Move)), XMFLOAT4(0, 0, 0, 1));
		Collider* collider = m_pObjMng->GetCollider(DetailedGroundCollider, ColliderTag::PROP, false);
		if (NULL == collider) break;

		s++;
		if (s > 10) collider->m_trigCollided = true;

		XMFLOAT3 look = collider->GetLook();
		XMFLOAT3 up = collider->GetUp();
		XMFLOAT3 right = Vector3::CrossProduct(up, look);

		XMVECTOR planeXY = XMPlaneFromPointNormal(XMLoadFloat3(&collider->GetCenter()), XMLoadFloat3(&look));
		XMVECTOR planeXZ = XMPlaneFromPointNormal(XMLoadFloat3(&collider->GetCenter()), XMLoadFloat3(&up));
		XMVECTOR planeYZ = XMPlaneFromPointNormal(XMLoadFloat3(&collider->GetCenter()), XMLoadFloat3(&right));

		XMVECTOR temp;
		XMFLOAT3 xmf3Temp;
		XMFLOAT3 xmf3MyExtents;



		temp = XMPlaneDotCoord(planeYZ, XMLoadFloat3(&GetPosition()));
		XMStoreFloat3(&xmf3Temp, temp);
		xmf3MyExtents.x = xmf3Temp.x;

		temp = XMPlaneDotCoord(planeXY, XMLoadFloat3(&GetPosition()));
		XMStoreFloat3(&xmf3Temp, temp);
		xmf3MyExtents.z = xmf3Temp.x;

		temp = XMPlaneDotCoord(planeXZ, XMLoadFloat3(&GetPosition()));
		XMStoreFloat3(&xmf3Temp, temp);
		xmf3MyExtents.y = xmf3Temp.x;

		XMFLOAT3 colliderExtents = collider->GetExtents();
		XMFLOAT3 dir(0, 0, 0);

		//if (xmf3MyExtents.x >= 0) { if (IsIn(xmf3MyExtents.x, colliderExtents.x, colliderExtents.x + radius)) dir = Vector3::Add(dir, Vector3::Normalize(right)); }
		//else { if (IsIn(xmf3MyExtents.x, -1 * colliderExtents.x - radius, -1 * colliderExtents.x)) dir = Vector3::Add(dir, Vector3::Normalize(Vector3::Multiply(-1, right))); }
		//if (xmf3MyExtents.y >= 0) { if (IsIn(xmf3MyExtents.y, colliderExtents.y, colliderExtents.y + radius)) dir = Vector3::Add(dir, Vector3::Normalize(up)); }
		//else { if (IsIn(xmf3MyExtents.y, -1 * colliderExtents.y - radius, -1 * colliderExtents.y)) dir = Vector3::Add(dir, Vector3::Normalize(Vector3::Multiply(-1, up))); }
		//if (xmf3MyExtents.z >= 0) { if (IsIn(xmf3MyExtents.z, colliderExtents.z, colliderExtents.z + radius)) dir = Vector3::Add(dir, Vector3::Normalize(look)); }
		//else { if (IsIn(xmf3MyExtents.z, -1 * colliderExtents.z - radius, -1 * colliderExtents.z)) dir = Vector3::Add(dir, Vector3::Normalize(Vector3::Multiply(-1, look))); }


		//if (xmf3MyExtents.x >= 0)	{ if ( xmf3MyExtents.x - colliderExtents.x < radius) dir = Vector3::Add(dir, Vector3::Normalize(right));}
		//else						{ if (-xmf3MyExtents.x - colliderExtents.x < radius) dir = Vector3::Add(dir, Vector3::Normalize(Vector3::Multiply(-1, right)));}
		//if (xmf3MyExtents.y >= 0)	{ if ( xmf3MyExtents.y - colliderExtents.y < radius) dir = Vector3::Add(dir, Vector3::Normalize(up));}
		//else						{ if (-xmf3MyExtents.y - colliderExtents.y < radius) dir = Vector3::Add(dir, Vector3::Normalize(Vector3::Multiply(-1, up)));}
		//if (xmf3MyExtents.z >= 0)	{ if ( xmf3MyExtents.z - colliderExtents.z < radius) dir = Vector3::Add(dir, Vector3::Normalize(look));}
		//else						{ if (-xmf3MyExtents.z - colliderExtents.z < radius) dir = Vector3::Add(dir, Vector3::Normalize(Vector3::Multiply(-1, look)));}

		XMVECTOR parallel, perpendicular;

		//if (IsIn(abs(xmf3MyExtents.x), colliderExtents.x, colliderExtents.x + radius) || 
		//	IsIn(abs(xmf3MyExtents.y), colliderExtents.y, colliderExtents.y + radius) || 
		//	IsIn(abs(xmf3MyExtents.z), colliderExtents.z, colliderExtents.z + radius)) {
		//	// 이전 위치가 CollisionZone 안일 경우 다음 진행(그 라운드 코너 부분)
		//	if (IsIn(xmf3MyExtents.x, colliderExtents.x, colliderExtents.x + radius))	dir = Vector3::Add(dir, right);
		//	if (IsIn(xmf3MyExtents.y, colliderExtents.y, colliderExtents.y + radius))	dir = Vector3::Add(dir, up);
		//	if (IsIn(xmf3MyExtents.z, colliderExtents.z, colliderExtents.z + radius))	dir = Vector3::Add(dir, look);
		//	if (IsIn(xmf3MyExtents.x, -colliderExtents.x - radius, -colliderExtents.x))	dir = Vector3::Add(dir, Vector3::Multiply(-1, right));
		//	if (IsIn(xmf3MyExtents.y, -colliderExtents.y - radius, -colliderExtents.y))	dir = Vector3::Add(dir, Vector3::Multiply(-1, up));
		//	if (IsIn(xmf3MyExtents.z, -colliderExtents.z - radius, -colliderExtents.z))	dir = Vector3::Add(dir, Vector3::Multiply(-1, look));
		//}
		//else {
		//	// 이전 위치가 CollisionZone 밖일 경우 아래로 진행
		//	float max = abs(xmf3MyExtents.x) - colliderExtents.x; int colliderDir = 0;
		//	if (abs(xmf3MyExtents.y) - colliderExtents.y > max) { max = abs(xmf3MyExtents.y) - colliderExtents.y; colliderDir = 1; }
		//	if (abs(xmf3MyExtents.z) - colliderExtents.z > max) { max = abs(xmf3MyExtents.z) - colliderExtents.z; colliderDir = 2; }

		//	if (colliderDir == 0) {
		//		if (xmf3MyExtents.x > 0)	dir = Vector3::Normalize(right);
		//		else					dir = Vector3::Normalize(Vector3::Multiply(-1, right));
		//	}
		//	else if (colliderDir == 1) {
		//		if (xmf3MyExtents.y > 0)	dir = Vector3::Normalize(up);
		//		else						dir = Vector3::Normalize(Vector3::Multiply(-1, up));
		//	}
		//	else {
		//		if (xmf3MyExtents.z > 0)	dir = Vector3::Normalize(look);
		//		else						dir = Vector3::Normalize(Vector3::Multiply(-1, look));
		//	}
		//}
		float max = abs(xmf3MyExtents.x) - colliderExtents.x; int colliderDir = 0;
		if (abs(xmf3MyExtents.y) - colliderExtents.y > max) { max = abs(xmf3MyExtents.y) - colliderExtents.y; colliderDir = 1; }
		if (abs(xmf3MyExtents.z) - colliderExtents.z > max) { max = abs(xmf3MyExtents.z) - colliderExtents.z; colliderDir = 2; }

		if (colliderDir == 0) {
			if (xmf3MyExtents.x > 0)	dir = Vector3::Normalize(right);
			else					dir = Vector3::Normalize(Vector3::Multiply(-1, right));
		}
		else if (colliderDir == 1) {
			if (xmf3MyExtents.y > 0)	dir = Vector3::Normalize(up);
			else						dir = Vector3::Normalize(Vector3::Multiply(-1, up));
		}
		else {
			if (xmf3MyExtents.z > 0)	dir = Vector3::Normalize(look);
			else						dir = Vector3::Normalize(Vector3::Multiply(-1, look));
		}

		dir = Vector3::Normalize(dir);

		XMVector3ComponentsFromNormal(&parallel, &perpendicular, XMLoadFloat3(&m_xmf3Move), XMLoadFloat3(&dir));

		XMStoreFloat3(&m_xmf3Move, perpendicular);

		//if(Vector3::DotProduct(dir, xmf3Temp) > 0)	m_xmf3Move = xmf3Temp;

		// 지면 판정
		if (Vector3::DotProduct(dir, XMFLOAT3(0, 1, 0)) > 0.8) {
			if (Falling) {
				m_AnimationState = static_cast<int>(AnimationState::ENDJUMP);
				m_AnimationTime = 0;
			}
			m_fHeightVelocity = fTimeElapsed * g_Gravity;
			m_IsOnGround = true;
		}

		/* For Test */
		xmf3MyExtents.x = abs(xmf3MyExtents.x);
		xmf3MyExtents.y = abs(xmf3MyExtents.y);
		xmf3MyExtents.z = abs(xmf3MyExtents.z);

	}
	m_pObjMng->ColliderTrigInit(ColliderTag::PROP);

	/* For Test */

	XMFLOAT3 xmf3Right = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	XMFLOAT3 xmf3Up = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	XMFLOAT3 xmf3Look = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);

	XMFLOAT2 rotationAngle = Rotate(fTimeElapsed);

	// X축 기준 회전값은 저장해서 관리하기.
	m_fXAxisRotation += rotationAngle.y;
	if (m_fXAxisRotation > 85) 
		m_fXAxisRotation = 85;
	if (m_fXAxisRotation < -70) 
		m_fXAxisRotation = -70;

	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(rotationAngle.x));
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

	xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Right), XMConvertToRadians(m_fXAxisRotation));
	xmf3Look	= Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	xmf3Up		= Vector3::TransformNormal(xmf3Up, xmmtxRotate);

	xmf3Look = Vector3::Normalize(xmf3Look);
	xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);
	xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf2RotatedLook	= xmf3Look;
	m_xmf2RotatedUp		= xmf3Up;


	XMStoreFloat3(&m_xmf3CameraTargetOffset, XMVector3Rotate(XMLoadFloat3(&m_xmf3CameraTargetOffset), XMQuaternionRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(rotationAngle.x))));


	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());

	TriggerOff();
	if (m_HealthPoint <= 0 && !m_IsDied) {
		m_IsDied = true;
		m_fRemainingTimeOfRespawn = g_RespawnDelay;
		m_fRemainingTimeOfSlow = 0;
		m_fRemainingTimeOfFire = 0;
		m_fRemainingTimeOfSkill1 = 0;
		stuntime = 0;
		dottime = 0;
		m_AnimationState = static_cast<int>(AnimationState::DIE);
		m_AnimationTime = 0;
		//CPlayer::Disable();
	}
}
void CPlayer::ProcessInput(UCHAR * pKeysBuffer, XMFLOAT2 mouse)
{

	if (g_IsMouseMode) {
		m_rotationInput = mouse;
	}
	else {
		if (pKeysBuffer[KEY::Q] & 0xF0) m_trigInput[static_cast<int>(Move::Q)] = true;
		if (pKeysBuffer[KEY::E] & 0xF0) m_trigInput[static_cast<int>(Move::E)] = true;
	}
	if (m_IsDied) return;

	if (pKeysBuffer[KEY::W] & 0xF0) m_trigInput[static_cast<int>(Move::W)] = true;
	if (pKeysBuffer[KEY::A] & 0xF0) m_trigInput[static_cast<int>(Move::A)] = true;
	if (pKeysBuffer[KEY::S] & 0xF0) m_trigInput[static_cast<int>(Move::S)] = true;
	if (pKeysBuffer[KEY::D] & 0xF0) m_trigInput[static_cast<int>(Move::D)] = true;
	if (pKeysBuffer[VK_SPACE] & 0xF0) m_trigInput[static_cast<int>(Move::SPACE)] = true;


}
void CPlayer::Enable()
{
	m_HealthPoint = g_DefaultHealthPoint;
	SetPosition(m_xmf3SpawnPoint);
	m_fHeightVelocity = 0;
	m_AIBrain->ChangeBehavior(new CMoveBehavior());
	stuntime = 0;
	dottime = 0;
	/*********************************************************************
	2019-07-03
	위치를 초기화시킬 때, 플레이어의 위치에 오프셋만큼 더해서(상대좌표 개념) 옮겨준다.

	2019-07-06
	충돌체 위치 원래 변경 해줬었는데 이제 Update()에서 자동으로 변경할 예정이므로 삭제함.

	*********************************************************************/
	CObject::Enable();
}
void CPlayer::Disable()
{
	m_fRemainingTimeOfRespawn = g_RespawnDelay;
	m_fRemainingTimeOfSlow = 0;
	m_fRemainingTimeOfFire = 0;
	m_fRemainingTimeOfSkill1 = 0;
	stuntime = 0;
	dottime = 0;

	CObject::Disable();
}
void CPlayer::Shoot()
{
	if (m_IsDied) return;

	m_AnimationState = static_cast<int>(AnimationState::FIRE);
	m_AnimationTime = 0;
	m_fRemainingTimeOfFire = m_fRPM;
}
void CPlayer::Skill(int idx)
{
	if (m_IsDied) return;

	m_AnimationState = static_cast<int>(AnimationState::FIRE);
	m_AnimationTime = 0;
	m_fRemainingTimeOfSkill1 = 2.0f;
}
void CPlayer::Slow()
{
	m_fRemainingTimeOfSlow = 3.0f;
	m_fSlowFactor = 0.5;
}
bool CPlayer::IsShootable()
{
	if (m_IsDied) return false;
	return m_fRemainingTimeOfFire <= 0;
}
bool CPlayer::IsSkillUseable(int idx)
{
	if (m_IsDied) return false;
	return m_fRemainingTimeOfSkill1 <= 0;
}
const XMFLOAT3 CPlayer::GetLook()
{

	return m_xmf2RotatedLook;
}
const XMFLOAT3 CPlayer::GetUp()
{
	return m_xmf2RotatedUp;
}
const XMFLOAT3 CPlayer::GetUnRotatedLook()
{
	XMFLOAT3 vector = { m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33 };
	return Vector3::Normalize(vector);
}
const XMFLOAT3 CPlayer::GetUnRotatedUp()
{
	XMFLOAT3 vector = { m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23 };
	return Vector3::Normalize(vector);
}
void CPlayer::Stun()
{
	stuntime = 3;
}
void CPlayer::DOT()
{
	dottime = 3;
}
void CPlayer::ChangeAnimClip()
{
	if (static_cast<int>(AnimationState::IDLE) == m_AnimationState)
		m_AnimationController->ChangeAnimClip("PlayerIdle");
	if (static_cast<int>(AnimationState::FIRE) == m_AnimationState)
		m_AnimationController->ChangeAnimClip("PlayerFire");
	if (static_cast<int>(AnimationState::RUN) == m_AnimationState)
		m_AnimationController->ChangeAnimClip("PlayerRun");
	if (static_cast<int>(AnimationState::STARTJUMP) == m_AnimationState)
		m_AnimationController->ChangeAnimClip("PlayerStartJump");
	if (static_cast<int>(AnimationState::ENDJUMP) == m_AnimationState)
		m_AnimationController->ChangeAnimClip("PlayerEndJump");
	if (static_cast<int>(AnimationState::DIE) == m_AnimationState)
		m_AnimationController->ChangeAnimClip("PlayerDied");
}
void CPlayer::TriggerOff()
{
	for (int i = 0; i < static_cast<int>(Move::count); i++) m_trigInput[i] = false;
	m_rotationInput.x = 0;
	m_rotationInput.y = 0;
}
XMFLOAT3 CPlayer::Move(float fTimeElapsed)
{
	XMFLOAT3 temp(0, 0, 0);
	if (m_trigInput[static_cast<int>(Move::W)]) { temp = Vector3::Add(temp, Vector3::Normalize(GetUnRotatedLook())); }
	if (m_trigInput[static_cast<int>(Move::A)]) { temp = Vector3::Add(temp, Vector3::Normalize(Vector3::Multiply(-1, GetRight()))); }
	if (m_trigInput[static_cast<int>(Move::S)]) { temp = Vector3::Add(temp, Vector3::Normalize(Vector3::Multiply(-1, GetUnRotatedLook()))); }
	if (m_trigInput[static_cast<int>(Move::D)]) { temp = Vector3::Add(temp, Vector3::Normalize(GetRight())); }
	temp = Vector3::Normalize(temp);

	temp.y += m_fHeightVelocity;

	return Vector3::Multiply(fTimeElapsed * m_fSpeed * m_fSlowFactor, temp);
}
XMFLOAT2 CPlayer::Rotate(float fTimeElapsed)
{
	XMFLOAT2 temp = XMFLOAT2(0,0);
	//if (g_IsMouseMode) {
		temp.x += m_rotationInput.x * fTimeElapsed * g_MouseInputSensitivity;
		temp.y += m_rotationInput.y * fTimeElapsed * 10.0f;
	//}
	//else {
	//	if (m_trigInput[static_cast<int>(Move::Q)]) temp += fTimeElapsed * m_fSpeed * -1;
	//	if (m_trigInput[static_cast<int>(Move::E)]) temp += fTimeElapsed * m_fSpeed;
	//}

	return temp;
}
//void CPlayer::SetHandMatrix()
//{
//	if(m_AnimationController) m_xmf4x4Hand = m_AnimationController->GetPositionOfBone("Bip001 L Hand");
//}


/*********************************************************************
2019-06-17
CProjectile
*********************************************************************/
CProjectile::CProjectile() : CObject()
{
	m_IsAlive = false;
	m_xmf3Direction = XMFLOAT3(0, 0, 0);
	m_fSpeed = g_fDefaultProjectileSpeed;
	m_fLifeTime = g_DefaultProjectileLifeTime;
	m_BaseDamage = g_DefaultDamage;
}
CProjectile::~CProjectile()
{

}
void CProjectile::Initialize(CObject * obj)
{
	/*********************************************************************
	2019-06-17
	여기서 해줘야 하는 것은
	인자로 받아온 오브젝트로부터 위치와 방향, 팀을 받아야 한다는 것.

	아 투사체가 회전하면서 내려와야 하면 흠. 그러면 흠. 방향을 흠.

	충돌체 위치도 바꿔줘야 함.
	*********************************************************************/
	XMFLOAT3 pos = obj->GetPosition();
	pos.y += 10;
	SetPosition(pos);
	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());


	/*********************************************************************
	2019-06-17
	렌더링할 메쉬의 방향
	*********************************************************************/
	SetLook(obj->GetLook());
	SetUp(obj->GetUp());
	SetRight(obj->GetRight());

	SetTeam(obj->GetTeam());

	m_xmf3Direction = Vector3::Normalize(obj->GetLook());
	m_fLifeTime = g_DefaultProjectileLifeTime;


	/*********************************************************************
	2019-06-18
	손의 행렬을 얻어왔다. 그럼 그 행렬로부터 위치랑 방향을 뽑아내면?

	지금은 미완성. 위에걸로 잠시 쓰자.
	*********************************************************************/
	//XMFLOAT4X4 hand = dynamic_cast<CPlayer*>(obj)->GetHandMatrix();
	//XMFLOAT3 xmf3Right		= XMFLOAT3(hand._11, hand._12, hand._13);
	//XMFLOAT3 xmf3Up			= XMFLOAT3(hand._21, hand._22, hand._23);
	//XMFLOAT3 xmf3Look		= XMFLOAT3(hand._31, hand._32, hand._33);
	//XMFLOAT3 xmf3Position	= XMFLOAT3(hand._41, hand._42, hand._43);
	//xmf3Position = Vector3::Add(xmf3Position, obj->GetPosition());
	//SetPosition(xmf3Position);
	//for (auto myb = m_CollisionBox.begin(); myb != m_CollisionBox.end(); ++myb)
	//	myb->Center = xmf3Position;
	//for (auto mys = m_CollisionSphere.begin(); mys != m_CollisionSphere.end(); ++mys)
	//	mys->Center = xmf3Position;
	//SetLook(xmf3Look);
	//SetUp(xmf3Up);
	//SetRight(xmf3Right);
}
//void CProjectile::Initialize(CObject * obj, const char * modelName, Effect * effect)
//{
//	XMFLOAT3 pos = obj->GetPosition();
//	pos.y += 25;
//	SetPosition(pos);
//	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());
//
//	fallingVelocity = 0;
//	m_ModelList.clear();
//	AddModel(GetModelByName(modelName));
//	m_vecEffects.push_back(effect);
//
//	SetLook(obj->GetLook());
//	SetUp(obj->GetUp());
//	SetRight(obj->GetRight());
//
//	SetTeam(obj->GetTeam());
//
//	m_xmf3Direction = Vector3::Normalize(obj->GetLook());
//	m_xmf3Direction.y += 0.1f;
//	m_xmf3Direction = Vector3::Normalize(m_xmf3Direction);
//	m_fLifeTime = g_DefaultProjectileLifeTime;
//}
void CProjectile::Initialize(CObject * obj, const char * modelName, bool isSkill)
{
	XMFLOAT3 pos = obj->GetPosition();
	pos.y += 25;
	SetPosition(pos);
	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());

	fallingVelocity = 0;
	m_ModelList.clear();
	AddModel(GetModelByName(modelName));

	m_IsSkill = isSkill;

	if (!isSkill) {
		m_vecEffects.push_back(new EDefaultDamage());
	}


	SetLook(obj->GetLook());
	SetUp(obj->GetUp());
	SetRight(obj->GetRight());

	SetTeam(obj->GetTeam());

	m_xmf3Direction = Vector3::Normalize(obj->GetLook());
	if (isSkill) {
		if (!g_SkillData.m_IsActivated[SkillType::STRAIGHT] && !g_SkillData.m_IsActivated[SkillType::AROUND]) {
			m_xmf3Direction.y += 0.1f;
			m_xmf3Direction = Vector3::Normalize(m_xmf3Direction);
		}
	}
	m_fLifeTime = g_DefaultProjectileLifeTime;

}
void CProjectile::Update(float fTimeElapsed)
{
	if (!m_IsAlive) return;
	
	/*********************************************************************
	2019-06-18
	LifeTime 관리
	*********************************************************************/
	m_fLifeTime -= fTimeElapsed;
	if (IsExpired()) { Disable(); return; }

	XMFLOAT3 xmf3Move = m_xmf3Direction;
	xmf3Move = Vector3::Multiply(m_fSpeed, xmf3Move);
	xmf3Move = Vector3::Multiply(fTimeElapsed, xmf3Move);

	if (m_IsSkill) {
		if (!g_SkillData.m_IsActivated[SkillType::STRAIGHT] && !g_SkillData.m_IsActivated[SkillType::AROUND]) {
			fallingVelocity -= fTimeElapsed * 0.2;
			m_xmf3Direction.y += fallingVelocity;
			m_xmf3Direction = Vector3::Normalize(m_xmf3Direction);
		}
	}

	m_xmf4x4World._41 += xmf3Move.x;
	m_xmf4x4World._42 += xmf3Move.y;
	m_xmf4x4World._43 += xmf3Move.z;

	//if (m_AnimationController) m_AnimationController->Update(fTimeElapsed);

	///*********************************************************************
	//2019-06-18
	//충돌체 파트
	//*********************************************************************/
	//for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());

}
void CProjectile::LateUpdate(float fTimeElapsed)
{
	if (!m_IsAlive) return;
	for (int i = 0; i < m_Collider.size(); ++i) m_Collider[i].Update(GetPosition(), GetQuaternion());

}
void CProjectile::Damage(CObject* obj)
{
	dynamic_cast<CPlayer*>(obj)->TakeDamage(static_cast<int>(m_BaseDamage));
}

void CProjectile::AddCollisionEffect(CObject * p)
{
	if (!m_IsSkill) {
		for (int i = 0; i < m_vecEffects.size(); ++i) {
			m_vecEffects[i]->Act(dynamic_cast<CPlayer*>(p));
		}
	}


}

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
	for (int i = 0; i < g_vecAnimController.size(); ++i) delete g_vecAnimController[i];


	MemoryClear(g_vecAINode);
	MemoryClear(g_vecTexture);
	MemoryClear(g_vecModel);
	MemoryClear(g_vecAnimController);


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
	if(m_pd3dCBPlayersResource)		m_pd3dCBPlayersResource->Release();
	if(m_pd3dCBProjectilesResource)	m_pd3dCBProjectilesResource->Release();
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
		pbMappedcbui->m_fAlpha = m_FloatingUI[i]->GetAlpha();
	}
	for (int i = 0; i < m_DefaultUI.size(); i++) {
		pbMappedcbui = (CB_UI_INFO *)((UINT8 *)m_pCBMappedDefaultUIs + (i * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbui->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_DefaultUI[i]->m_xmf4x4World)));
		pbMappedcbui->m_xmf2Size = m_DefaultUI[i]->GetSize();
		pbMappedcbui->m_fAlpha = m_DefaultUI[i]->GetAlpha();

	}

	m_pd3dCommandList->SetPipelineState(g_vecPSO[static_cast<int>(PSO::DefaultModel)]);
	for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->Render(m_pd3dCommandList);
	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->Render(m_pd3dCommandList);

	m_pd3dCommandList->SetPipelineState(g_vecPSO[static_cast<int>(PSO::AnimatedModel)]);
	for (int i = 0; i < m_Players.size(); ++i)		m_Players[i]->Render(m_pd3dCommandList);

	m_pd3dCommandList->SetPipelineState(g_vecPSO[static_cast<int>(PSO::FloatingUI)]);
	for (int i = 0; i < m_FloatingUI.size(); ++i)			m_FloatingUI[i]->Render(m_pd3dCommandList);
	// 여기에 PSO 변경하는거 넣어
	m_pd3dCommandList->SetPipelineState(g_vecPSO[static_cast<int>(PSO::DefaultUI)]);
	for (int i = 0; i < m_DefaultUI.size(); ++i)			m_DefaultUI[i]->Render(m_pd3dCommandList);

}
void CObjectManager::Update(float fTime)
{
	static float waitForNextScene = 0;
	if (m_Pause) {
		m_DefaultUI[5]->Enable();
		waitForNextScene += fTime;
		if (waitForNextScene > 3) {
			waitForNextScene = 0;
			m_Pause = false;
			m_pFramework->ChangeScene(SceneType::LOBBY);
		}
		return;
	}
	if (SceneType::MAINPLAY == m_SceneType) {

		while (!g_queueRequest.empty()) {
			Request temp = g_queueRequest.front();
			if (RequestType::Shoot == temp.type) {
				if (m_Players[temp.playerIdx]->IsShootable()) {
					m_Players[temp.playerIdx]->Shoot();
					auto iter = find_if(m_Projectiles.begin(), m_Projectiles.end(), [](CObject* p) {return !(p->IsAlive()); });
					if (iter != m_Projectiles.end()) {
						dynamic_cast<CProjectile*>((*iter))->Initialize(m_Players[temp.playerIdx], "Model_PaperBox_box_1");
						(*iter)->SetAlive(true);
						g_System->playSound(g_vecSound[static_cast<int>(SOUND::SHOT)], 0, false, &g_Channel);

					}
				}
			}
			else if (RequestType::Skill0 == temp.type) {
				auto iter = find_if(m_Projectiles.begin(), m_Projectiles.end(), [](CObject* p) {return !(p->IsAlive()); });
				if (iter != m_Projectiles.end()) {
					dynamic_cast<CProjectile*>((*iter))->Initialize(m_Players[temp.playerIdx], "Model_Crystal_default", true);
					(*iter)->SetAlive(true);
					g_System->playSound(g_vecSound[static_cast<int>(SOUND::SHOT)], 0, false, &g_Channel);


				}
			}
			else if (RequestType::MoveForward == temp.type) {
				m_Players[temp.playerIdx]->MoveForwardTrigOn();
			}
			g_queueRequest.pop();
		}


		//int hp = m_Players[1]->GetHP();

		//static_cast<CPlayer*>(m_Players[1])->GetHP() / static_cast<float>(g_DefaultHealthPoint);
		//m_FloatingUI[0]->SetScale(XMFLOAT2(hp / static_cast<float>(g_DefaultHealthPoint), 0.02f));
		////m_DefaultUI[0]->SetPosition(XMFLOAT3(0, f*0.1,0));
		//XMFLOAT3 pos = m_Players[1]->GetPosition();
		//pos.y += 50;
		//pos.x -= 25;
		//m_FloatingUI[0]->SetPosition(pos);

		//Vector3::Multiply(10, m_Players[0]->GetRight());
		//Vector3::Multiply(10, m_Players[0]->GetLook());
		//pos = Vector3::Add( m_Players[0]->GetPosition(), Vector3::Multiply(10, m_Players[0]->GetRight()));
		//pos = Vector3::Add(pos, Vector3::Multiply(10, m_Players[0]->GetLook()));
		//m_FloatingUI[1]->SetPosition(pos);

		//static float f;
		//if (f > 1) f = 0;
		//f += fTime;
		//for (int i = 0; i < m_DefaultUI.size(); ++i) m_DefaultUI[i]->SetAlpha(f);

		for (int i = 5; i < 5 + 10; ++i)
			if (m_FloatingUI[i]->IsAlive()) {
				m_FloatingUI[i]->lifetime -= fTime;
				if (m_FloatingUI[i]->lifetime <= 0) {
					m_FloatingUI[i]->SetAlive(false);
					continue;
				}
				m_FloatingUI[i]->SetLookAt(m_Players[0]->GetPosition());
			}
		if (m_FloatingUI[15]->IsAlive()) {
			m_FloatingUI[15]->lifetime -= fTime;
			if (m_FloatingUI[15]->lifetime <= 0) {
				m_FloatingUI[15]->SetAlive(false);
			}
			else {
				float temp = 0.5 - m_FloatingUI[15]->lifetime;
				temp *= 2;
				m_FloatingUI[15]->SetScale(XMFLOAT2(temp, temp));
				if (g_SkillData.m_IsActivated[SkillType::AROUND]) {

				XMFLOAT3 pos = m_Players[0]->GetPosition();
				pos.x -= m_FloatingUI[15]->GetSize().x * 0.5;
				pos.z += m_FloatingUI[15]->GetSize().y * 0.5;
				m_FloatingUI[15]->SetPosition(pos);
				}
				else {
					XMFLOAT3 pos = g_SkillData.trigPosition;
					pos.x -= m_FloatingUI[15]->GetSize().x * 0.5;
					pos.z += m_FloatingUI[15]->GetSize().y * 0.5;
					m_FloatingUI[15]->SetPosition(pos);
				}

			}
		}

		float f = m_Players[0]->GetHP();
		m_DefaultUI[8]->SetScale(XMFLOAT2(f / 100, 1));
		f = m_Players[0]->m_fRemainingTimeOfSkill1;
		m_DefaultUI[7]->SetScale(XMFLOAT2((2-f) / 2.0f, 1));



	}

	for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->Update(fTime);
	for (int i = 0; i < m_Players.size(); ++i)		m_Players[i]->Update(fTime);
	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->Update(fTime);
	for (int i = 0; i < m_FloatingUI.size(); ++i)	m_FloatingUI[i]->Update(fTime);
	for (int i = 0; i < m_DefaultUI.size(); ++i)	m_DefaultUI[i]->Update(fTime);

	LateUpdate(fTime);
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
		//	cameraOffsetY += 0.05f; m_Players[0]->SetCameraTargetOffset(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
		//}
		//if (pKeysBuffer[KEY::K] & 0xF0) {
		//	cameraOffsetY -= 0.05f; m_Players[0]->SetCameraTargetOffset(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
		//}
		//if (pKeysBuffer[KEY::J] & 0xF0) {
		//	cameraOffsetZ += 0.05f; m_Players[0]->SetCameraTargetOffset(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
		//}
		//if (pKeysBuffer[KEY::L] & 0xF0) {
		//	cameraOffsetZ -= 0.05f; m_Players[0]->SetCameraTargetOffset(XMFLOAT3(0, cameraOffsetY, cameraOffsetZ));
		//}

		if (pKeysBuffer[VK_LBUTTON] & 0xF0) {
			Request temp;
			temp.playerIdx = 0;
			temp.type = RequestType::Shoot;
			g_queueRequest.push(temp);
			/*********************************************************************
			2019-06-17
			마우스를 클릭하면 해줘야 하는 것들.

			먼저, IsAlive == false인 투사체를 찾는다.
			찾았으면 그 객체를 초기화하고(위치 변경, 방향 변경 등등)
			IsAlive = true로 변경한다.

			Render와 Update는 IsAlive가 true 일 때만 되게 해야 한다.
			*********************************************************************/

		}
		if (pKeysBuffer[VK_RBUTTON] & 0xF0) {
			if (m_Players[0]->IsSkillUseable()) {
				m_Players[0]->Skill();
				g_SkillData.Act(m_Players);
				if (g_SkillData.m_IsActivated[SkillType::AROUND]) {
					m_FloatingUI[15]->SetAlive(true);
					m_FloatingUI[15]->SetScale(XMFLOAT2(0, 0));
					m_FloatingUI[15]->lifetime = 0.5;
					m_FloatingUI[15]->SetPosition(m_Players[0]->GetPosition());
				}
			}

			//Request temp;
			//temp.playerIdx = 0;
			//temp.type = RequestType::Skill0;
			//g_queueRequest.push(temp);

		}
	}
	else if (SceneType::LOBBY == m_SceneType) {
		if (pKeysBuffer[VK_RETURN] & 0xF0) m_pFramework->ChangeScene(SceneType::MAINPLAY);

		if (pKeysBuffer[KEY::Q] & 0xF0) {
			g_SkillData.m_IsActivated[SkillType::AROUND] = true;
			m_DefaultUI[7]->SetAlive(true);
		}
		if (pKeysBuffer[KEY::W] & 0xF0) {
			g_SkillData.m_IsActivated[SkillType::STRAIGHT] = true;
			m_DefaultUI[8]->SetAlive(true);

		}
		if (pKeysBuffer[KEY::E] & 0xF0) {
			g_SkillData.m_IsActivated[SkillType::RANGE1] = true;
			m_DefaultUI[9]->SetAlive(true);

		}
		if (pKeysBuffer[KEY::R] & 0xF0) {
			g_SkillData.m_IsActivated[SkillType::POWER1] = true;
			m_DefaultUI[10]->SetAlive(true);

		}
		if (pKeysBuffer[KEY::T] & 0xF0) {
			g_SkillData.m_IsActivated[SkillType::STUN] = true;
			m_DefaultUI[11]->SetAlive(true);

		}
		if (pKeysBuffer[KEY::Y] & 0xF0) {
			g_SkillData.m_IsActivated[SkillType::DOT] = true;
			m_DefaultUI[0]->SetAlive(true);

		}
		if (pKeysBuffer[KEY::A] & 0xF0) {
			g_SkillData.m_IsActivated[SkillType::AROUND] = false;
			m_DefaultUI[7]->SetAlive(false);

		}
		if (pKeysBuffer[KEY::S] & 0xF0) {
			g_SkillData.m_IsActivated[SkillType::STRAIGHT] = false;
			m_DefaultUI[8]->SetAlive(false);

		}
		if (pKeysBuffer[KEY::D] & 0xF0) {
			g_SkillData.m_IsActivated[SkillType::RANGE1] = false;
			m_DefaultUI[9]->SetAlive(false);

		}
		if (pKeysBuffer[KEY::F] & 0xF0) {
			g_SkillData.m_IsActivated[SkillType::POWER1] = false;
			m_DefaultUI[10]->SetAlive(false);

		}
		if (pKeysBuffer[KEY::G] & 0xF0) {
			g_SkillData.m_IsActivated[SkillType::STUN] = false;
			m_DefaultUI[11]->SetAlive(false);

		}
		if (pKeysBuffer[KEY::H] & 0xF0) {
			g_SkillData.m_IsActivated[SkillType::DOT] = false;
			m_DefaultUI[0]->SetAlive(false);

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
void CObjectManager::SetFramework(CFramework * p)
{
	m_pFramework = p;
}
void CObjectManager::LateUpdate(float fTime)
{
	for (auto projectile = m_Projectiles.begin(); projectile != m_Projectiles.end(); ++projectile) {

		for (auto player = m_Players.begin(); player != m_Players.end(); ++player)
			if ((*player)->IsAlive() && !(*player)->m_IsDied)
				if (IsCollidable((*player), (*projectile)))
					if ((*player)->IsCollide(*(*projectile))) {
						(*projectile)->AddCollisionEffect((*player));

						for (int i = 5; i < 5 + 10; ++i) {
							if (!m_FloatingUI[i]->IsAlive()) {
								m_FloatingUI[i]->SetAlive(true);
								m_FloatingUI[i]->lifetime = 0.5;
								XMFLOAT3 pos = (*projectile)->GetPosition();
								pos.y += 16;
								pos.x -= 16;
								m_FloatingUI[i]->SetPosition(pos);
								break;
							}
						}


						if ((*projectile)->m_IsSkill) {
							if (!g_SkillData.m_IsActivated[SkillType::AROUND]) {

								if (!g_SkillData.m_IsActivated[SkillType::RANGE1] && !g_SkillData.m_IsActivated[SkillType::RANGE2]) {
									vector<CPlayer*> temp;
									temp.push_back((*player));
									g_SkillData.GetTarget(temp);
								}
								else {
									g_SkillData.trigFindTarget = true;
									g_SkillData.trigPosition = (*projectile)->GetPosition();
								}
							}
						}
						(*projectile)->Disable();
					}
		for (auto prop = m_Props.begin(); prop != m_Props.end(); ++prop) {
			if ((*prop)->IsCollide(*(*projectile))) {
				if (!g_SkillData.m_IsActivated[SkillType::AROUND]) {

					if (!g_SkillData.m_IsActivated[SkillType::RANGE1] && !g_SkillData.m_IsActivated[SkillType::RANGE2]) {
					}
					else {
						g_SkillData.trigFindTarget = true;
						g_SkillData.trigPosition = (*projectile)->GetPosition();
					}
				}
				(*projectile)->Disable();
			}
		}
	}

	if (g_SkillData.trigFindTarget) {
		vector<CPlayer*> temp;
		float range = 0;
		if (g_SkillData.m_IsActivated[SkillType::RANGE1]) range += 100;
		if (g_SkillData.m_IsActivated[SkillType::RANGE2]) range += 200;

		for (int i = 1; i < m_Players.size(); ++i) {
			if (GetDistance(g_SkillData.trigPosition, m_Players[i]->GetPosition()) < range) {
				temp.push_back(m_Players[i]);
			}
		}

		m_FloatingUI[15]->SetAlive(true);
		m_FloatingUI[15]->SetScale(XMFLOAT2(0, 0));
		m_FloatingUI[15]->lifetime = 0.5;
		m_FloatingUI[15]->SetPosition(g_SkillData.trigPosition);

		g_SkillData.GetTarget(temp);


		g_SkillData.trigFindTarget = false;
	}

	g_SkillData.AffectToTarget();



	if (SceneType::MAINPLAY == m_SceneType) {

		if (m_Players[0]->GetHP() <= 0) m_Players[0]->TakeDamage(-1);


		g_player0Info.pos = m_Players[0]->GetPosition();
		g_player0Info.isDied = m_Players[0]->m_IsDied;

		static float progress = 0;

		if (!m_Players[0]->m_IsDied && m_Players[0]->IsCollide(*m_Props[1], true)) {
			progress += fTime * 0.05f;
			if (progress > 1) {
				progress = 0;
				m_Pause = true;
				m_DefaultUI[0]->Disable();
				m_DefaultUI[1]->Disable();
				m_DefaultUI[2]->Disable();
				m_DefaultUI[3]->Disable();
				return;
			}
			m_DefaultUI[1]->Enable();
			m_DefaultUI[2]->Enable();
			m_DefaultUI[1]->SetScale(XMFLOAT2(progress, 1.0f));
			m_DefaultUI[3]->Enable();
		}
		else {
			m_DefaultUI[1]->Disable();
			m_DefaultUI[2]->Disable();
			m_DefaultUI[3]->Disable();
		}

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
	d3dDescriptorHeapDesc.NumDescriptors = m_nObjects + g_vecTexture.size();
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
	m_pd3dCBPropResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_Props.size(),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBPropResource)	m_pd3dCBPropResource->Map(0, NULL, (void **)&m_pCBMappedPropObjects);

	m_pd3dCBPlayersResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_Players.size(),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBPlayersResource) m_pd3dCBPlayersResource->Map(0, NULL, (void **)&m_pCBMappedPlayers);

	m_pd3dCBProjectilesResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_Projectiles.size(),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBProjectilesResource) m_pd3dCBProjectilesResource->Map(0, NULL, (void **)&m_pCBMappedProjectiles);


	/*
	내가 지금 하던 생각:
	저 m_nUI 같은 변수들은 최대한 vec.size()로 대체하고 미리 값 받아서 vec로 개수 생성하고 그 뒤에 Create() 함수들을 부르는게 낫겠다.

	일단 하던거나 마저 하고.
	*/


	ncbElementBytes = ((sizeof(CB_UI_INFO) + 255) & ~255);
	m_pd3dCBFloatingUIsResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_FloatingUI.size(),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	if (nullptr != m_pd3dCBFloatingUIsResource) m_pd3dCBFloatingUIsResource->Map(0, NULL, (void **)&m_pCBMappedFloatingUIs);

	m_pd3dCBDefaultUIsResource = ::CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, NULL, ncbElementBytes * m_DefaultUI.size(),
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
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * count++);
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}
	if (nullptr != m_pd3dCBPlayersResource) {
		d3dGpuVirtualAddress = m_pd3dCBPlayersResource->GetGPUVirtualAddress();
		for (int i = 0; i < m_Players.size(); i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * count++);
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}
	if (nullptr != m_pd3dCBProjectilesResource) {
		d3dGpuVirtualAddress = m_pd3dCBProjectilesResource->GetGPUVirtualAddress();
		for (int i = 0; i < m_Projectiles.size(); i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * count++);
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
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * count++);
			m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
		}
	}
	if (nullptr != m_pd3dCBDefaultUIsResource) {
		d3dGpuVirtualAddress = m_pd3dCBDefaultUIsResource->GetGPUVirtualAddress();
		for (int i = 0; i < m_DefaultUI.size(); i++) {
			d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (ncbElementBytes * i);
			D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
			d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * count++);
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

	if (_heapchk() != _HEAPOK)
		DebugBreak();

	pTexture->SetRootArgument(g_RootParameterTexture, d3dSrvGPUDescriptorHandle);
	m_d3dSrvGPUDescriptorStartHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	if (_heapchk() != _HEAPOK)
		DebugBreak();
}
void CObjectManager::CreateObjectData()
{
	CImporter importer(m_pd3dDevice, m_pd3dCommandList);

	//m_SkillData = new CSkill();


	if (SceneType::MAINPLAY == m_SceneType) {

		importer.ImportLevel("LevelData_TestMap", m_LevelDataDesc);

		//XMFLOAT3 nodePositions[9] = {
		//	XMFLOAT3(0,0,0),
		//	XMFLOAT3(-200,0,88),
		//	XMFLOAT3(-200,0,230),
		//	XMFLOAT3(-139,0,257),
		//	XMFLOAT3(-127,0,341),
		//	XMFLOAT3(-42,0,387),
		//	XMFLOAT3(-18,0,297),
		//	XMFLOAT3(73,0,270),
		//	XMFLOAT3(56,0,137)
		//};
		//int nodeNextIdx[9] = {
		//	-1,
		//	0,
		//	1,
		//	2,
		//	3,
		//	4,
		//	7,
		//	8,
		//	0
		//};

		//for (int i = 0; i < 9; ++i) {
		//	AINode* tempNode = new AINode();
		//	tempNode->m_xmf3Position = nodePositions[i];
		//	g_vecAINode.push_back(tempNode);
		//}
		//for (int i = 0; i < 9; ++i) {
		//	if(-1 != nodeNextIdx[i]) g_vecAINode[i]->next = g_vecAINode[nodeNextIdx[i]];
		//	else g_vecAINode[i]->next = NULL;
		//}

		XMFLOAT3 nodePositions[1] = {
	XMFLOAT3(0,0,0)
		};
		int nodeNextIdx[1] = {
			-1
		};

		for (int i = 0; i < 1; ++i) {
			AINode* tempNode = new AINode();
			tempNode->m_xmf3Position = nodePositions[i];
			g_vecAINode.push_back(tempNode);
		}
		for (int i = 0; i < 1; ++i) {
			if (-1 != nodeNextIdx[i]) g_vecAINode[i]->next = g_vecAINode[nodeNextIdx[i]];
			else g_vecAINode[i]->next = NULL;
		}

		if (_heapchk() != _HEAPOK)
			DebugBreak();

		int nProps = 2;
		int nPlayers = 5;
		int nFloatingUI = 5 + 10 + 1;
		int nDefaultUI = 6 + 1 + 1 + 1;
		int nProjectiles = nPlayers * g_nProjectilePerPlayer;
		m_nObjects = nProps + nPlayers + nProjectiles + nFloatingUI + nDefaultUI;

		m_Props.resize(nProps);
		m_Players.resize(nPlayers);
		m_Projectiles.resize(nProjectiles);
		m_FloatingUI.resize(nFloatingUI);
		m_DefaultUI.resize(nDefaultUI);

		MemoryClear(g_vecTexture);
		importer.ImportTexture(L"0615_Box_diff", "Texture_PaperBox");
		importer.ImportTexture(L"character_2_diff_test3", "Texture_Character");
		importer.ImportTexture(L"0618_LevelTest_diff", "Texture_Level");
		importer.ImportTexture(L"box_diff", "Texture_StandardBox");
		importer.ImportTexture(L"HealthBar", "Texture_HPBar");
		importer.ImportTexture(L"DefaultMaterial_albedo", "Texture_Chair");
		importer.ImportTexture(L"spotlight_BaseColor", "Texture_Spotlight");

		importer.ImportTexture(L"Bar_ProgressBar", "Texture_ProgressBar");
		importer.ImportTexture(L"Bar_Capturing", "Texture_CapturingPoint");
		importer.ImportTexture(L"Text_Capturing", "Texture_Text_Capturing");
		importer.ImportTexture(L"Text_Defeat", "Texture_Text_Defeat");
		importer.ImportTexture(L"Text_Victory", "Texture_Text_Victory");

		importer.ImportTexture(L"crosshair", "Texture_Crosshair");
		importer.ImportTexture(L"TEX_crystal", "Texture_Crystal");
		//importer.ImportTexture(L"oasisnight_bk", "Texture_SkyBox_bk");
		//importer.ImportTexture(L"oasisnight_dn", "Texture_SkyBox_dn");
		//importer.ImportTexture(L"oasisnight_ft", "Texture_SkyBox_ft");
		//importer.ImportTexture(L"oasisnight_lf", "Texture_SkyBox_lf");
		//importer.ImportTexture(L"oasisnight_rt", "Texture_SkyBox_rt");
		//importer.ImportTexture(L"oasisnight_up", "Texture_SkyBox_up");
		importer.ImportTexture(L"city_bk", "Texture_SkyBox_bk");
		importer.ImportTexture(L"city_dn", "Texture_SkyBox_dn");
		importer.ImportTexture(L"city_ft", "Texture_SkyBox_ft");
		importer.ImportTexture(L"city_lf", "Texture_SkyBox_lf");
		importer.ImportTexture(L"city_rt", "Texture_SkyBox_rt");
		importer.ImportTexture(L"city_up", "Texture_SkyBox_up");
		importer.ImportTexture(L"Text_0", "Texture_Text_0");
		importer.ImportTexture(L"Text_1", "Texture_Text_1");
		importer.ImportTexture(L"Text_2", "Texture_Text_2");
		importer.ImportTexture(L"Text_3", "Texture_Text_3");
		importer.ImportTexture(L"Text_4", "Texture_Text_4");
		importer.ImportTexture(L"Text_5", "Texture_Text_5");
		importer.ImportTexture(L"Text_6", "Texture_Text_6");
		importer.ImportTexture(L"Text_7", "Texture_Text_7");
		importer.ImportTexture(L"Text_8", "Texture_Text_8");
		importer.ImportTexture(L"Text_9", "Texture_Text_9");
		importer.ImportTexture(L"smoke", "Effect_Smoke");
		importer.ImportTexture(L"ring", "Effect_Ring");
		importer.ImportTexture(L"Info", "Info_Infobar");
		importer.ImportTexture(L"Skillbar", "Info_Skillbar");
		CreateDescriptorHeap();

		for (int i = 0; i < g_vecTexture.size(); ++i) CreateTextureResourceView(g_vecTexture[i]);
		importer.ImportModel("0730_LevelTest", "Texture_Level", ModelType::DefaultModel, "Model_Level");
		importer.ImportModel("0725_Character", "Texture_Character", ModelType::AnimatedModel, "Model_Character");
		importer.ImportModel("0725_PaperBox_NoSpitPerVertexNormal", "Texture_PaperBox", ModelType::DefaultModel, "Model_PaperBox");
		importer.ImportModel("crystal", "Texture_Crystal", ModelType::DefaultModel, "Model_Crystal");
		importer.ImportModel("Bench", "Texture_Chair", ModelType::DefaultModel, "Model_Bench");
		importer.ImportModel("Spotlight", "Texture_Crystal", ModelType::DefaultModel, "Model_Spotlight");
		importer.ImportModel("", "Texture_HPBar", ModelType::FloatingUI, "UI_TEST");
		importer.ImportModel("", "Texture_StandardBox", ModelType::FloatingUI, "UI_TEST2");
		importer.ImportModel("", "Texture_Crosshair", ModelType::FloatingUI, "UI_Crosshair");
		importer.ImportModel("", "Texture_SkyBox_bk", ModelType::FloatingUI, "SkyBox_bk");
		importer.ImportModel("", "Texture_SkyBox_dn", ModelType::FloatingUI, "SkyBox_dn");
		importer.ImportModel("", "Texture_SkyBox_ft", ModelType::FloatingUI, "SkyBox_ft");
		importer.ImportModel("", "Texture_SkyBox_lf", ModelType::FloatingUI, "SkyBox_lf");
		importer.ImportModel("", "Texture_SkyBox_rt", ModelType::FloatingUI, "SkyBox_rt");
		importer.ImportModel("", "Texture_SkyBox_up", ModelType::FloatingUI, "SkyBox_up");
		importer.ImportModel("", "Effect_Smoke", ModelType::FloatingUI, "Effect_Smoke");
		importer.ImportModel("", "Effect_Ring", ModelType::FloatingUI, "Effect_Ring");
		importer.ImportModel("", "Info_Infobar", ModelType::FloatingUI, "Info_Infobar");
		importer.ImportModel("", "Info_Skillbar", ModelType::FloatingUI, "Info_Skillbar");

		importer.ImportModel("", "Texture_ProgressBar", ModelType::FloatingUI, "ProgressBarBack");
		importer.ImportModel("", "Texture_CapturingPoint", ModelType::FloatingUI, "ProgressBarFrnt");
		importer.ImportModel("", "Texture_Text_Capturing", ModelType::FloatingUI, "Text_Capturing");
		importer.ImportModel("", "Texture_Text_Defeat", ModelType::FloatingUI, "Text_Defeat");
		importer.ImportModel("", "Texture_Text_Victory", ModelType::FloatingUI, "Text_Victory");

		importer.ImportAnimController("AnimCtrl_Character");

		importer.ImportAnimClip("0603_CharacterIdle", "AnimCtrl_Character", true);
		importer.ImportAnimClip("0603_CharacterRun", "AnimCtrl_Character", true);
		importer.ImportAnimClip("0603_CharacterFire", "AnimCtrl_Character", false);
		importer.ImportAnimClip("0603_CharacterStartJump", "AnimCtrl_Character", false);
		importer.ImportAnimClip("0603_CharacterEndJump", "AnimCtrl_Character", false);
		importer.ImportAnimClip("0603_CharacterDied", "AnimCtrl_Character", false);

		CreateConstantBufferResorce();
		CreateConstantBufferView();

		int count = 0;
		for (int i = 0; i < m_Props.size(); i++) {
			CObject* obj = new CObject();
			obj->SetMng(this);
			if (0 == i) {
				obj->AddModel(GetModelByName("Model_Level_Box042"));
				obj->AddModel(GetModelByName("Model_Level_Box045"));
				obj->AddModel(GetModelByName("Model_Level_Box047"));
				for (int j = 0; j < m_LevelDataDesc.nCollisionMaps; ++j) {
					obj->AddCollider(
						m_LevelDataDesc.CollisionPosition[j],
						m_LevelDataDesc.CollisionScale[j],
						m_LevelDataDesc.CollisionRotation[j]);
				}
			}
			else if (1 == i) {
				obj->AddCollider( XMFLOAT3(0, 0, 0), 100, true);
			}
			else if (2 == i) {

			}
			//else if (1 == i) {
			//	obj->AddModel(GetModelByName("Model_Bench_Cube.005"));
			//	obj->SetPosition(0.0f, 0.0f, 70.0f);
			//	obj->SetRotation(XMFLOAT3(0, 35, 0));
			//}
			//else {
			//	obj->AddModel(GetModelByName("Model_Spotlight_Spotlight"));
			//	obj->SetPosition(30.0f, 0.0f, 0.0f);
			//	obj->SetRotation(XMFLOAT3(0, 80, 0));
			//}
			///else {
			///	//obj->AddModel(importer.GetModelByName("Model_Box2_Box001"));
			///	obj->AddModel(GetModelByName("Model_2B_body"));
			///	//obj->AddModel(GetModelByName("Model_CharacterStatic_body"));
			///	//obj->AddModel(GetModelByName("Model_CharacterStatic_jumper"));
			///	//obj->AddModel(GetModelByName("Model_CharacterStatic_mask"));
			///	obj->SetPosition(0.0f, 0.0f, 0.0f);
			///}

			obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);
			m_Props[i] = obj;
		}
		for (int i = 0; i < m_Players.size(); i++) {
			CPlayer* obj = new CPlayer();
			obj->SetMng(this);
			obj->AddModel(GetModelByName("Model_Character_body"));
			obj->AddModel(GetModelByName("Model_Character_jumper"));
			obj->AddModel(GetModelByName("Model_Character_mask"));

			obj->SetAnimCtrl(GetAnimCtrlByName("AnimCtrl_Character"));

			obj->playerId = i;

			//obj->SetPosition(0, 100, i * g_fDefaultUnitScale * 3);
			obj->AddCollider(
				XMFLOAT3(0, 14, 0),
				XMFLOAT3(6, 8, 6),
				XMFLOAT4(0, 0, 0, 1)
			);
			obj->SetCameraTargetOffset(XMFLOAT3(0, 50, 0));

			if (i == 0) {
				obj->SetTeam(0);
				XMFLOAT3 temp = m_LevelDataDesc.Team1SpawnPointPosition[2]; temp.y += 100;
				obj->SetPosition(temp);
				obj->SetRotation(XMFLOAT3(0, 180, 0));

				//obj->m_SkillData->m_IsActivated[SkillType::]

			}
			else if(i == 1){
				obj->SetTeam(1);
				//XMFLOAT3 temp = m_LevelDataDesc.Team2SpawnPointPosition[0]; temp.y += 50;
				//obj->SetPosition(temp);
				obj->SetPosition(XMFLOAT3(100, 100, 100));
			}
			else if (i == 2) {
				obj->SetTeam(1);
				//XMFLOAT3 temp = m_LevelDataDesc.Team2SpawnPointPosition[1]; temp.y += 100;
				//obj->SetPosition(temp);
				obj->SetPosition(XMFLOAT3(-100, 100, 100));
			}
			else if (i == 3) {
				obj->SetTeam(1);
				//XMFLOAT3 temp = m_LevelDataDesc.Team2SpawnPointPosition[0]; temp.y += 50;
				//obj->SetPosition(temp);
				obj->SetPosition(XMFLOAT3(100, 100, -50));
			}
			else if (i == 4) {
				obj->SetTeam(1);
				//XMFLOAT3 temp = m_LevelDataDesc.Team2SpawnPointPosition[1]; temp.y += 100;
				//obj->SetPosition(temp);
				obj->SetPosition(XMFLOAT3(-100, 100, -50));
			}
			obj->SetSpawnPoint(obj->GetPosition());


			obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);

			if (i >= 1) obj->m_AIBrain->PowerOn();

			m_Players[i] = obj;
		}
		for (int i = 0; i < m_Projectiles.size(); i++) {
			CProjectile* obj = new CProjectile();
			obj->SetMng(this);
			obj->AddModel(GetModelByName("Model_PaperBox_box_1"));

			obj->SetAlive(false);
			obj->AddCollider(XMFLOAT3(0, 0, 0), 10.0f);
			obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);
			m_Projectiles[i] = obj;
		}
		for (int i = 0; i < m_FloatingUI.size(); i++) {
			CUI* obj = new CUI();
			obj->SetMng(this);

			if (0 == i) {
				obj->AddModel(GetModelByName("SkyBox_bk"));
				obj->SetPosition(-1024.0f * 2, 1024.0f * 2, -1024.0f * 2);
				obj->Initialize(XMFLOAT2(2048 * 2, 2048 * 2));

			}
			else if (1 == i) {
				obj->AddModel(GetModelByName("SkyBox_ft"));
				obj->SetPosition(1024.0f * 2, 1024.0f * 2, 1024.0f * 2);
				obj->Initialize(XMFLOAT2(2048 * 2, 2048 * 2));
				obj->SetRotation(XMFLOAT3(0, -180, 0));

			}
			else if (2 == i) {
				obj->AddModel(GetModelByName("SkyBox_rt"));
				obj->SetPosition(1024.0f * 2, 1024.0f * 2, -1024.0f * 2);
				obj->Initialize(XMFLOAT2(2048 * 2, 2048 * 2));
				obj->SetRotation(XMFLOAT3(0, -90, 0));
			}
			else if (3 == i) {
				obj->AddModel(GetModelByName("SkyBox_lf"));
				obj->SetPosition(-1024.0f * 2, 1024.0f * 2, 1024.0f * 2);
				obj->Initialize(XMFLOAT2(2048 * 2, 2048 * 2));
				obj->SetRotation(XMFLOAT3(0, 90, 0));

			}
			else if (4 == i) {
				obj->AddModel(GetModelByName("SkyBox_dn"));
				obj->SetPosition(-1024.0f * 2, -1024.0f * 2, 1024.0f * 2);
				obj->Initialize(XMFLOAT2(2048 * 2, 2048 * 2));
				obj->SetRotation(XMFLOAT3(90, 0, 0));

			}
			else if (4 < i && i < 4 + 10) {
				obj->SetAlive(false);
				obj->AddModel(GetModelByName("Effect_Smoke"));
				obj->Initialize(XMFLOAT2(32, 32));
			}
			else {
				obj->SetAlive(false);

				float range = 200;
				if (g_SkillData.m_IsActivated[SkillType::RANGE1]) range += 200;
				if (g_SkillData.m_IsActivated[SkillType::RANGE2]) range += 400;

				obj->AddModel(GetModelByName("Effect_Ring"));
				obj->Initialize(XMFLOAT2(range, range));
				obj->SetRotation(XMFLOAT3(90, 0, 0));
			}

			obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);
			m_FloatingUI[i] = obj;
		}
		for (int i = 0; i < m_DefaultUI.size(); i++) {
			CUI* obj = new CUI();
			obj->SetMng(this);
			if (0 == i) {
				obj->AddModel(GetModelByName("UI_Crosshair"));
				obj->SetPosition(0.5, 0.35, 0.0f);
				obj->Initialize(XMFLOAT2(50, 50));
			}
			else if (1 == i) {
				obj->AddModel(GetModelByName("ProgressBarFrnt"));
				obj->SetPosition(0.5, 0.25, 0.0f);
				obj->Initialize(XMFLOAT2(475, 8));
				obj->Disable();
			}
			else if (2 == i) {
				obj->AddModel(GetModelByName("ProgressBarBack"));
				obj->SetPosition(0.5, 0.25, 0.0f);
				obj->Initialize(XMFLOAT2(500, 500));
				obj->Disable();

			}
			else if (3 == i) {
				obj->AddModel(GetModelByName("Text_Capturing"));
				obj->SetPosition(0.5, 0.1, 0.0f);
				obj->Initialize(XMFLOAT2(300, 300));
				obj->Disable();
			}
			else if (4 == i) {
				obj->AddModel(GetModelByName("Text_Defeat"));
				obj->SetPosition(0.5, 0.4, 0.0f);
				obj->Initialize(XMFLOAT2(500, 500));
				obj->Disable();

			}
			else if (5 == i) {
				obj->AddModel(GetModelByName("Text_Victory"));
				obj->SetPosition(0.5, 0.4, 0.0f);
				obj->Initialize(XMFLOAT2(500, 500));
				obj->Disable();

			}
			else if (6 == i) {
				obj->AddModel(GetModelByName("Info_Infobar"));
				obj->SetPosition(0.8, 0.8, 0.0f);
				obj->Initialize(XMFLOAT2(250, 250));

			}
			else if (7 == i) {
				obj->AddModel(GetModelByName("Info_Skillbar"));
				obj->SetPosition(0.82, 0.8, 0.0f);
				obj->Initialize(XMFLOAT2(200, 20));

			}
			else if (8 == i) {
				obj->AddModel(GetModelByName("UI_TEST"));
				obj->SetPosition(0.82, 0.75, 0.0f);
				obj->Initialize(XMFLOAT2(200, 20));

			}

			obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);
			m_DefaultUI[i] = obj;
		}

	}
	else if (SceneType::LOBBY == m_SceneType)
	{
		importer.ImportLevel("LevelData_TestMap", m_LevelDataDesc);


		/*********************************************************************
		2019-06-15
		서술자 힙을 생성하기 위해 개수들을 정해준다. 지금은 임의로 하지만 나중에는
		m_nProps는 LevelData에서 읽어오고, 나머지는 Defines.h에서 가져올 것.
		*********************************************************************/
		int nProps = 0;
		int nPlayers = 0;
		int nFloatingUI = 0;
		int nDefaultUI = 1 + 12;
		int nProjectiles = nPlayers * g_nProjectilePerPlayer;
		m_nObjects = nProps + nPlayers + nProjectiles + nFloatingUI + nDefaultUI;

		m_Props.resize(nProps);
		m_Players.resize(nPlayers);
		m_Projectiles.resize(nProjectiles);
		m_FloatingUI.resize(nFloatingUI);
		m_DefaultUI.resize(nDefaultUI);


		/*********************************************************************
		2019-06-15
		텍스처도 여기서 넣어야 할 것 같음. 텍스처를 먼저 만들어둔다.
		텍스처는 서술자 힙 만들고 나서 해야 되는거 아냐?

		2019-07-21
		텍스처를 전역 벡터로 관리하기 시작.
		*********************************************************************/

		g_vecTexture.clear();
		importer.ImportTexture(L"TitleImg_001", "Texture_TitleImg");
		importer.ImportTexture(L"skill_around", "skill_around");
		importer.ImportTexture(L"skill_dot", "skill_dot");
		importer.ImportTexture(L"skill_powerUp", "skill_powerUp");
		importer.ImportTexture(L"skill_rangeUp", "skill_rangeUp");
		importer.ImportTexture(L"skill_straight", "skill_straight");
		importer.ImportTexture(L"skill_stun", "skill_stun");
		importer.ImportTexture(L"skill_highlight", "skill_highlight");
		CreateDescriptorHeap();
		for (int i = 0; i < g_vecTexture.size(); ++i) CreateTextureResourceView(g_vecTexture[i]);

		importer.ImportModel("", "Texture_TitleImg", ModelType::FloatingUI, "UI_TitleImg");

		importer.ImportModel("", "skill_around", ModelType::FloatingUI,		"skill_around");
		importer.ImportModel("", "skill_dot", ModelType::FloatingUI,		"skill_dot");
		importer.ImportModel("", "skill_powerUp", ModelType::FloatingUI,	"skill_powerUp");
		importer.ImportModel("", "skill_rangeUp", ModelType::FloatingUI,	"skill_rangeUp");
		importer.ImportModel("", "skill_straight", ModelType::FloatingUI,	"skill_straight");
		importer.ImportModel("", "skill_stun", ModelType::FloatingUI,		"skill_stun");
		importer.ImportModel("", "skill_highlight", ModelType::FloatingUI,	"skill_highlight");


		CreateConstantBufferResorce();
		CreateConstantBufferView();

		int count = 0;
		for (int i = 0; i < m_Props.size(); i++) {

		}
		for (int i = 0; i < m_Players.size(); i++) {

		}
		for (int i = 0; i < m_Projectiles.size(); i++) {

		}
		for (int i = 0; i < m_FloatingUI.size(); i++) {

		}
		for (int i = 0; i < m_DefaultUI.size(); i++) {
			CUI* obj = new CUI();
			obj->SetMng(this);
			if (i == 12) {
			obj->AddModel(GetModelByName("UI_TitleImg"));
			obj->SetPosition(0.5, 0.5, 0.0f);
			obj->Initialize(XMFLOAT2(960, 540));
			}

			if (i == 1) {
				obj->AddModel(GetModelByName("skill_around"));
				obj->SetPosition(0.4, 0.4, 0.0f);
				obj->Initialize(XMFLOAT2(64, 64));
			}
			if (i == 2) {
				obj->AddModel(GetModelByName("skill_straight"));
				obj->SetPosition(0.4, 0.5, 0.0f);
				obj->Initialize(XMFLOAT2(64, 64));
			}
			if (i == 3) {
				obj->AddModel(GetModelByName("skill_rangeUp"));
				obj->SetPosition(0.5, 0.4, 0.0f);
				obj->Initialize(XMFLOAT2(64, 64));
			}
			if (i == 4) {
				obj->AddModel(GetModelByName("skill_powerUp"));
				obj->SetPosition(0.5, 0.5, 0.0f);
				obj->Initialize(XMFLOAT2(64, 64));
			}
			if (i == 5) {
				obj->AddModel(GetModelByName("skill_stun"));
				obj->SetPosition(0.6, 0.4, 0.0f);
				obj->Initialize(XMFLOAT2(64, 64));
			}
			if (i == 6) {
				obj->AddModel(GetModelByName("skill_dot"));
				obj->SetPosition(0.6, 0.5, 0.0f);
				obj->Initialize(XMFLOAT2(64, 64));
			}
			if (i == 7) {
				obj->AddModel(GetModelByName("skill_highlight"));
				obj->SetPosition(0.4, 0.4, 0.0f);
				obj->Initialize(XMFLOAT2(64, 64));
				obj->SetAlive(false);
			}
			if (i == 8) {
				obj->AddModel(GetModelByName("skill_highlight"));
				obj->SetPosition(0.4, 0.5, 0.0f);
				obj->SetAlive(false);
				obj->Initialize(XMFLOAT2(64, 64));
			}
			if (i == 9) {
				obj->AddModel(GetModelByName("skill_highlight"));
				obj->SetPosition(0.5, 0.4, 0.0f);
				obj->SetAlive(false);
				obj->Initialize(XMFLOAT2(64, 64));
			}
			if (i == 10) {
				obj->AddModel(GetModelByName("skill_highlight"));
				obj->SetPosition(0.5, 0.5, 0.0f);
				obj->Initialize(XMFLOAT2(64, 64));
				obj->SetAlive(false);
			}
			if (i == 11) {
				obj->AddModel(GetModelByName("skill_highlight"));
				obj->SetPosition(0.6, 0.4, 0.0f);
				obj->SetAlive(false);
				obj->Initialize(XMFLOAT2(64, 64));
			}
			if (i == 0) {
				obj->AddModel(GetModelByName("skill_highlight"));
				obj->SetPosition(0.6, 0.5, 0.0f);
				obj->SetAlive(false);
				obj->Initialize(XMFLOAT2(64, 64));
			}


			obj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize) * count++);
			m_DefaultUI[i] = obj;
		}
	}

	if (_heapchk() != _HEAPOK)
		DebugBreak();

	for (int i = 0; i < m_Props.size(); ++i)		m_Props[i]->CreateConstantBufferResource(m_pd3dDevice, m_pd3dCommandList);
	for (int i = 0; i < m_Players.size(); ++i)		m_Players[i]->CreateConstantBufferResource(m_pd3dDevice, m_pd3dCommandList);
	for (int i = 0; i < m_Projectiles.size(); ++i)	m_Projectiles[i]->CreateConstantBufferResource(m_pd3dDevice, m_pd3dCommandList);
	for (int i = 0; i < m_FloatingUI.size(); ++i)	m_FloatingUI[i]->CreateConstantBufferResource(m_pd3dDevice, m_pd3dCommandList);
	for (int i = 0; i < m_DefaultUI.size(); ++i)	m_DefaultUI[i]->CreateConstantBufferResource(m_pd3dDevice, m_pd3dCommandList);

	if (_heapchk() != _HEAPOK)
		DebugBreak();

}
void CObjectManager::MakeDamageUI(int damage, CObject *obj)
{
	vector<int> vec;
	float meshSize = damage * 0.1f;
	while (damage != 0) {
		vec.push_back( damage % 10 );
		damage %= 10;
	}
	for (int i = 0; i < vec.size(); ++i) {
		for (int j = 0; j < m_DamageUI.size(); ++j) {
			if (!m_DamageUI[j]->IsAlive()) {

				m_DamageUI[j]->Initialize(XMFLOAT2(meshSize, meshSize), vec[i]);
				XMFLOAT3 pos = obj->GetPosition();
				pos.x -= meshSize * i;
				m_DamageUI[j]->SetPosition(pos);
				continue;
			}
		}
	}



}
//void CObjectManager::CollisionCheck()
//{
//	/*********************************************************************
//	2019-06-18
//	충돌검사 표
//	┼──────────┼────────────┼────────────┼────────────┼
//	│		   │    Prop    │   Player   │ Projectile │
//	┼──────────┼────────────┼────────────┼────────────┼
//	│Prop	   │			│	  O		 │	   O	  │
//	┼──────────┼────────────┼────────────┼────────────┼
//	│Player	   │	 O	    │	  O		 │	   O	  │
//	┼──────────┼────────────┼────────────┼────────────┼
//	│Projectile│	 O	    │	  O		 │			  │
//	┼──────────┼────────────┼────────────┼────────────┼
//	디비전처럼 날아오는 수류탄을 총으로 쏴서 맞추고 할 거면 뭐...
//
//	플레이어로 다 돌리고 투사체로 프롭이랑 하면 될 것 같다.
//	충돌하면? 여기서 처리 다 해?
//
//	충돋했을 때 처리들:
//	- 플레이어가 프롭과 충돌하면 이동 무효. (슬라이드는 나중에 구현하기)
//	- 플레이어가 플레이어와 충돌하면 이동 무효. (서로 밀치기는 나중에 구현하기)
//	- 플레이어가 프로젝타일과 충돌하면 프로젝타일의 효과를 플레이어에게 적용.
//	- 프로젝타일이 프롭과 충돌하면 프로젝타일 비활성화.
//
//	이동 무효를 하려면 플레이어의 트리거를 꺼버리면 되나?
//	근데 충돌체크는 이동 후에 검사해야 하는데?
//	그럼 LateUpdate()를 만들자! 충돌검사를 하고 충돌했으면 각 객체에게 트리거를 켜주고
//	LateUpdate()를 하게 하기.
//	누구랑 충돌했는지 그냥 전달해주기로 함.
//	*********************************************************************/
//	/*
//	for (auto player = m_Players.begin(); player != m_Players.end(); ++player) {
//		// 활성화된 애만 처리할 것.
//		if ((*player)->IsAlive()) {
//
//			for (auto prop = m_Props.begin(); prop != m_Props.end(); ++prop)
//				if (IsCollidable((*player), (*prop)))
//					if ((*player)->IsCollide((*prop)))
//						(*player)->AddCollideInfo((*prop));
//
//			for (auto otherPlayer = m_Players.begin(); otherPlayer != m_Players.end(); ++otherPlayer)
//				if (IsCollidable((*player), (*otherPlayer)))
//					if ((*player)->IsCollide((*otherPlayer))) {
//						(*player)->AddCollideInfo((*otherPlayer));
//						(*otherPlayer)->AddCollideInfo((*player));
//					}
//			for (auto projectile = m_Projectiles.begin(); projectile != m_Projectiles.end(); ++projectile)
//				if (IsCollidable((*player), (*projectile)))
//					if ((*player)->IsCollide((*projectile))) {
//						(*player)->AddCollideInfo((*projectile));
//						(*projectile)->Disable();
//					}
//		}
//	}
//	for (auto projectile = m_Projectiles.begin(); projectile != m_Projectiles.end(); ++projectile) {
//		// 활성화된 애만 처리할 것.
//		if ((*projectile)->IsAlive()) {
//			for (auto prop = m_Props.begin(); prop != m_Props.end(); ++prop)
//				if (IsCollidable((*projectile), (*prop)))
//					if ((*projectile)->IsCollide((*prop))) {
//						(*projectile)->Disable();
//						break;
//					}
//
//		}
//	}
//	*/
//
//
//}

Collider::Collider()
{
}

/*********************************************************************
2019-07-05
충돌체 자체의 쿼터니언은 정해져야 함.
자체의 회전각과 주인 객체의 회전각이 더해져야 하는데 이건 어떻게 만들거?
Quaternion 더하는 함수가 있나?
*********************************************************************/
Collider::Collider(XMFLOAT3 offset, XMFLOAT3 extents, XMFLOAT4 quaternion, bool trig)
	: m_xmf3Offset(offset)
	, m_Box(BoundingOrientedBox(XMFLOAT3(0, 0, 0), extents, XMFLOAT4(0, 0, 0, 0)))
	, m_Type(ColliderType::BOX)
	, m_xmf4OrigOrientaion(quaternion)
	, m_IsTrigger(trig)
{

}

Collider::Collider(XMFLOAT3 offset, float radius, bool trig)
	: m_xmf3Offset(offset)
	, m_Type(ColliderType::SPHERE)
	, m_Sphere(BoundingSphere(XMFLOAT3(0, 0, 0), radius))
	, m_IsTrigger(trig)
{

}

void Collider::Update(XMFLOAT3 position, XMFLOAT4 rotation)
{
	SetPosition(position, GetRotatedOffset(rotation));
	SetRotation(rotation);
}

bool Collider::IsCollide(const Collider & other)
{
	if (m_IsTrigger) return false;
	/*********************************************************************
	2019-07-05
	조기리턴문을 쓰고 싶다!

	if문 왼쪽부터 검사하고 아니면 패스할테니까 저렇게 해도 괜찮겠지???
	*********************************************************************/
	if (m_Type == ColliderType::BOX) {
		if (other.m_Type == ColliderType::BOX)  return m_Box.Intersects(other.m_Box);
		else									return m_Box.Intersects(other.m_Sphere);
	}
	else {
		if (other.m_Type == ColliderType::BOX)  return m_Sphere.Intersects(other.m_Box);
		else									return m_Sphere.Intersects(other.m_Sphere);
	}
	return false;
}

void Collider::SetOrientation(const XMFLOAT4 & orientation)
{
	if (m_Type == ColliderType::BOX)	m_Box.Orientation = orientation;
}

bool Collider::TriggerCheck(const Collider & other)
{
	if (m_Type == ColliderType::BOX) {
		if (other.m_Type == ColliderType::BOX)  return m_Box.Intersects(other.m_Box);
		else									return m_Box.Intersects(other.m_Sphere);
	}
	else {
		if (other.m_Type == ColliderType::BOX)  return m_Sphere.Intersects(other.m_Box);
		else									return m_Sphere.Intersects(other.m_Sphere);
	}
	return false;
}

XMFLOAT3 Collider::GetLook()
{
	XMFLOAT3 look(0, 0, 1);

	if (m_Type == ColliderType::BOX)	XMStoreFloat3(&look, XMVector3Rotate(XMLoadFloat3(&look), XMLoadFloat4(&m_Box.Orientation)));

	return look;
}

XMFLOAT3 Collider::GetUp()
{
	XMFLOAT3 up(0, 1, 0);

	if (m_Type == ColliderType::BOX)	XMStoreFloat3(&up, XMVector3Rotate(XMLoadFloat3(&up), XMLoadFloat4(&m_Box.Orientation)));

	return up;
}

XMFLOAT3 Collider::GetCenter()
{
	if (m_Type == ColliderType::BOX)	return m_Box.Center;
	else								return m_Sphere.Center;
}

XMFLOAT3 Collider::GetExtents()
{
	if (m_Type == ColliderType::BOX)	return m_Box.Extents;
	else return XMFLOAT3(0, 0, 0);
}

XMFLOAT4 Collider::GetOrientation()
{
	if (m_Type == ColliderType::BOX)	return m_Box.Orientation;
	else return XMFLOAT4(0, 0, 0, 1);
}

XMFLOAT3 GetBetweenVector(const Collider & A, const Collider & B)
{
	if (A.m_Type == ColliderType::BOX) {
		if (B.m_Type == ColliderType::BOX)  return Vector3::Subtract(B.m_Box.Center, A.m_Box.Center);
		else								return Vector3::Subtract(B.m_Sphere.Center, A.m_Box.Center);
	}
	else {
		if (B.m_Type == ColliderType::BOX)  return Vector3::Subtract(B.m_Box.Center, A.m_Sphere.Center);
		else								return Vector3::Subtract(B.m_Sphere.Center, A.m_Sphere.Center);
	}
	return XMFLOAT3(0, 0, 0);
}

XMFLOAT3 Collider::GetRotatedOffset(XMFLOAT4 rotation)
{
	XMFLOAT3 result;
	XMStoreFloat3(&result, XMVector3Rotate(XMLoadFloat3(&m_xmf3Offset), XMLoadFloat4(&rotation)));
	return result;
}

void Collider::SetPosition(XMFLOAT3 position, XMFLOAT3 rotatedOffset)
{
	if (m_Type == ColliderType::BOX)	m_Box.Center = Vector3::Add(position, rotatedOffset);
	else								m_Sphere.Center = Vector3::Add(position, rotatedOffset);

}

void Collider::SetRotation(XMFLOAT4 rotation)
{
	if (m_Type == ColliderType::BOX) {
		XMFLOAT4 objRotation = rotation;
		XMStoreFloat4(&m_Box.Orientation, XMQuaternionRotationMatrix(XMMatrixMultiply(XMMatrixRotationQuaternion(XMLoadFloat4(&objRotation)), XMMatrixRotationQuaternion(XMLoadFloat4(&m_xmf4OrigOrientaion)))));


		//m_Box.Orientation = rotation;
	}

}

CUI::CUI()
{
	m_xmf2Size = XMFLOAT2(0, 0);
	m_xmf2Scale = XMFLOAT2(1.0f, 1.0f);
	m_MouseState = MouseState::NONE;
	m_fAlpha = 1.0f;
}

void CUI::SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(g_RootParameterUI, m_d3dCbvGPUDescriptorHandle);

}

void CUI::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, bool isDebug)
{
	if (!m_IsAlive) return;
	if (!m_ModelList.empty()) {
		if (m_AnimationController) {
			ChangeAnimClip();
			SetAnimatedMatrix(m_AnimationController, m_AnimationTime);
		}
		UpdateConstantBuffer(pd3dCommandList);
		for (auto p = m_ModelList.begin(); p != m_ModelList.end(); ++p) {

			pd3dCommandList->SetGraphicsRootDescriptorTable(g_RootParameterUI, m_d3dCbvGPUDescriptorHandle);
			(*p)->Render(pd3dCommandList, isDebug);
		}
	}
}

void CUI::Initialize(XMFLOAT2 size)
{
	m_xmf2Size = size;
}

void CUI::SetScale(XMFLOAT2 scale)
{
	m_xmf2Scale = scale;
}

const XMFLOAT2 CUI::GetSize()
{
	return XMFLOAT2(m_xmf2Size.x * m_xmf2Scale.x, m_xmf2Size.y * m_xmf2Scale.y);
}

// Size는 Damege와 비례하게 넣고 내부에서 Scale로 크기 변경
void CDamageUI::Initialize(XMFLOAT2 size, int num)
{
	SetAlive(true);
	m_xmf2Size = size;
	m_fLifeTime = 1.0f;
}

void CDamageUI::Update(float fTimeElapsed)
{
	/*
	생성되고 위로 살짝 올라가다가 아래로 이동
	생성되고 살짝 커졌다가 작아져서 소멸
	0.2초에 정점, 1.0초에 소멸

	계속 플레이어 위치를 보게 해야 하는데 그건 어떻게 할 것인지
	이건 추후 업데이트에서 하도록 하자.
	*/
	//static float time[3] = { 0.0f, 0.2f, 1.0f };
	//static float scale[3] = { 0.5f, 1.0f, 0.0f };
	//float curTime = 1.0f - m_fLifeTime;
	//for (int i = 0; i < 2; i++) {
	//}

	m_fLifeTime -= fTimeElapsed;
	if (m_fLifeTime <= 0.0f) Disable();


}
