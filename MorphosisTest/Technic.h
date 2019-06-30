#pragma once
#include "Skill.h"


#define MAXSKILLNUM 3
class CTechnic
{
public:
	void AddSkill(CSkill *skill);
	void DeleteSkill(int idx);
	void DeleteAllSkill();
	void Activate();
	void ResetCurCooldownTime();

public:
	CTechnic();
	~CTechnic();

private:
	CSkill *mainSkill = NULL;
	CSkill *subSkill[MAXSKILLNUM];
	float maxCooldownTime = 0;
	float curCooldownTime = 0;
};

