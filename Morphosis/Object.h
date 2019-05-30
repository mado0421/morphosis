#pragma once
#include "Model.h"
#include "Camera.h"
#include "AnimationData.h"


// 기본적인 오브젝트들 전용(크기 변동이 없는 기본적인 UI
struct CB_OBJECT_INFO {
	XMFLOAT4X4	m_xmf4x4World;
	UINT		m_nMaterialIndex;
};

struct CB_UI_INFO {
	XMFLOAT4X4	m_xmf4x4World;
	UINT		m_nMaterialIndex;
};

class CObject
{
public:
	XMFLOAT4X4						m_xmf4x4World;

	CModel							*model = NULL;

	CMesh							**m_ppMeshes;
	int								m_nMeshes;
	CMaterial						*m_pMaterial = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

	CB_OBJECT_INFO					*m_pcbMappedObject = NULL;
	ID3D12Resource					*m_pd3dcbObject = NULL;

	bool							isAlive = true;


public:
	CObject();
	~CObject();

public:
	virtual void Initialize();

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	//virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual void Update(float fTimeElapsed);

	void SetModel(CModel *model);
	void SetMaterial(CMaterial *pMaterial);
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3 xmf3Position);

	const XMFLOAT3 GetPosition();
	const XMFLOAT3 GetLook();
	const XMFLOAT3 GetUp();
	const XMFLOAT3 GetRight();

	void SetLook(XMFLOAT3 look);
	void SetUp(XMFLOAT3 up);
	void SetRight(XMFLOAT3 right);
};

class CCollideObejct : public CObject
{
public:
	CMesh							**m_ppTestMeshes;
	int								m_nTestMeshes;
	BoundingOrientedBox				m_collisionBox;

	BoundingOrientedBox GetOOBB() { return m_collisionBox; }
	void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation) {m_collisionBox = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation);}
	bool IsCollide(const BoundingOrientedBox& other) {return m_collisionBox.Intersects(other);}
	void SetOOBBMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList) {
		CTestMesh *pTestMesh = new CTestMesh(pd3dDevice, pd3dCommandList, m_collisionBox.Extents);

		if (m_ppTestMeshes)
		{
			m_ppTestMeshes[0] = pTestMesh;
			m_nTestMeshes = 1;
		}
		else
		{
			m_ppTestMeshes = new CMesh*;
			m_ppTestMeshes[0] = pTestMesh;
			m_nTestMeshes = 1;
		}
	}

	virtual void TestRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);

};

#define G (0.05 * 9.8)
class CMovingObject : public CCollideObejct
{
public:
	XMFLOAT3						m_xmf3Variation;
	XMFLOAT3						m_xmf3RotateAngle;
	float							m_fSpeed = 100.0f;

	float							m_fGravityAccel = 0;
	bool							isFalling = true;
	float							prevHeight = 0;
public:

	void MoveOOBB(float fTimeElapsed) {
		m_collisionBox.Center.x += m_xmf3Variation.x * fTimeElapsed * m_fSpeed;
		m_collisionBox.Center.y += m_xmf3Variation.y * fTimeElapsed * m_fSpeed;
		m_collisionBox.Center.z += m_xmf3Variation.z * fTimeElapsed * m_fSpeed;
	}
	void FallingOOBB(float fTimeElapsed) {
		prevHeight = m_collisionBox.Center.y;
		m_fGravityAccel += fTimeElapsed * static_cast<float>(G) * 1.5f;
		m_collisionBox.Center.y -= m_fGravityAccel;
	}

	virtual void Update(float fTimeElapsed);

	void AddPosVariation(XMFLOAT3 xmf3Velocity);
	void AddRotateAngle(XMFLOAT3 xmf3Angle);
};

#define TIMER_ATT 0.05
#define TIMER_RESPANW 5
class CPlayerObject : public CMovingObject
{
public:
	short m_hp = 100;
	float m_timer = 0;
	float m_attTimer = 0;
	bool m_team = 0;
	bool m_jump = false;

public:
	virtual void Initialize();
	virtual void Update(float fTimeElapsed);

	void Attack();
	void Damaged(int val);
	void Jump();

	void SetTeam(bool team) { m_team = team; }

	bool IsDead() { return m_hp <= 0; }
	bool IsFireable() { return (m_attTimer <= 0) && !IsDead(); }
};

class CAnimationPlayerObject : public CPlayerObject
{
public:
	AnimationData* anim;
public:
	void Animate(float time) {
		anim->GenerateToWorldMatrix(time);
	}
};


class Importer {
	vector<AnimationBone> bones;
	vector<AnimationKey> keys;
	vector<AnimationMesh> meshes;

public:
	void ImportFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture* pTexture, const char* fileName, CAnimationPlayerObject& obj) {

		std::ifstream in;

		std::string MeshFileName;
		MeshFileName += fileName;
		MeshFileName += ".dat";

		in.open(MeshFileName, std::ios::in | std::ios::binary);
		/***************************************************************
		bone 정보 쓰고 mesh 정보 쓰고 key 정보 쓰고
		***************************************************************/
		/***************************************************************
		걍 몇 개 있는지랑 name이랑 부모boneIdx만 저장하자.
		부모 없으면 부모 boneIdx 값 -1임
		***************************************************************/
		int nBones;
		in.read((char*)&nBones, sizeof(int));
		bones.resize(nBones);
		for (int i = 0; i < nBones; ++i) {
			in.read((char*)&(bones[i].m_parentIdx), sizeof(int));
			in.read((char*)&(bones[i].m_translation), sizeof(XMFLOAT3));
			in.read((char*)&(bones[i].m_rotation), sizeof(XMFLOAT3));

			swap(bones[i].m_translation.y, bones[i].m_translation.z);
			swap(bones[i].m_rotation.y, bones[i].m_rotation.z);

		}

		/***************************************************************
		mesh별로 ControlPoint 다 써야됨.
		float3, int4, float4
		그거하고 polygonVertexIndex도 쓰기
		***************************************************************/
		int nMeshes;
		in.read((char*)&nMeshes, sizeof(int));
		meshes.resize(nMeshes);
		for (int i = 0; i < nMeshes; ++i) {
			int nCPs;
			in.read((char*)&nCPs, sizeof(int));
			meshes[i].controlPoints.resize(nCPs);
			for (int j = 0; j < nCPs; ++j) {
				in.read((char*)&(meshes[i].controlPoints[j].pos), sizeof(XMFLOAT3));
				in.read((char*)&(meshes[i].controlPoints[j].boneIdx), sizeof(XMINT4));
				in.read((char*)&(meshes[i].controlPoints[j].weight), sizeof(XMFLOAT4));

				swap(meshes[i].controlPoints[j].pos.y, meshes[i].controlPoints[j].pos.z);
			}
		}
		for (int i = 0; i < nMeshes; ++i) {
			int nPVI;
			in.read((char*)&nPVI, sizeof(int));
			meshes[i].polygonVertexIndex.resize(nPVI);
			for (int j = 0; j < nPVI; ++j) {
				in.read((char*)&(meshes[i].polygonVertexIndex[j]), sizeof(int));
			}
		}

		CModel *model = new CModel();
		for (int i = 0; i < nMeshes; ++i) {
			CAnimMesh* tempMesh = new CAnimMesh(pd3dDevice, pd3dCommandList, meshes[i]);
			model->AddMesh(tempMesh);
		}
		model->SetTexture(pTexture);
		obj.SetModel(model);



		/***************************************************************
		키가 몇 개 있고, 각 키마다 Bone이 몇 개 있을 것인지 저장해야 함.
		일단은 모든 Bone이 xyz 요소값을 가지는 것으로 간주하고 작성하자.
		***************************************************************/
		int nKeys;
		in.read((char*)&nKeys, sizeof(int));


		keys.resize(nKeys);

		for (int i = 0; i < nKeys; ++i) {
			int nBones;/* = keys[i].data.size();*/
			in.read((char*)&nBones, sizeof(int));
			//keys[i].m_pBones.resize(nBones);
			keys[i].m_pBones.resize(nBones); // 쓸 일은 없지만 후에 resize() 함수 호출 시에 직관성을 위해
			keys[i].m_boneIdx.resize(nBones);
			keys[i].m_translations.resize(nBones);
			keys[i].m_rotations.resize(nBones);
		}

		for (int i = 0; i < nKeys; ++i) {
			float keyTime;
			int boneIdx;
			XMFLOAT3 tmpFloat3;
			in.read((char*)&keyTime, sizeof(float));

			keys[i].m_keytime = keyTime;

			for (int j = 0; j < keys[i].m_boneIdx.size(); ++j) {
				in.read((char*)&boneIdx, sizeof(int));
				keys[i].m_boneIdx[j] = boneIdx;
				in.read((char*)&tmpFloat3, sizeof(XMFLOAT3));
				keys[i].m_translations[j].x = tmpFloat3.x;
				keys[i].m_translations[j].y = tmpFloat3.y;
				keys[i].m_translations[j].z = tmpFloat3.z;
				in.read((char*)&tmpFloat3, sizeof(XMFLOAT3));
				keys[i].m_rotations[j].x = tmpFloat3.x;
				keys[i].m_rotations[j].y = tmpFloat3.y;
				keys[i].m_rotations[j].z = tmpFloat3.z;
				swap(keys[i].m_translations[j].y, keys[i].m_translations[j].z);
				swap(keys[i].m_rotations[j].y, keys[i].m_rotations[j].z);
			}
		}

		sort(keys.begin(), keys.end(), &AnimationKey::CompareByKeyTime);

		in.close();

		std::string AnimFileName;
		AnimFileName += fileName;
		AnimFileName += "_anim.dat";

		AnimData tmp;
		tmp.ImportFile(AnimFileName.c_str());




		obj.anim = new AnimationData();
		//obj.anim->Init(bones, keys);
		obj.anim->Init(tmp);

	}
};






































class CProjectileObject : public CMovingObject
{
public:
	bool m_team = 0;
	float m_fLifeTime = 1.0f;

public:
	virtual void Initialize();
	virtual void Initialize(CMovingObject *user);
	virtual void Update(float fTimeElapsed);

	void SetTeam(bool team) { m_team = team; }
};

class CDefaultUI : public CObject
{
public:
	CDefaultUI();
	~CDefaultUI();

public:
	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void SetExtents(XMFLOAT2 extents);
	virtual void SetExtents(float x, float y);

	virtual void Initialize();
	virtual void Initialize(CObject& other);
	virtual void Update(float fTimeElapsed);
	//위치, 크기(Extents), 텍스쳐 인덱스
public:
	XMFLOAT2 extents;
	unsigned int texIdx;
	float lifeTime = 3;
	float curLife = 0;
	bool isAlive = false;

};

class CFlexibleUI : public CDefaultUI
{

};

//======================================================================================================//
//======================================================================================================//
//======================================================================================================//

class Object {
public:	// 함수 파트
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void TestRender(ID3D12GraphicsCommandList *pd3dCommandList) {}
	virtual void Update(float fTimeElapsed);

	const XMFLOAT3 GetPosition() {
		return XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	}
	const XMFLOAT3 GetLook() {
		XMFLOAT3 vector = { m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33 };
		return Vector3::Normalize(vector);
	}
	const XMFLOAT3 GetUp() {
		XMFLOAT3 vector = { m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23 };
		return Vector3::Normalize(vector);
	}
	const XMFLOAT3 GetRight() {
		XMFLOAT3 vector = { m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13 };
		return Vector3::Normalize(vector);
	}

	virtual void Initialize(XMFLOAT3 pos, XMFLOAT3 dir, UINT64 nCbvGPUDescriptorHandlePtr, void * pAddress = nullptr) {
		SetPosition(pos);
		SetDirection(dir);
		SetMappedAddress(pAddress);
		SetCbvGPUDescriptorHandlePtr(nCbvGPUDescriptorHandlePtr);
	}
	void AddMesh(CMesh *pMesh, CTexture *pTexture) {
		if (!m_pModel) m_pModel = new Model();
		m_pModel->AddTexMesh(pMesh, pTexture);
	}

private: // 내부에서만 쓸 함수들
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	void SetPosition(XMFLOAT3 pos) {
		m_xmf4x4World._41 = pos.x;
		m_xmf4x4World._42 = pos.y;
		m_xmf4x4World._43 = pos.z;
	}
	void SetDirection(XMFLOAT3 direction) {
		XMVECTOR normDir = XMVector3Normalize(XMLoadFloat3(&direction));
		XMVECTOR upVector, rightVector, rotateQuat;

		rotateQuat = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(90), 0, 0);
		upVector = XMVector3Rotate(normDir, rotateQuat);

		rotateQuat = XMQuaternionRotationRollPitchYaw(0, XMConvertToRadians(-90), 0);
		rightVector = XMVector3Rotate(normDir, rotateQuat);

		XMFLOAT3 temp;
		XMStoreFloat3(&temp, normDir); SetLook(temp);
		XMStoreFloat3(&temp, upVector); SetUp(temp);
		XMStoreFloat3(&temp, rightVector); SetRight(temp);
	}
	void SetMappedAddress(void * pAddress) {
		m_pcbMappedObjectAddress = (CB_OBJECT_INFO*)pAddress;
	}

	void SetLook(XMFLOAT3 look) {
		m_xmf4x4World._31 = look.x;
		m_xmf4x4World._32 = look.y;
		m_xmf4x4World._33 = look.z;
	}
	void SetUp(XMFLOAT3 up) {
		m_xmf4x4World._21 = up.x;
		m_xmf4x4World._22 = up.y;
		m_xmf4x4World._23 = up.z;
	}
	void SetRight(XMFLOAT3 right) {
		m_xmf4x4World._11 = right.x;
		m_xmf4x4World._12 = right.y;
		m_xmf4x4World._13 = right.z;
	}

public:	// 생성자 소멸자 파트
	Object();

protected:	// 변수 파트
	// for Position and Rotation
	XMFLOAT4X4					m_xmf4x4World;

	// for Render
	Model						*m_pModel;
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescriptorHandle;
	UINT						m_iRootParameterIdx;

	// for Animation
	float						m_fAnimTime;

protected:
	// 상속하지 않는 이 클래스만의 MappedAddress
	CB_OBJECT_INFO				*m_pcbMappedObjectAddress;
};

class CollideObject : public Object {
public:
	// 충돌 객체는 처음 생성될 때 OOBB를 만들어줘야 한다.
	void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation) { m_OOBB = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation); }
	bool IsCollide(const BoundingOrientedBox& other) { return m_OOBB.Intersects(other); }
	virtual void TestRender(ID3D12GraphicsCommandList *pd3dCommandList) {
		if (!m_pCollisionModel) return;

		XMStoreFloat4x4(&m_pcbMappedObjectAddress->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

		pd3dCommandList->SetGraphicsRootDescriptorTable(m_iRootParameterIdx, m_d3dCbvGPUDescriptorHandle);
		m_pCollisionModel->Render(pd3dCommandList);
	}
	virtual void Initialize(XMFLOAT3 pos, XMFLOAT3 dir, UINT64 nCbvGPUDescriptorHandlePtr, XMFLOAT3 extents, void * pAddress = nullptr) {
		Object::Initialize(pos, dir, nCbvGPUDescriptorHandlePtr, pAddress);
		SetOOBB(pos, extents, XMFLOAT4(0, 0, 0, 1));

		// 여기서 m_pCollisionModel 넣어줘야 됨~~~~
	}



public:
	CollideObject();

protected:
	// for Collision Check
	BoundingOrientedBox		m_OOBB;

private:
	Model					*m_pCollisionModel;
};

class MovingObejct : public CollideObject {
public:
	// 이동 객체는 움직일 방향과 이동 속도, 중력 영향 유무(가능하다면 계수까지)

	// ProcessInput()에서 회전각과 이동량을 정해주면 Update()에서 흐른 시간과 곱하여 실제 이동하기.
	// 회전하면 바뀌는 것: 방향 벡터?
	void Rotate(XMFLOAT3 angle) {
		m_xmf3RotationRad.x += XMConvertToRadians(angle.x);
		m_xmf3RotationRad.y += XMConvertToRadians(angle.y);
		m_xmf3RotationRad.z += XMConvertToRadians(angle.z);
	}
	void Translate(XMFLOAT3 translation) {
		m_xmf3TranslationValue.x += translation.x;
		m_xmf3TranslationValue.y += translation.y;
		m_xmf3TranslationValue.z += translation.z;
	}
	virtual void Update(float fTimeElapsed);

public:
	MovingObejct();

protected:
	// 필요한 것들: 방향 벡터, 현재 수평 기준 상하 회전각
	// 근데 방향 벡터는 look Vector 아닌가? 필요없을거 같음 땅땅땅.
	//XMFLOAT3 m_xmf3Direction;

	// 변동량
	XMFLOAT3 m_xmf3RotationRad;
	XMFLOAT3 m_xmf3TranslationValue;

	// 중력 영향 받나요?? 100% 받으면 1.0f, 안 받으면 0.0f
	float m_fGravityFactor;
};

//class AnimationObejct : public MovingObejct {
//public:
//public:
//protected:
//};

#define MAX_PLAYER 8
#define BULLET_PER_PL 1
#define PROJECTILE_PER_PL 1
#define SKILLJUDGE_PER_PL 1
#define MAX_PARTICLE 1
#define MAX_EFFECT 1

class ObjectManager {
public:
	UINT GetNumTotalObjects() {
		// levelData가 있다면 데이터를 읽어서 정확한 Object 개수를 구할 수 있다.
		// TerrainMeshes + Props + TerrainCollisionBoxes 등
		// 근데 그건 지금 없으니까 일단은 하나로 하자.
		return 1 + 0 + 1 +
			MAX_PLAYER * SKILLJUDGE_PER_PL +
			MAX_PLAYER +
			MAX_PLAYER * BULLET_PER_PL +
			MAX_PLAYER * PROJECTILE_PER_PL +
			MAX_PARTICLE +
			MAX_EFFECT;

		//return m_terrainMeshes.size() + m_props.size() + m_terrainCollisionBoxes.size() + m_skillJudgeObjects.size() +
		//	m_players.size() + m_bullets.size() + m_skillProjectiles.size() + m_particles.size() + m_effects.size();
	}
	//void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	void Update(float fTimeElapsed);

	// terrainMeshes, terrainCollisionBoxes, props, light, spawnPoint, targetArea, texture 등에서
	// terrainMeshes, terrainCollisionBoxes, props, texture를 담당.
	void GenerateTerrain();

	void Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE	d3dCbvCPUDescriptorStartHandle);

	void RenderTerrainMeshes(ID3D12GraphicsCommandList *pd3dCommandList) {
		for (auto p = m_terrainMeshes.begin(); p != m_terrainMeshes.end(); ++p) p->Render(pd3dCommandList);
	}
	void RenderCollideObjects(ID3D12GraphicsCommandList *pd3dCommandList) {
		for (auto p = m_props.begin(); p != m_props.end(); ++p) p->Render(pd3dCommandList);
		for (auto p = m_terrainCollisionBoxes.begin(); p != m_terrainCollisionBoxes.end(); ++p) p->Render(pd3dCommandList);
		for (auto p = m_skillJudgeObjects.begin(); p != m_skillJudgeObjects.end(); ++p) p->Render(pd3dCommandList);

	}
	void RenderMovingObjects(ID3D12GraphicsCommandList *pd3dCommandList) {
		for (auto p = m_players.begin(); p != m_players.end(); ++p) p->Render(pd3dCommandList);
		for (auto p = m_bullets.begin(); p != m_bullets.end(); ++p) p->Render(pd3dCommandList);
		for (auto p = m_skillProjectiles.begin(); p != m_skillProjectiles.end(); ++p) p->Render(pd3dCommandList);

	}
	void RenderParticles(ID3D12GraphicsCommandList *pd3dCommandList) {
		for (auto p = m_particles.begin(); p != m_particles.end(); ++p) p->Render(pd3dCommandList);

	}
	void RenderEffects(ID3D12GraphicsCommandList *pd3dCommandList) {
		for (auto p = m_effects.begin(); p != m_effects.end(); ++p) p->Render(pd3dCommandList);

	}

	Object* GetTarget(int idx) {
		return &(m_players.at(idx));
	}

	void AddMesh(CMesh *pMesh) {
		m_meshes.emplace_back(pMesh);
	}
	void AddTexture(CTexture *pTex) {
		m_textures.emplace_back(pTex);
	}
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }

private:
	void CreateConstantBufferView(ID3D12Device * pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorStartHandle);
	void CreateResource(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);
	void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);

public:

private:
	// Objects
	std::vector<Object>			m_terrainMeshes;

	std::vector<CollideObject>	m_props;
	std::vector<CollideObject>	m_terrainCollisionBoxes;
	std::vector<CollideObject>	m_skillJudgeObjects;

	std::vector<MovingObejct>	m_players;
	std::vector<MovingObejct>	m_bullets;
	std::vector<MovingObejct>	m_skillProjectiles;

	std::vector<MovingObejct>	m_particles;
	std::vector<Object>			m_effects;

	ID3D12Resource				*m_pd3dcbResource;
	CB_OBJECT_INFO				*m_pcbMappedObjects;
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescriptorHandle;

	std::vector<CMesh*>			m_meshes;
	std::vector<CTexture*>		m_textures;
};





//======================================================================================================//
//======================================================================================================//
//======================================================================================================//