// TestClient.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"


#define SERVERIP	"127.0.0.1"
#define	SERVERPORT	9000
#define BUFSIZE		512

//std::queue<char>	g_inputToSendQueue;
//PlayerInfo			g_playerInfo[4];
//float				g_time;
TestFramework g_framework;

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
	//PlayerInfo g_playerInfo;
	//g_playerInfo.socket = sock;
	//test = 1;	send(g_playerInfo.socket, (char*)&test, sizeof(char), 0);
	//test = 2;	send(g_playerInfo.socket, (char*)&test, sizeof(char), 0);
	//test = 3;	send(g_playerInfo.socket, (char*)&test, sizeof(char), 0);
	//recvn(g_playerInfo.socket, (char*)&test, sizeof(char), 0); std::cout << (int)test << "\n";
	//recvn(g_playerInfo.socket, (char*)&test, sizeof(char), 0); std::cout << (int)test << "\n";
	//recvn(g_playerInfo.socket, (char*)&test, sizeof(char), 0);
	//if (GameMsg::Check == test) std::cout << "Check\n";
	//else std::cout << "?\n";
	//test = GameMsg::OK;
	//send(g_playerInfo.socket, (char*)&test, sizeof(char), 0);

	/***********************************************************
	동시접속 테스트 용
	***********************************************************/
	//PlayerInfo	tempPIf;
	//char		msg;
	//tempPIf.socket = sock;
	//tempPIf.modelType		= rand() % 8;
	//tempPIf.techniqueSet	= rand() % 8;
	//tempPIf.weapon			= rand() % 8;
	//SendPlayerInfo(tempPIf);
	//recvn(tempPIf.socket, (char*)&msg, sizeof(char), 0);
	//tempPIf.playerIdx = msg;
	//
	//g_playerInfo[tempPIf.playerIdx] = tempPIf;
	//for (int i = 0; i < tempPIf.playerIdx; ++i) RecvPlayerInfo(tempPIf.socket, g_playerInfo);
	//for (int i = 0; i < 4 - tempPIf.playerIdx; ++i) {
	//	RecvPlayerInfo(tempPIf.socket, g_playerInfo);
	//}
	//std::vector<std::thread> m_worker;
	//m_worker.push_back(std::thread(RecvThread));

	/***********************************************************
	테스트 클라이언트 내용 주고받기 테스트용
	***********************************************************/
	g_framework.Initialize(sock);
	g_framework.Frame();

	//close_socket()
	closesocket(sock);

	//윈속 종료
	WSACleanup();
	return 0;
}
