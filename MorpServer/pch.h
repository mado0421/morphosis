#ifndef PCH_H
#define PCH_H

// TODO: 여기에 미리 컴파일하려는 헤더 추가
#include <WinSock2.h>
#include <iostream>
#include <chrono>
#include <DirectXMath.h>

#include <queue>

#include "CSkill.h"
#endif //PCH_H

// Defines
#define TEST
#ifdef TEST
#define MAX_PLAYER					1
#else
#define MAX_PLAYER					8
#endif
#define MAX_PROJECTILE_PER_PLAYER	32
#define MAX_PROJECTILE				(MAX_PLAYER * MAX_PROJECTILE_PER_PLAYER)
#define MAX_SKILL					4
#define MAX_TECHNIQUE				4
#define SIZE_MSG					1
#define DEFAULT_ROUND_TIME			(60*5)
#define DEFAULT_RESULT_TIME			(60*0.5)

namespace Msg {
	enum {
		ConnectionCheck = 0,
		StartRount,
		PlayerLeave,
		RoundEnd,
		ResultEnd
	};
}

namespace Team {
	enum {
		Blue = 0,
		Orange,

		count
	};
}

inline int recvn(SOCKET s, char *buf, int len, int flags)
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
inline void err_quit(const char *msg)
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
inline void err_display(const char *msg)
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

/***********************************************************************

Packet

C2S랑 S2C랑 패킷을 다르게 해야할 필요성이 있음.
클라에서 서버로 보내는 정보는 내가 이동했다, 점프했다, 공격했다, 회전했다 정도지만
서버에서 클라로 보내는 정보는 어떤 투사체가 생성됐다, 플레이어 상태가 갱신됐다 등이므로.

서버에서 클라로 보내는 정보.
 - 투사체 정보
 - 플레이어 정보

 담아야 하는 내용.
 - 방향, 고개각도, 속력
 - 위치(x, y, z)
 - 플레이어 idx, 추가 정보 idx(그 플레이어가 몇 번 공격을 했는지)

클라에서 서버로 보내는 정보.
 - 이동할 거다.
 - 점프할 거다.
 - 공격할 거다.(몇 번째로)
 - 회전할 거다.(각도 나와야 함)

***********************************************************************/

//namespace PacketType {
//	enum {
//		MoveForward = 0,
//		MoveBackward,
//		MoveRight,
//		MoveLeft,
//		RotateXAxis,
//		RotateYAxis,
//		Jump,
//		Fire,
//
//		count
//	};
//}
//
//struct Packet {
//	byte	m_type;
//	float	m_value;
//};

namespace PacketType {
	enum {
		MoveForward = 0,
		MoveBackward,
		MoveRight,
		MoveLeft,
		RotateXAxis,
		RotateYAxis,
		Jump,
		Fire,

		count
	};
}

struct C2SPacket {
	byte	type;
	float	value;
};

struct S2CPacket {
	byte	type;		// 플레이어인지 누군지
	float	angleYAxis;	// 보는 방향
	float	angleXAxis;	// 고개 각도
	float	speed;		// 속력
	float	x, y, z;	// 위치
	byte	playerIdx;	// 플레이어 idx
	byte	attackIdx;	// 일반 공격 0, 테크닉 1~4
};

/***********************************************************************

투사체 정보가 가져야 할 내용들.

투사체는 CPlayer에서 포인터로 가리킬 예정이므로 누구의 것인지 정도는 필요 없을 것.

 - 투사체의 위치, 회전 등의 변환 정보를 담을 transform 데이터
 - 투사체의 속성

***********************************************************************/

struct ProjectileInfo {
	/* Transform Data */
	DirectX::XMFLOAT4X4 m_xmf4x4Transform;

	/* Team 0: blue, 1: orenge */
	bool				m_team;

	/* 첫 번째는 mainEffect, 그 외는 subEffect */
	CSkill *			m_pSkill[4];
};

/***********************************************************************

플레이어 정보가 가져야 할 내용들.

 - 플레이어의 위치, 회전 등의 변환 정보를 담을 transform 데이터
 - 플레이어의 체력, 보고 있는 시야 방향(x축 각도), 모델 종류
 - 스킬의 종류, 컴포넌트 등

***********************************************************************/

class CPlayer {
public:
	void Update(float elapsedTime);
	void Move(byte direction);
	void Fire(int type);
	void Jump();

public:
	/* socket */
	SOCKET				m_socket;

	/* Trasform Data */
	DirectX::XMFLOAT4X4 m_xmf4x4Transform;

	/* Team 0: blue, 1: orenge */
	bool				m_team;

	/* HP 0 ~ 200, default 100 */
	unsigned char		m_HP;

	/* viewAngle 5 ~ 175, default 90 
	모델 렌더링 전용
	투사체 발사는 클라에서 서버로 방향벡터를 주는 것이 맞는가?
	맞으면 이 변수는 그냥 char 써도 될 것 같은데
	만약 클라에서 서버로 얘가 뭘 쐈다 정도만 보내고 서버에서 캐릭터의
	보고 있는 방향을 가지고 판단한다면?
	그렇게 한다 해도 1도 정도는 큰 문제 없지 않을까? 건슈팅 게임인데 당연히 되겠지..
	*/
	unsigned char		m_viewAngle;

	/* modelType head, upper, lower, shoes */

	/* weaponType smg, shotgun, */
	byte				m_weaponType;

	/* Technique */
	/**************************************************************************
	┌───────────┬──────────┬──────────┬──────────┐
	│mainSkill_0│subskill_0│subskill_1│subskill_2│
	├───────────┼──────────┼──────────┼──────────┤
	│mainSkill_1│subskill_0│subskill_1│subskill_2│
	├───────────┼──────────┼──────────┼──────────┤
	│mainSkill_2│subskill_0│subskill_1│subskill_2│
	├───────────┼──────────┼──────────┼──────────┤
	│mainSkill_3│subskill_0│subskill_1│subskill_2│
	└───────────┴──────────┴──────────┴──────────┘
	**************************************************************************/
	CSkill*				m_pTechniques[MAX_TECHNIQUE][MAX_SKILL];

};

/***********************************************************************

레벨 데이터가 가져야 하는 정보.

 - 플레이어 번호별 스폰 리스트.
 - 충돌 맵(공간 분할을 할 거면 큰 Zone과 CollisionBox 들로 구성되는 것이 맞음).
 - 조명 위치와 종류.
 - 데코레이션(충돌검사 X, 렌더 O).
 - 프롭(충돌검사 O, 렌더 O).
 - 존(충돌검사 O, 렌더 X).

***********************************************************************/

struct LevelInfo {

};

/***********************************************************************

버프 데이터가 가져야 하는 정보.

 - 어떤 버프인지, 지속 시간은 얼마인지, 누구에게 적용인지.

***********************************************************************/

struct BuffInfo {
	unsigned char	m_type;
	float			m_remainingTime;
	char			m_targetIdx;

	BuffInfo() {}
	BuffInfo(int type, float duration, int targetIdx) :
		m_type(type), m_remainingTime(duration), m_targetIdx(targetIdx) {}

	void Update(float elepsedTime) {
		m_remainingTime -= elepsedTime;
	}

	/* 끝났으면 true 반환, 안 끝났으면 false 반환 */
	bool isEnd() { return ( m_remainingTime <= 0 ); }
};