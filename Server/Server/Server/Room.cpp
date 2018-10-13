#include "Room.h"

Room::Room()
{
}

Room::~Room()
{
}

void Room::Init()
{
	is_played = false;

	for (int i = 0; i < MAX_PLAYER_PER_GAME; ++i)
	{
		player_indexes[i] = -1;
		is_player_ready[i] = false;
	}
	cur_player_num = 0;
}

void Room::Destroy()
{
	is_played = false;

	for (int i = 0; i < MAX_PLAYER_PER_GAME; ++i)
	{
		player_indexes[i] = -1;
		is_player_ready[i] = false;
	}
	cur_player_num = 0;
}

void Room::PlayerEnter(int clientKey)
{
	for (int i = 0; i < MAX_PLAYER_PER_GAME; ++i)
	{
		if (player_indexes[i] == -1)continue;
		player_indexes[i] = clientKey;
	}
}

void RoomManager::Init()
{
	for (int i = 0; i < MAX_ROOM; ++i)
		room[i].Init();
}

void RoomManager::Matching(int clientKey)
{
	for (int i = 0; i < MAX_ROOM; ++i)
	{
		if (room[i].is_played == true)continue;
		if (room[i].IsFull())continue;
		room[i].PlayerEnter(clientKey);
	}
}
