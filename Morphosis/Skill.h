#pragma once

enum class SkillType {
	AROUND,
	STRAIGHT,
	RANGE1,
	POWER1,
	STUN,
	DOT,
	RANGE2,
	CLUSTER,

	count
};

#include "Object.h"

class CSkill
{
public:
	CSkill();
	~CSkill();

	void Active(CObject* object) {
		if (m_IsOpen[static_cast<int>(SkillType::AROUND)]) {
			GetTarget(object);
			AffectToTarget();
		}
		else if (m_IsOpen[static_cast<int>(SkillType::STRAIGHT)]) {
			MakeBullet();
		}
		else /*if (m_IsOpen[static_cast<int>(SkillType::µÑ ´Ù ¾Æ´Ô)])*/ {
			MakeBullet();
		}
	}
	void GetTarget(CObject* object) {
		if (m_IsOpen[static_cast<int>(SkillType::AROUND)]) {
			
		}
	}
	void AffectToTarget();

	void MakeBullet();
public:
	bool	m_IsOpen[ static_cast<int>( SkillType::count )];

};

