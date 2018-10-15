#pragma once

#define SERVER_PORT				8000

#define MAX_BUFF_SIZE			4000
#define MAX_PACKET_SIZE			255

#define MAX_CLIENT				5000
#define MAX_PLAYER_PER_GAME		8

#define MAX_ROOM ( MAX_CLIENT /MAX_PLAYER_PER_GAME )


#define Client_To_Server
#define CS_MatchingING 1

#define CS_MOVE
#define CS_ATTACK
#define CS_SKILL0
#define CS_SKILL1
#define CS_SKILL2

#define TT_ECHO	10


#define Server_To_Client