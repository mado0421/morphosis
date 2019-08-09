#include "stdafx.h"
#include "Effect.h"
#include "Player.h"


void EDefaultDamage::Act(CPlayer * p)
{
	p->TakeDamage(8);
}

void ESlow::Act(CPlayer * p)
{
	p->Slow();
}