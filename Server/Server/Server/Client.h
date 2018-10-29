#pragma once
#include"header.h"

class Client {
public:
	SOCKET socket;
	bool in_connected;

	// POSITION	4x3 matrix
	WTrans w_matrix;

	// OTHER


	// IO
	EXOver exover;
	int packet_size;
	int prev_size;
	char prev_packet[MAX_PACKET_SIZE];
	
public:
	void Init();
};