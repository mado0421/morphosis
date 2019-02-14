#pragma once

#define MAX_PLAYER 4

namespace GameMsg {
	enum {
		LevelIdx = 0,
		PlayerInfo,
		PlayerLeave,
		Check,
		OK,
		StartGame,

		count
	};
}

namespace RoomState {
	enum {
		Ready = 0,
		Playing
	};
}

namespace S2CType {
	enum {
		Move = 0,
		Fire,
		Dmg,
	};
}

namespace C2SType {
	enum {
		Move = 0,
		Jump,
		Fire,
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
