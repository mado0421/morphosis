#pragma once
#include <WinSock2.h>
#include <windows.h> 
#include <windowsx.h>
#include"protocol.h"
#include"PacketList.h"
#pragma comment (lib, "ws2_32.lib")

#define	BUF_SIZE	1024
#define	WM_SOCKET	WM_USER + 1

class NetModule
{
public:
	SOCKET socket;
	WSABUF	send_wsabuf;
	char 	send_buffer[BUF_SIZE];
	WSABUF	recv_wsabuf;
	char	recv_buffer[BUF_SIZE];

public:
	NetModule();
	~NetModule();
	
	void Init();
	void Destroy();
};