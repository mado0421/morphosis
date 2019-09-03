#pragma once

//class CPlayer;
//
//class Effect {
//public:
//	virtual void Act(CPlayer* p) = 0;
//};
//
//class EDefaultDamage : public Effect {
//public:
//	virtual void Act(CPlayer* p);
//};
//
//class ESlow : public Effect {
//public:
//	virtual void Act(CPlayer* p);
//};
//
//class Component {
//public:
//	virtual void MainEffect(CPlayer* target, CastType type) = 0;
//	virtual void SubEffect(CPlayer* target, CastType type) = 0;
//
//	float m_Cooltime;
//	CastType m_Type;
//};
//
//class CSkill {
//public:
//	void SetMainComponent(Component* component) {
//		m_MainComponent = component;
//		m_Cooltime = m_MainComponent->m_Cooltime;
//		m_Type = m_MainComponent->m_Type;
//	}
//	void AddComponent(Component* component) {
//		if(m_SubComponents.size() < 2) m_SubComponents.push_back(component);
//	}
//
//	void Act(CPlayer* target) {
//		m_MainComponent->MainEffect(target, m_Type);
//		for (int i = 0; i < m_SubComponents.size(); ++i) 
//			m_SubComponents[i]->SubEffect(target, m_Type);
//		
//	}
//
//public:
//	Component			*m_MainComponent;
//	vector<Component*>	m_SubComponents;
//
//	float m_Cooltime;
//	CastType m_Type;
//};
//
//class Stun : Component {
//public:
//	virtual void MainEffect(CPlayer* target, CastType type);
//	virtual void SubEffect(CPlayer* target, CastType type);
//
//	float m_Cooltime = 5;
//	CastType m_Type = CastType::HIT;
//};
//
//class Heal : Component {
//public:
//	virtual void MainEffect(CPlayer* target, CastType type);
//	virtual void SubEffect(CPlayer* target, CastType type);
//
//	float m_Cooltime = 7;
//	CastType m_Type = CastType::SELFCAST;
//};
//
//class Grenade : Component {
//public:
//	virtual void MainEffect(CPlayer* target, CastType type);
//	virtual void SubEffect(CPlayer* target, CastType type);
//
//	float m_Cooltime = 5;
//	CastType m_Type = CastType::PROJECTILE;
//};
//
//class Push : Component {
//public:
//	virtual void MainEffect(CPlayer* target, CastType type);
//	virtual void SubEffect(CPlayer* target, CastType type);
//
//	float m_Cooltime = 5;
//	CastType m_Type = CastType::AROUND;
//};