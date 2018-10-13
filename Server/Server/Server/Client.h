#pragma once
#include"header.h"

struct Client {
	SOCKET socket;
	bool in_connected;

	// POSITION
	
	// OTHER


	// IO
	EXOver exover;
	int packet_size;
	int prev_size;
	char prev_packet[MAX_PACKET_SIZE];
	
public:
	void Init();
};