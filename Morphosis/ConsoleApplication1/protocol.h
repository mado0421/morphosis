#pragma once

#define MAX_PLAYER 4

/***********************************************
고정크기 패킷을 주고 받고,
맨 앞의 타입으로 어떻게 해석할 것인지 판단.

어쩌지.
고정 크기 패킷 보내기 vs 고정 + 유동 크기 패킷 보내기
***********************************************/
namespace GameMsg {
	enum {
		No=0,
		Ok,
		RequestConfirm,

		ClientAccept,
		/***********************************************
		AccountData는 그 사람이 갖고 있는 돈, 스킬포인트,
		현재 어떤 테크닉 셋인지, 무기는 뭘 끼고 있는지,
		만약 한다면 외형은 어떤 것인지 등
		PlayerData는 돈과 스킬 포인트 등을 제외한 렌더링을
		위한 정보들.
		예를 들어 외형, 팀, 무기, 테크닉 셋 등
		***********************************************/

		ClientAccountData,

		RequestChangeWeapon,
		RequestChangeTechnique,

		ClientStartMatching,
		ClinetPlayerData,

		/***********************************************
		LevelData와 기타 등등
		***********************************************/
		RoomData,

		RoomPrepareToStart,
		ClientPrepareToStart,
		RoomStartGame,

		C2SData,
		S2CData,

		RoomPrepareToEnd,
		ClientResult,

		//LevelIdx,
		//PlayerInfo,
		//PlayerLeave,
		//Check,
		//StartGame,

		count
	};
}

namespace RoomState {
	enum {
		Ready = 0,
		Playing
	};
}

/***********************************************
애니메이션을 바꿔야 하는 행위는?

- 멈춰있다가 이동
- 이동하다가 멈춤
- 공격
- 사망
- 점프
- 재장전

렌더링에 필요한 행위는?

- 보는 방향 변경
- 

***********************************************/

namespace S2CType {
	enum {
		Damage,
		Move,
		Jump,
		Create,
		Rotate,
		Reload,
		Dead,

	};
}

namespace C2SType {
	enum {
		Attack,
		Jump,
		Move,
		Rotate,
		UseTech,
		Reload
	};
}

struct S2CPacket {
	byte type;
	byte value[16];
};

struct C2SPacket {
	byte type;
	byte value[16];
};

/***********************************************
종류별 패킷 구조체
이름은 C2S or S2C + GameMsg 으로 통일
***********************************************/

struct C2SClientAccept {
	char ID[16];
	char PW[16];
};

struct S2CClientAccountData {
	char playerName[16];
	int	 money;
	int	 skillPoint;
	byte weaponType;
	byte techniqueType[3][4];
	byte customizeType[4];
};

struct C2SRequestChangeWeapon {
	byte weaponType;
};

struct C2SRequestChangeTechnique {
	byte techniqueIdx;
	byte techniqueType[4];
};

