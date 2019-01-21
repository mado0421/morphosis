#pragma once

/*******************************************************************************

Skill 클래스의 사용 방식

투사체와 이펙트 오브젝트에서 Skill이 작용하는 요소는
update() 정도?

피격 했을 때 어떤 처리를 할 것인가?
(예를 들어 대상 캐릭터에게 직접 효과 적용을 할 것인지, 지면과 충돌처리를 해서 그 자리에 구름을 만든다던지)

그럼 작성해야 하는 내용은
얘가 어떻게 날아가는지(조준한 곳에 즉발인지, 직선으로 날아가는지, 포물선으로 날아가는지)
영향을 주는 범위는 어떻게 되는지(범위? 맞은 대상?

어떻게 가는지(광선을 쏴서 보고 있는 곳에 그대로 생성을 하거나, 보는 방향으로 직선으로 투사체를 쏘거나, 보는 방향으로 포물선을 그리는 투사체를 쏘거나, 빔을 쏘거나, 걍 내 위치에 쓰거나 등)
어떻게 시작 판정을 하는지(쓰는 순간 바로 시작하거나, 무언가와 충돌하거나(적, 지형, 아군 등), 일정 시간이 지나거나 등)
어떻게 대상을 정하는지(부딪힌 상대에게, 영역을 생성해서 그 안에 있는 상대에게 등) 
어떻게 영향을 주는지(바로 효과 적용, 버프디버프 큐에 추가 등)

*******************************************************************************/

class CSkill
{
public:
	virtual void Update() = 0;

	virtual void Moving() = 0;
	virtual void invoke() = 0;
	virtual void FindTarget() = 0;
	virtual void Affect() = 0;
public:
	CSkill();
	~CSkill();
private:
	DirectX::XMFLOAT4X4 m_transform;
	unsigned short		m_speed;
	unsigned short		m_effectRange;
	unsigned short		m_effectValue;
};

class CSkillGrenade : public CSkill {
public:
	virtual void Update() {}

	virtual void Moving() { /* LookVector로 m_speed * elapsedTime 만큼 이동 */ }
	virtual void invoke() { /* 일정 시간이 지나면 폭발이 맞는데 일단은 부딪히면 터지는걸로 하자. 여기엔 그럼 조건을 넣어주는게 맞는가? */ }
	virtual void FindTarget() { /* 원점 중심으로 일정 범위 내에 적이 닿으면 타겟으로 선정. 이걸 하려면 밖에서 접근 가능한 플레이어 타겟 리스트가 있어야 함. */ }
	virtual void Affect() { /* 타겟 리스트의 플레이어들에게, 일정량만큼 데미지를 */ }
public:
	CSkillGrenade() {}
	~CSkillGrenade() {}
};