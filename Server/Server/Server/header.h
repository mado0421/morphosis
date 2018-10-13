#pragma once
#include<iostream>
#include<WinSock2.h>
#include<windowsx.h>
#include<thread>
#include<vector>
#include"protocol.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;


struct EXOver {
	WSAOVERLAPPED wsaover;
	WSABUF wsabuf;
	char IOCPbuf[MAX_BUFF_SIZE];
	char event_type;
};


static const char EVT_RECV = 0;
static const char EVT_SEND = 1;

