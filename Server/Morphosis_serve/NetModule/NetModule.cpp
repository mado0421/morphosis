#include "stdafx.h"
#include "NetModule.h"

NetModule::NetModule()
{
}


NetModule::~NetModule()
{
}



void NetModule::Init(HWND hWnd)
{
	
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr(ipAdress);

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;

	main_window_handle = hWnd;
}

int NetModule::Connect()
{
	int Result = WSAConnect(socket, reinterpret_cast<sockaddr*>(&ServerAddr),
		sizeof(ServerAddr), NULL, NULL, NULL, NULL);

	WSAAsyncSelect(socket, main_window_handle, WM_SOCKET, FD_CLOSE | FD_READ);

	return Result;
}

void NetModule::Process_WM_SOCKET(WPARAM wparam, LPARAM lparam)
{
	if (WSAGETSELECTERROR(lparam)) {
		closesocket((SOCKET)wparam);
	}
	switch (WSAGETSELECTEVENT(lparam)) {
	case FD_READ:
		ReadPacket((SOCKET)wparam);
		break;
	case FD_CLOSE:
		closesocket((SOCKET)wparam);
		break;
	}
}

void NetModule::ReadPacket(SOCKET socket)
{
	DWORD ioBtye, ioFlag = 0;

	int ret = WSARecv(socket, &recv_wsabuf, 1, &ioBtye, &ioFlag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
		std::cout << "Recv Error " << err_code<<"\n";
	}
	int r_size = ioBtye;
	char* ptr = recv_buffer;
	
	while (r_size != 0)
	{
		if (packet_size == 0)
		{
			packet_size = ptr[0];
		}
		int remain = packet_size - prev_size;
		if (r_size >= remain)
		{
			memcpy(packet_buffer + prev_size, ptr, remain);
			ProcessPacket(packet_buffer);

			r_size -= remain;
			ptr += remain;
			packet_size = 0;
			prev_size = 0;
		}
		else
		{
			memcpy(packet_buffer + prev_size, ptr, r_size);
			prev_size += r_size;
			r_size = 0;
		}
	}
}

void NetModule::ProcessPacket(char * packet)
{
	Packet *p = reinterpret_cast<Packet*>(packet);

	if (p->type == TT_ECHO)
	{
		TT_Packet_Echo * rp = reinterpret_cast<TT_Packet_Echo*>(packet);
		std::tm* now = std::localtime(&rp->time);
		cout << now->tm_mon + 1 << " / " << now->tm_mday << " / " << now->tm_hour << ":" << now->tm_min << endl;
	}
	if (p->type == SC_IDENTIFY)
	{
		SC_Identify_Packet * id = reinterpret_cast<SC_Identify_Packet*>(packet);
		key = id->key;
		cout << "my key = " << key << "\n";
	}
	if (p->type == SC_MOVE)
	{
		needFlag = true;
	}
}

void NetModule::SendPacket(void * packet,int size)
{
	DWORD iobyte;
	send_wsabuf.len = size;
	int ret = WSASend(socket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
	cout << "send size = " << size << "\n";
}

void NetModule::SendPacket()
{
	TT_Packet_Echo *packet = reinterpret_cast<TT_Packet_Echo*>(send_buffer);
	packet->size = sizeof(packet);
	send_wsabuf.len = sizeof(packet);
	DWORD iobyte;
	packet->type = TT_ECHO;
	packet->time = std::time(0);

	int ret = WSASend(socket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
	cout << "SendPacket size: " << iobyte << endl;

}

void NetModule::Destroy()
{
	closesocket(socket);
	WSACleanup();
}
