#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <WinSock2.h>
#include <windows.h> 
#include <windowsx.h>
#include<iostream>
#include"protocol.h"
#include"PacketList.h"
#pragma comment (lib, "ws2_32.lib")

#pragma warning(disable:4996)

#define	BUF_SIZE	1024
#define	WM_SOCKET	WM_USER + 1
class NetModule
{
public:
	SOCKET socket;
	char ipAdress[256] = "127.0.0.1";
	SOCKADDR_IN ServerAddr;
	HWND main_window_handle;

	WSABUF	send_wsabuf;
	char 	send_buffer[BUF_SIZE];

	WSABUF	recv_wsabuf;
	char	recv_buffer[BUF_SIZE];

	char	packet_buffer[BUF_SIZE];
	int		packet_size = 0;
	int		prev_size = 0;

public:
	NetModule();
	~NetModule();
	
	void Init(HWND hWnd);
	void Destroy();

	int Connect();
	void Process_WM_SOCKET(WPARAM wparam, LPARAM lparam);
	void ReadPacket(SOCKET socket);
	void ProcessPacket(char* packet);

	//	tmp
	void SendPacket();
};