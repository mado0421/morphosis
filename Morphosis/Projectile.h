#pragma once
#include "Object.h"

class CProjectile : public CObject {
public:
	CProjectile();
	~CProjectile();

public:
	void			Initialize(CObject* obj);
	void			Initialize(CObject* obj, const char* modelName, Effect* effect);
	virtual void	Update(float fTimeElapsed) override;
	virtual void	LateUpdate(float fTimeElapsed) override;
	void			Damage(CObject* obj);

private:
	bool			IsExpired() const { return m_fLifeTime <= 0; }

protected:
	XMFLOAT3		m_xmf3Direction;	//Normal Vector
	float			m_fSpeed;
	float			m_fLifeTime;
	float			m_BaseDamage;
};