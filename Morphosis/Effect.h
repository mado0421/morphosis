#pragma once

class CPlayer;

class Effect {
public:
	virtual void Act(CPlayer* p) = 0;
};

class EDefaultDamage : public Effect {
public:
	EDefaultDamage() {
		cout << "EDefaultDamage ���� - " << sizeof(EDefaultDamage) << "\n";
	}
public:
	virtual void Act(CPlayer* p);
};

class ESlow : public Effect {
public:
	ESlow() {
		cout << "ESlow ���� - " << sizeof(ESlow) << "\n";
	}
public:
	virtual void Act(CPlayer* p);
};