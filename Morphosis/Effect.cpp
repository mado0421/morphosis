#include "stdafx.h"
#include "Effect.h"
#include "Object.h"

CSkill g_SkillData;


void EDefaultDamage::Act(CPlayer * p)
{
	p->TakeDamage(8);
}

void ESlow::Act(CPlayer * p)
{
	p->Slow();
}

//void Stun::MainEffect(CPlayer * target, CastType type)
//{
//	target->Stun();
//}
//
//void Stun::SubEffect(CPlayer * target, CastType type)
//{
//	if (CastType::SELFCAST == type) {
//		
//	}
//	else if (CastType::AROUND == type) {
//
//	}
//	else if (CastType::HIT == type) {
//
//	}
//	else if (CastType::PROJECTILE == type) {
//
//	}
//}
//
//void Heal::MainEffect(CPlayer * target, CastType type)
//{
//	target->Heal();
//}
//
//void Grenade::MainEffect(CPlayer * target, CastType type)
//{
//	target->
//}
//
//void Push::MainEffect(CPlayer * target, CastType type)
//{
//	target->Push();
//}

void CSkill::Act(vector<CPlayer*> vec)
{
	if (m_IsActivated[SkillType::AROUND]) {
		GetTarget(vec);
		AffectToTarget();
	}
	else {
		CreateBullet();
	}
}

void CSkill::GetTarget(vector<CPlayer*> vec)
{
	if (m_IsActivated[SkillType::AROUND]) {
		float range = 100;
		if (m_IsActivated[SkillType::RANGE1]) range += 100;
		if (m_IsActivated[SkillType::RANGE2]) range += 200;
		/*
		플레이어 0 주변의 적들을 찾으면 됨
		*/
		for (int i = 1; i < vec.size(); ++i) {
			if (GetDistance(vec[0]->GetPosition(), vec[i]->GetPosition()) < range) {
				m_vecTarget.push_back(vec[i]);
			}
		}
	}
	else {
		for (int i = 0; i < vec.size(); ++i) m_vecTarget.push_back(vec[i]);
	}
}

void CSkill::AffectToTarget()
{
	if (m_vecTarget.size() > 0) {

		for (int i = 0; i < m_vecTarget.size(); ++i) {
			m_vecTarget[i]->TakeDamage(50);
			if (m_IsActivated[SkillType::POWER1]) {
				m_vecTarget[i]->TakeDamage(25);
			}
			if (m_IsActivated[SkillType::STUN]) {
				m_vecTarget[i]->Stun();
			}
			if (m_IsActivated[SkillType::DOT]) {
				m_vecTarget[i]->DOT();
			}
		}
		m_vecTarget.clear();
	}
}

void CSkill::CreateBullet()
{
	Request temp;
	temp.playerIdx = 0;
	temp.type = RequestType::Skill0;
	g_queueRequest.push(temp);
}
