#pragma once

#define MAX_PLAYER 4

/***********************************************
����ũ�� ��Ŷ�� �ְ� �ް�,
�� ���� Ÿ������ ��� �ؼ��� ������ �Ǵ�.

��¼��.
���� ũ�� ��Ŷ ������ vs ���� + ���� ũ�� ��Ŷ ������
***********************************************/
namespace GameMsg {
	enum {
		No=0,
		Ok,
		RequestConfirm,

		ClientAccept,
		/***********************************************
		AccountData�� �� ����� ���� �ִ� ��, ��ų����Ʈ,
		���� � ��ũ�� ������, ����� �� ���� �ִ���,
		���� �Ѵٸ� ������ � ������ ��
		PlayerData�� ���� ��ų ����Ʈ ���� ������ ��������
		���� ������.
		���� ��� ����, ��, ����, ��ũ�� �� ��
		***********************************************/

		ClientAccountData,

		RequestChangeWeapon,
		RequestChangeTechnique,

		ClientStartMatching,
		ClinetPlayerData,

		/***********************************************
		LevelData�� ��Ÿ ���
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
�ִϸ��̼��� �ٲ�� �ϴ� ������?

- �����ִٰ� �̵�
- �̵��ϴٰ� ����
- ����
- ���
- ����
- ������

�������� �ʿ��� ������?

- ���� ���� ����
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
������ ��Ŷ ����ü
�̸��� C2S or S2C + GameMsg ���� ����
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

