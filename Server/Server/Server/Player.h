#pragma once
struct TransInfo
{
	union position {
		struct 
		{
			float x, y, z;
		};
		float p[3];
	};
};

class Player
{
public:
	Player();
	~Player();

public:
	int hp;
	int team;
	bool isJump;

	//	?
	float timer;
	float attTimer;
};

