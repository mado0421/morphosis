#pragma once

// Define 정의
//#define TEST_

/*********************************************************************
2019-06-16
디바이스 관련 상수
*********************************************************************/
//#ifdef TEST_
//#define FRAME_BUFFER_WIDTH			(int)(1920   / 2.0)
//#define FRAME_BUFFER_HEIGHT			(int)(1080   / 2.0)
//#else
#define FRAME_BUFFER_WIDTH			1920
#define FRAME_BUFFER_HEIGHT			1080
//#endif


/*********************************************************************
2019-06-16
루트파라미터 인덱스로 관리할 글로벌 변수
*********************************************************************/
extern int g_RootParameterCamera;
extern int g_RootParameterObject;
extern int g_RootParameterTexture;
extern int g_RootParameterAnimation;
extern int g_RootParameterUI;


/*********************************************************************
2019-06-16
텍스처 관련 상수
*********************************************************************/
#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05


/*********************************************************************
2019-06-16
파일 입출력 관련 상수
*********************************************************************/
#define ASSETPATH "Assets/"
#define LASSETPATH L"Assets/"
constexpr float		g_fLevelScale		= 40.0f;

/*********************************************************************
2019-07-20
리소스 관리를 위한 변수들
*********************************************************************/
class CTexture;
class CModel;
struct ID3D12PipelineState;
class CAnimationController;
struct AINode;

struct Request;
struct PlayerStats {
	XMFLOAT3 pos;
	bool isDied;
};




extern std::vector<CTexture*>				g_vecTexture;
extern std::vector<CModel*>					g_vecModel;
extern std::vector<CAnimationController*>	g_vecAnimController;
extern std::vector<AINode*>					g_vecAINode;
extern std::vector<ID3D12PipelineState*>	g_vecPSO;
extern PlayerStats							g_player0Info;
extern std::queue<Request>					g_queueRequest;

class FMOD::Sound;
class FMOD::System;
class FMOD::Channel;
extern std::vector<FMOD::Sound *>	g_vecSound;
extern FMOD::System					*g_System;
extern FMOD::Channel				*g_Channel;
//struct FMOD_SYSTEM;
//struct FMOD_CHANNEL;
//struct FMOD_SOUND;
//extern FMOD_SYSTEM *g_System;
//extern std::vector< FMOD_SOUND *>	g_vecSound;
//extern FMOD_CHANNEL* channel;
enum class SOUND {
	BGM,
	SHOT,

	count
};
/*********************************************************************
2019-06-16
오브젝트 관련 상수
*********************************************************************/
constexpr float		g_fDefaultUnitScale			= 25.0f;

extern unsigned int	gnCbvSrvDescriptorIncrementSize;
constexpr float		g_fDefaultPlayerSpeed		= g_fDefaultUnitScale * 10;
constexpr float		g_fDefaultProjectileSpeed	= g_fDefaultPlayerSpeed * 5.0f;
constexpr int		g_nAnimBone					= 64;
constexpr int		g_nProjectilePerPlayer		= 30;
constexpr float		TIMER_ATT					= 0.05f;
constexpr float		TIMER_RESPANW				= 5.0f;

constexpr int		TEAM_DEFAULT				= 0;
constexpr int		TEAM_RED					= 1;
constexpr int		TEAM_BLUE					= 2;

constexpr int		g_DefaultHealthPoint		= 100;
constexpr float		g_RespawnDelay				= 3.0f;

constexpr int		g_DefaultDamage				= 10;
constexpr int		g_DefaultRPM				= 550;
constexpr float		g_DefaultProjectileLifeTime = 5.0f;

/*********************************************************************
2019-06-16
카메라 관련 상수
*********************************************************************/
#define CAM_Y_DISTANCE 0











#define MOUSE_XSPEED 10
#define MOVE_SPEED 0.15f
#define ROTATE_SPEED 300
#define PO_PER_PLAYER 16

/*********************************************************************
2019-07-02
플레이어 입력과 게임에서 사용하는 여러 가지 수치
*********************************************************************/
constexpr float		g_Gravity				= 9.8f;



constexpr float		g_MouseInputSensitivity = 50.0f;
extern bool			g_IsMouseMode;


enum class RequestType { MoveForward, Shoot, Skill0, Skill1, Jump, MoveBackward };
enum class CastType {SELFCAST, AROUND, HIT, PROJECTILE };
struct Request {
	int			playerIdx;
	RequestType type;
};
enum class ColliderTag {
	PROP,
	PROJECTILE,
	PLAYER
};
enum class ColliderType
{
	BOX = 0,
	SPHERE
};
enum class ModelType {
	DefaultModel,
	AnimatedModel,
	FloatingUI,
	ETC
};
enum class PSO {
	DefaultModel,
	AnimatedModel,
	FloatingUI,
	DefaultUI,
	Debug,

	count
};
enum class Move {
	W,
	A,
	S,
	D,
	Q,
	E,
	SPACE,

	count
};
enum class AnimationState {
	IDLE,
	RUN,
	FIRE,
	STARTJUMP,
	ENDJUMP,
	DIE,

	count
};
enum class SceneType {
	//LOBBY,
	MAINPLAY,

	count
};
enum class MouseState {
	NONE,
	OVER,
	CLICKED
};
namespace KEY {
	enum {
		A = 65,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		_0 = 48,
		_1,
		_2,
		_3,
		_4,
		_5,
		_6,
		_7,
		_8,
		_9
	};
}
