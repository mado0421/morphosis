#pragma once

class CPlayer;

class Effect {
public:
	virtual void Act(CPlayer* p) = 0;
};

class EDefaultDamage : public Effect {
public:
	virtual void Act(CPlayer* p);
};

class ESlow : public Effect {
public:
	virtual void Act(CPlayer* p);
};

