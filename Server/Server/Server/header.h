#pragma once
#include<iostream>
#include<WinSock2.h>
#include<windowsx.h>
#include<thread>
#include<vector>
#include<ctime>
#include"protocol.h"

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;


struct EXOver {
	WSAOVERLAPPED wsaover;
	WSABUF wsabuf;
	char IOCPbuf[MAX_BUFF_SIZE];
	char event_type;
};


static const char EVT_RECV = 0;
static const char EVT_SEND = 1;

#pragma pack(push ,1)
class WTrans
{
public:
	float rx, ry, rz;
	float ux, uy, uz;
	float lx, ly, lz;
	float px, py, pz;

public:

	void Identity() {
		rx = 1, ry = 0, rz = 0;
		ux = 0, uy = 1, uz = 0;
		lx = 0, ly = 0, lz = 1;
		px = 0, py = 0, pz = 0;
	}
	void SetRight(float x, float y, float z) { rx = x; ry = y; rz = z; }
	void SetUp(float x, float y, float z) { ux = x; uy = y; uz = z; }
	void SetLook(float x, float y, float z) { lx = x; ly = y; lz = z; }
	void SetPosition(float x, float y, float z) { lx = x; ly = y; lz = z; }
};
#pragma pack(pop)

