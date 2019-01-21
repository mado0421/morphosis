#include "pch.h"
#include "CRoom.h"


void CRoom::Initialize()
{
	m_remainingTime			= DEFAULT_ROUND_TIME;
	m_elapsedTime			= 0;
	m_prevTime				= std::chrono::system_clock::now();

	/* Player Initialize */
	for (int i = 0; i < MAX_PLAYER; ++i) m_playerList->m_socket = NULL;
}

void CRoom::PlayerArrive(SOCKET & socket)
{
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (NULL == m_playerList[i].m_socket) {
			m_playerList[i].m_socket = socket;
			m_numPlayer++;
			break;
		}
	}
}

void CRoom::Update(float elapsedTime)
{

}

void CRoom::AfterUpdate()
{
}

bool CRoom::isAllPlayerReady()
{
	char msg[SIZE_MSG];
	int retval;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (NULL != m_playerList[i].m_socket) {
			msg[0] = Msg::ConnectionCheck;
			retval = send(m_playerList[i].m_socket, msg, SIZE_MSG, 0);
			if (SOCKET_ERROR == retval) return false;
			retval = recvn(m_playerList[i].m_socket, msg, SIZE_MSG, 0);
			if (SOCKET_ERROR == retval) return false;
		}
		else return false;
	}
	return true;
}

bool CRoom::isRoundEnd()
{
	/**********************************************
	
	어떤 상황에서 끝낼 것인가?

	 - 플레이어가 아무도 남지 않은 상황.
	 - 한 팀이 m_victoryPercent를 100까지 달성한 상황.
	 - m_remainingRoundTime이 0 아래로 내려간 상황.

	**********************************************/
	if (isTimeOut()) return true;
	if (isVictory()) return true;
	if (isAllPlayerLeave()) return true;
	return false;
}

bool CRoom::isResultEnd()
{
	if (isTimeOut()) return true;
	return false;
}

void CRoom::ChangeRoomState()
{
}

CRoom::CRoom()
{
}


CRoom::~CRoom()
{
}
