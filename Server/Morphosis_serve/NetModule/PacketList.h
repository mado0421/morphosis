#pragma once
#include<ctime>
#pragma pack (push,1)

class Packet
{
public:
	BYTE size;
	BYTE type;
};


class CS_R_Packet_Matching : public Packet
{

};

class SC_R_Packet_JoinPlayer : public Packet
{
	// PlayerInfo?
};



class SC_Packet_PutPlayer : public Packet
{
	int a;
	char b;
	int c;
};


class CS_Move_Packet :public Packet
{
public:
	float x, y, z;
};

class SC_Identify_Packet :public Packet
{
public:
	int key;
};

class SC_Move_Packet :public Packet
{
public:
	int key;
	float x, y, z;
};


class SC_Pos_Packet :public Packet
{
public:
	int client_key;
	float x, y, z;
};

class TT_Packet_test : public Packet
{
	char a;
	float b;
};


class TT_Packet_Echo :public Packet
{
public:
	std::time_t time;
};


#pragma pack(pop)