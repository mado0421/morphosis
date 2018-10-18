#pragma once
class Player
{
public:
	int x, y;
	int hp;

public:
	Player();
	~Player();

	void Init();
	void Destroy();

	void Update();
	void Render();

	void Attack();
	void Skill0();
	void Skill1();

};

