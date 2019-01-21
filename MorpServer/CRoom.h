#pragma once

/****************************************************************

Room의 역할.

Room은 플레이어들을 같이 모아두고 게임을 진행하는 역할.

Room에서 해줘야 할 것들.
 - 플레이어 소켓을 받아서 갖고 있기.
 - 플레이어가 8명이 전부 들어오면 RoomState를 Matching에서 Play로 바꿔주기.
 - RoomState가 Matching에서 Play가 되면 GameThread를 돌려서 애들 Update 해주기.
 - 시간이 다 지나거나, 승패 결정이 되면 RoomState를 Matching으로 바꾸고 애들은 전부 내쫓기?
 - Matching -(all Player Accept)-> Play -(round End)-> Result(뒷정리) -(30sec)-> Matching

Room에서 갖고 있어야 할 정보.
 - 플레이어들의 소켓.
 - Room 시간. (매칭 시작 시간을 해도 되고 하여간 고유 시간)
 - 플레이어들의 정보, 투사체들의 정보, 이펙트들의 정보, 파티클 매니저들의 정보(미리 만들어두고 풀링? 하는 식으로)

Update()에서 이런건 해주세요!!
 - 버프 큐 돌리면서 시간 업데이트하기

AfterUpdate()에서 충돌체크랑 버프 큐 비우기랑 기타 등등을 하는게?

클라가 룸에 접속하면 우리 이 맵 쓸거니까 넌 그거 로드하고 있어라.
로드 다 하면 말해줘~~ 를 할 것인지 아니면 계속 물어보면서
너 지금 로드 다 했니? 혹시 나간건 아니지..? 하기

****************************************************************/

class CRoom
{
public:
	void Initialize();

	void PlayerArrive(SOCKET& socket);
	
	void Update(float elapsedTime);
	void AfterUpdate();

	bool isTimeOut() { return m_remainingTime <= 0; }
	bool isVictory() { return (m_victoryPercent[Team::Blue] >= 100 || m_victoryPercent[Team::Orange] >= 100); }
	bool isAllPlayerLeave() {
		for (int i = 0; i < MAX_PLAYER; ++i) if (NULL != m_playerList[i].m_socket) return false;
		return true;
	}

	bool isAllPlayerReady();
	bool isRoundEnd();
	bool isResultEnd();

	/******************************************************************
	ChangeRoomState()에서 하는 일.
	Matching -(all Player Accept)-> Play -(round End)-> Result(뒷정리) -(30sec)-> Matching 
	******************************************************************/
	void ChangeRoomState();

public:
	CRoom();
	~CRoom();

public:
	/* Room 관련 변수 */
	unsigned int							m_roomID;
	char									m_roomState;

	/* 시간관련 변수 */
	double									m_remainingTime;
	double									m_elapsedTime;
	std::chrono::system_clock::time_point	m_curTime;
	std::chrono::system_clock::time_point	m_prevTime;

	/* 승패 관련 변수 */
	/* 0: Team::Blue, 200: Team::Orange, 100: Default */
	float m_capturePercent;
	/* 0 ~ 100, 100: victory */
	float m_victoryPercent[Team::count];

	/* 버프 관련 변수 */
	std::queue<BuffInfo>					m_buffQueue;

	/* 플레이어 관련 변수 */
	CPlayer								m_playerList[MAX_PLAYER];
	char									m_numPlayer;

	/* 투사체 관련 변수 */
	ProjectileInfo							m_projectileList[MAX_PROJECTILE];

	/* 레벨 데이터 */
	LevelInfo								m_level;

};