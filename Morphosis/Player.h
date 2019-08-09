#pragma once
#include "Object.h"

class CTinMan;

class CPlayer : public CObject {
public:
	CPlayer();
	~CPlayer();

public:
	virtual void CreateConstantBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);

	// 기본적인 작동함수
	virtual void	Update(float fTimeElapsed) override;
	virtual void	LateUpdate(float fTimeElapsed) override;
	virtual void	ProcessInput(UCHAR* pKeysBuffer, float mouse) override;

	const int GetHP() { return m_HealthPoint; }

	// 외부 설정 함수
	virtual void	Enable()override;
	virtual void	Disable()override;
	void			SetSpawnPoint(const XMFLOAT3& pos) { m_xmf3SpawnPoint = pos; }
	void			SetLookAt(const XMFLOAT3& point) {
		SetLook(Vector3::Normalize(Vector3::Subtract(point, GetPosition())));
		SetUp(XMFLOAT3(0, 1, 0));
		SetRight(Vector3::Normalize(Vector3::CrossProduct(GetUp(), GetLook())));
	}
	// 외부 작동 함수
	void			MoveForwardTrigOn() { m_trigInput[static_cast<int>(Move::W)] = true; }
	void			Shoot();
	void			Skill(int idx = 0);
	void			TakeDamage(int val) { m_HealthPoint -= val; if (m_HealthPoint < 0) m_HealthPoint = 0; }
	void			Slow();
	// 조건 함수
	bool			IsShootable();
	bool			IsSkillUseable(int idx = 0);
	bool			IsMoving() {
		for (int i = 0; i < static_cast<int>(Move::count); ++i)
			if (m_trigInput[i]) return true;
		return false;
	}
	bool			IsJump();
	bool			IsOnAir();
	bool			IsShot();
	bool			IsDied();


protected:
	// 내부 설정 함수
	void			TriggerOff();
	XMFLOAT3		Move(float fTimeElapsed);
	float			Rotate(float fTimeElapsed);
	void			ChangeAnimClip();

public:
	CTinMan			*m_AIBrain = NULL;

protected:
	// 이동
	XMFLOAT3		m_xmf3Move;
	float			m_fSpeed;
	float			m_fSlowFactor = 1.0f;
	float			m_fRemainingTimeOfSlow = 0;
	// 키입력
	bool			m_trigInput[static_cast<int>(Move::count)];
	float			m_rotationInput;
	// 내부 객체 정보
	float			m_fRPM;	// 1/Round Per Minute
	float			m_fRemainingTimeOfFire;
	XMFLOAT3		m_xmf3SpawnPoint;
	int				m_HealthPoint;
	float			m_fRemainingTimeOfRespawn;
	float			m_fRemainingTimeOfSkill1;
	bool			m_IsOnGround = false;
};