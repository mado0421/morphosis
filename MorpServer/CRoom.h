#pragma once

/****************************************************************

Room�� ����.

Room�� �÷��̾���� ���� ��Ƶΰ� ������ �����ϴ� ����.

Room���� ����� �� �͵�.
 - �÷��̾� ������ �޾Ƽ� ���� �ֱ�.
 - �÷��̾ 8���� ���� ������ RoomState�� Matching���� Play�� �ٲ��ֱ�.
 - RoomState�� Matching���� Play�� �Ǹ� GameThread�� ������ �ֵ� Update ���ֱ�.
 - �ð��� �� �����ų�, ���� ������ �Ǹ� RoomState�� Matching���� �ٲٰ� �ֵ��� ���� ���ѱ�?
 - Matching -(all Player Accept)-> Play -(round End)-> Result(������) -(30sec)-> Matching

Room���� ���� �־�� �� ����.
 - �÷��̾���� ����.
 - Room �ð�. (��Ī ���� �ð��� �ص� �ǰ� �Ͽ��� ���� �ð�)
 - �÷��̾���� ����, ����ü���� ����, ����Ʈ���� ����, ��ƼŬ �Ŵ������� ����(�̸� �����ΰ� Ǯ��? �ϴ� ������)

Update()���� �̷��� ���ּ���!!
 - ���� ť �����鼭 �ð� ������Ʈ�ϱ�

AfterUpdate()���� �浹üũ�� ���� ť ����� ��Ÿ ����� �ϴ°�?

Ŭ�� �뿡 �����ϸ� �츮 �� �� ���Ŵϱ� �� �װ� �ε��ϰ� �־��.
�ε� �� �ϸ� ������~~ �� �� ������ �ƴϸ� ��� ����鼭
�� ���� �ε� �� �ߴ�? Ȥ�� ������ �ƴ���..? �ϱ�

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
	ChangeRoomState()���� �ϴ� ��.
	Matching -(all Player Accept)-> Play -(round End)-> Result(������) -(30sec)-> Matching 
	******************************************************************/
	void ChangeRoomState();

public:
	CRoom();
	~CRoom();

public:
	/* Room ���� ���� */
	unsigned int							m_roomID;
	char									m_roomState;

	/* �ð����� ���� */
	double									m_remainingTime;
	double									m_elapsedTime;
	std::chrono::system_clock::time_point	m_curTime;
	std::chrono::system_clock::time_point	m_prevTime;

	/* ���� ���� ���� */
	/* 0: Team::Blue, 200: Team::Orange, 100: Default */
	float m_capturePercent;
	/* 0 ~ 100, 100: victory */
	float m_victoryPercent[Team::count];

	/* ���� ���� ���� */
	std::queue<BuffInfo>					m_buffQueue;

	/* �÷��̾� ���� ���� */
	CPlayer								m_playerList[MAX_PLAYER];
	char									m_numPlayer;

	/* ����ü ���� ���� */
	ProjectileInfo							m_projectileList[MAX_PROJECTILE];

	/* ���� ������ */
	LevelInfo								m_level;

};