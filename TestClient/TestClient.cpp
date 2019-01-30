// TestClient.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"


#define SERVERIP	"121.170.59.114"
#define	SERVERPORT	9000
#define BUFSIZE		512

int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

// 소켓 함수 오류 출력 후 종료
void err_quit(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int main(int argc, char* argv[])
{
	int retval;

	//윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	//socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)err_quit("socket()");

	//connect()

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(SOCKADDR_IN));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)err_quit("connect()");

	/***********************************************************
	연결 테스트 용
	***********************************************************/
	//데이터 통신에 사용할 변수
	//int test = 0;
	////서버와 데이터 통신
	//while (1) {
	//	retval = send(sock, (char*)&test, sizeof(int), 0);
	//	if (retval == SOCKET_ERROR) {
	//		err_display("send()");
	//		break;
	//	}
	//	//데이터 받기
	//	retval = recvn(sock, (char*)&test, sizeof(int), 0);
	//	if (retval == SOCKET_ERROR) {
	//		err_display("recv()");
	//		break;
	//	}
	//	else if (retval == 0)
	//		break;
	//	std::cout << test << "\n";
	//	if (test > 100) break;
	//}

	/***********************************************************
	메세지 테스트 용
	***********************************************************/
	//char test;
	//PlayerInfo playerInfo;
	//playerInfo.socket = sock;
	//test = 1;	send(playerInfo.socket, (char*)&test, sizeof(char), 0);
	//test = 2;	send(playerInfo.socket, (char*)&test, sizeof(char), 0);
	//test = 3;	send(playerInfo.socket, (char*)&test, sizeof(char), 0);
	//recvn(playerInfo.socket, (char*)&test, sizeof(char), 0); std::cout << (int)test << "\n";
	//recvn(playerInfo.socket, (char*)&test, sizeof(char), 0); std::cout << (int)test << "\n";
	//recvn(playerInfo.socket, (char*)&test, sizeof(char), 0);
	//if (GameMsg::Check == test) std::cout << "Check\n";
	//else std::cout << "?\n";
	//test = GameMsg::OK;
	//send(playerInfo.socket, (char*)&test, sizeof(char), 0);

	/***********************************************************
	동시접속 테스트 용
	***********************************************************/
	PlayerInfo	tempPIf;
	PlayerInfo	playerInfo[4];
	char		msg;
	tempPIf.modelType		= rand() % 8;
	tempPIf.techniqueSet	= rand() % 8;
	tempPIf.weapon			= rand() % 8;
	SendPlayerInfo(tempPIf);
	recvn(tempPIf.socket, (char*)&msg, sizeof(char), 0);
	tempPIf.playerIdx = msg;
	
	playerInfo[tempPIf.playerIdx] = tempPIf;
	for (int i = 0; i < tempPIf.playerIdx; ++i) RecvPlayerInfo(tempPIf.socket, playerInfo);

	for (int i = 0; i < 4 - tempPIf.playerIdx; ++i) {
		RecvPlayerInfo(tempPIf.socket, playerInfo);
	}



	//close_socket()
	closesocket(sock);

	//윈속 종료
	WSACleanup();
	return 0;
}
