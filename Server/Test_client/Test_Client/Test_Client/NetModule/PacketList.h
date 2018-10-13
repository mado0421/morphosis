#pragma once
#include"../stdafx.h"
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

class TT_Packet_test : public Packet
{
	char a;
	float b;
};



#pragma (pop)