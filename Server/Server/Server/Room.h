#pragma once
#include"header.h"

class Room
{
public:
	int cur_player_num;
	int player_indexes[MAX_PLAYER_PER_GAME];
	bool is_player_ready[MAX_PLAYER_PER_GAME];

	bool is_played;
	
public:

	Room();
	~Room();
	void Init();
	void Destroy();

	bool IsFull() { return (MAX_PLAYER_PER_GAME - cur_player_num) ? false : true; };
	void PlayerEnter(int clientKey);

};

class RoomManager
{
public:
	Room room[MAX_ROOM];


public:
	void Init();
	
	void Matching(int clientKey);
};