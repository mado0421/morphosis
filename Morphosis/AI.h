#pragma once

class CPlayer;


struct AINode {
public:
	AINode() {
	}
	XMFLOAT3 m_xmf3Position = XMFLOAT3(0,0,0);
	AINode* next			= NULL;
};

inline float GetDistance(XMFLOAT3 a, XMFLOAT3 b) {
	return Vector3::Length(Vector3::Subtract(b, a));
}

/*
	MOVE: 
		���� ����� ��� ��ġ�� ����. 
		�����ϸ� �� ����� ���� ��ġ�� ��. 
		���� ��ġ�� ���� ���, �� ������ DEFEND�� �ٲ�.
		��ó�� ���� ���� ���, ATTACK���� �ٲ�.
	DEFEND: 
		�� �ڸ����� ������ ����.
		��ó�� ���� ���� ���, ATTACK���� �ٲ�.
	ATTACK: 
		���� �������� ����. 
		���� �� ��ó���� �������� �ٽ� MOVE��.
*/

class CAIBehavior {
public:
	virtual void Update(CPlayer*) = 0;
};

class CMoveBehavior : public CAIBehavior {
public:
	CMoveBehavior() {
	}
	virtual void Update(CPlayer* obj) override;

private:
	enum class AIMoveState {
		GOTOCLOSESTNODE,
		GOTONEXTNODE
	};

	AIMoveState m_state			= AIMoveState::GOTOCLOSESTNODE;
	AINode*		m_targetNode	= NULL;
};

class CDefendBehavior : public CAIBehavior {
public:
	CDefendBehavior() {
	}
	virtual void Update(CPlayer* obj) override;
};

class CAttackhavior : public CAIBehavior {
public:
	virtual void Update(CPlayer* obj) override;
};


class CTinMan
{
public:
	CTinMan(CAIBehavior* behavior, CPlayer* m_pObject)
		: m_curBehavior(behavior)
		, m_pObject(m_pObject)
	{

	}
	~CTinMan() {
		if (m_curBehavior) delete m_curBehavior;
	}

public:
	void PowerOn() { m_Power = true; }
	void PowerOff() { m_Power = false; }
	void Update()								{ if(m_Power) m_curBehavior->Update(m_pObject); }
	void ChangeBehavior(CAIBehavior* behavior)	{ m_curBehavior = behavior; }


private:
	CAIBehavior*	m_curBehavior;
	CPlayer*		m_pObject;
	bool			m_Power = false;
};

