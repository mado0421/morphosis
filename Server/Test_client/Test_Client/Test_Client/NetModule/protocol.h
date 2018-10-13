#pragma once
#include"../stdafx.h"
#define SERVER_PORT				8000

#define MAX_BUFF_SIZE			4000
#define MAX_PACKET_SIZE			255

#define MAX_CLIENT				5000
#define MAX_PLAYER_PER_GAME		8

#define MAX_ROOM ( MAX_CLIENT /MAX_PLAYER_PER_GAME )


#define Client_To_Server
#define CS_MATCHING 1

#define CS_MOVE
#define CS_ATTACK
#define CS_SKILL0
#define CS_SKILL1
#define CS_SKILL2



#define Server_To_Client