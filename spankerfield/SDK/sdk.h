#pragma once
#include "../common.h"

#pragma warning (disable : 4200)

#define OFFSET_DXRENDERER             0x142738080
#define OFFSET_GAMECONTEXT            0x142670D80
#define OFFSET_GAMERENDERER           0x142672378
#define OFFSET_DBRENDERER             0x14063D0C0 
#define OFFSET_DRAWTEXT               0x14063E2E0
#define OFFSET_DRAWLINE               0x14063DEB0
#define OFFSET_DRAWLINERECT           0x14063DF20
#define OFFSET_DRAWFILLRECT           0x14063E0B0
#define OFFSET_BORDERINPUTNODE        0x142671FB0
#define OFFSET_ANGLES				  0x1423B2EC0
#define OFFSET_FIRING_WEAPON		  0x1423B2EC8
#define OFFSET_MAIN					  0x142364B78
#define OFFSET_WORLDRENDERSETTINGS	  0x1426724A0
#define OFFSET_DXDISPLAYSETTINGS	  0x142364C70
#define OFFSET_FLAG					  0x142385A98
#define OFFSET_SSMODULE				  0x14273D6E8
#define OFFSET_CONSOLECOMMANDS		  0x140663A20
#define OFFSET_DISPATCHMESSAGE		  0x140626810
#define OFFSET_PBSSRETURN			  0x140CBCD29
#define OFFSET_FIRSTTYPEINFO		  0x1423E41B8
#define OFFSET_GETENTITYLIST		  0x1407CD5D0

#define OFFSET_WEAPONSHOOTSPACE OFFSET_FIRING_WEAPON + 0x28

#define MAX_PLAYERS                            70
#define MAX_EXPLOSIVES                        128

#if !(defined _WIN64)
#error Only for x64 systems.
#endif

#include "type_info.h"

#define _PTR_MAX_VALUE ((PVOID)0x000F000000000000)
__forceinline bool IsValidPtr(PVOID p) { return (p >= (PVOID)0x10000) && (p < _PTR_MAX_VALUE) && p != nullptr; }

using namespace DirectX::SimpleMath;

class GameEntity;
class DxRenderer;
class Screen;
class DxDisplaySettings;
class GameRenderer;
class GameRenderSettings;
class RenderView;
class BorderInputNode;
class InputCache;
class UnneededClass;
class Keyboard;
class KeyboardDevice;
class Mouse;
class MouseDevice;
class MouseState;
class Main;
class Server;
class GameSettings;
class ServerSettings;
class Client;
class ClientSettings;
class FileSystem;
class GameTimer;
class GameTimeSettings;
class ServerJobData;
class MessageManager;
class ClientGameContext;
class GameView;
class Online;
class InterpolationManager;
class PlayerManager;
class GameTime;
class Level;
class HavokPhysicsManager;
class TerrainPhysicsEntity;
class Physics;
class PhysicsSettings;
class PhysicsRenderSettings;
class IPhysicsRayCaster;
class HavokManager;
class LevelData;
class Connection;
class ClientPlayer;
class VeniceSoldierCustimizationAsset;
class CustomizationTable;
class Customization;
class ClientVehicleEntity;
class ClientSoldierEntity;
class AntAnimatableComponent;
class GameAnimatable;
class ClientSoldierWeaponsComponent;
class AnimatedSoldierWeaponHandler;
class SoldierWeapon;
class SoldierAimingSimulation;
class AimAssist;
class TypeInfo;
class ClientWeapon;
class PrimaryFire;
class FiringFunctionData;
class ShotConfigData;
class BulletEntityData;
class ProjectileEntityData;
class WeaponSuppressionData;
class MessProjectileEntityData;
class SoundAsset;
class SoundScopeData;
class SoundScopeStrategyData;
class OverheatData;
class WeaponModifier;
class WeaponFiring;
class ClientSoldierBodyComponent;
class RagdollComponent;
class UpdatePoseResultData;
class QuatTransform;
class ParachuteComponent;
class ClientSpottingComponent;
class ClientSpottingTargetComponent;
class ClientExplosionEntity;
class ClientGameWorld;
class VaultComponent;
class SoldierSuppressionComponent;
class ClientControllableEntity;
class HealthComponent;
class WeaponShotModifier;
class SoundModifier;
class FiringEffectsModifier;
class FiringDataModifier;
class ProjectileModifier;
class AimingSimulationModifier;
class ZeroingModifier;
class WeaponSway;
class GunSwayData;

template<typename T>
struct WeakToken
{
	T* m_pRealPtr;
};

template<typename T>
struct ConstWeakPtr
{
	WeakToken<T>* m_pToken;

	T* GetData()
	{
		__try {
			if (!this)
				return NULL;

			if (!this->m_pToken)
				return NULL;

			if (!this->m_pToken->m_pRealPtr)
				return NULL;

			return (T*)((DWORD_PTR)this->m_pToken->m_pRealPtr - sizeof(void*));
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return nullptr;
		}

	}

};

template<typename T>
class WeakPtr : public ConstWeakPtr<T>
{
};

struct OnlineId
{
	uint64_t m_personaid;	// 0x00
	char m_id[0x11];		// 0x08
	char pad_019[0x7];		// 0x19
};

class DxRenderer
{
public:
	BYTE pad00[0x38];                    // 0x00
	Screen* m_pScreen;                    // 0x38
	BYTE pad78[0xC0];                    // 0x40
	ID3D11Device* m_pDevice;            // 0x100
	ID3D11DeviceContext* m_pContext;    // 0x108
	char _0x0110[192];
	void* m_settings; //0x01D0 

	static DxRenderer* GetInstance()
	{
		return *(DxRenderer**)OFFSET_DXRENDERER;
	}
};//Size=0x0110

class Screen
{
public:
	BYTE pad00[0x58];				//0x00
	unsigned int m_Width;			//0x58
	unsigned int m_Height;			//0x5C
	unsigned int m_WindowWidth;			//0x60
	unsigned int m_WindowHeight;		//0x64
	BYTE pad01[0x88];				//0x68
	IDXGISwapChain* m_pSwapChain;		//0xF0
};

class DxDisplaySettings
{
public:
	char _0x0000[40];
	__int32 m_FullscreenHeight; //0x0028 
	__int32 m_FullscreenWidth; //0x002C 
	float m_RefreshRate; //0x0030 
	__int32 m_FullscreenOutputIndex; //0x0034 
	__int32 m_PresentInterval; //0x0038 
	__int32 m_PresentImmediateThreshold; //0x003C 
	__int32 m_RenderAheadLimit; //0x0040 
	float m_StereoDepth; //0x0044 
	float m_StereoConvergeanceScale; //0x0048 
	float m_StereoSeparationScale; //0x004C 
	float m_StereoSoldierZoomConvergeanceScale; //0x0050 
	__int32 m_NVIDIAMinDriverVersion; //0x0054 
	char* m_AMDMinDriverVersion; //0x0058 
	float m_LowResDisplayPlaneScale; //0x0060 
	char _0x0064[52];

};//Size=0x0098

class GameRenderer
{
public:
	char _0x0000[72];
	GameRenderSettings* m_RenderSettings; //0x0048 
	char _0x0050[16];
	RenderView* m_pRenderView; //0x0060 

	static GameRenderer* GetInstance()
	{
		return *(GameRenderer**)OFFSET_GAMERENDERER;
	}
};//Size=0x0068

class GameRenderSettings
{
public:
	char _0x0000[16];
	__int32 m_InactiveSkipFrameCount; //0x0010 
	float m_ResolutionScale; //0x0014 
	__int32 m_XenonRingBufferSize; //0x0018 
	char _0x001C[36];
};//Size=0x0040

class RenderView
{
public:
	char _0x0000[64];
	Matrix m_Transform; //0x0040 
	char _0x0080[52];
	float m_FovY; //0x00B4 
	float m_DefaultFovY; //0x00B8 
	float m_NearPlane; //0x00BC 
	float m_FarPlane; //0x00C0 
	float m_AspectRatio; //0x00C4 
	float m_OrthoWidth; //0x00C8 
	float m_OrthoHeight; //0x00CC 
	char _0x00D0[384];
	float m_FovX; //0x0250 
	float m_DepthHeightRatio; //0x0254 
	float m_FovScale; //0x0258 
	float m_FovScaleSquared; //0x025C 
	Matrix m_View; //0x0260 
	Matrix m_ViewTranspose; //0x02A0 
	Matrix m_ViewInverse; //0x02E0 
	Matrix m_Projection; //0x0320 
	Matrix m_ViewAtOrigin; //0x0360 
	Matrix m_ProjectionTranspose; //0x03A0 
	Matrix m_ProjectionInverse; //0x03E0 
	Matrix m_ViewProjection; //0x0420 
	Matrix m_ViewProjectionTranspose; //0x0460 
	Matrix m_ViewProjectionInverse; //0x04A0 

};//Size=0x04E0

class BorderInputNode
{
public:
	char _0x0000[8];
	InputCache* m_InputCache; //0x0008 
	char _0x0010[48];
	__int64 m_Vtable; //0x0040 
	char _0x0048[8];
	Keyboard* m_pKeyboard; //0x0050 
	Mouse* m_pMouse; //0x0058 
	char _0x0060[40];

	static BorderInputNode* GetInstance()
	{
		return *(BorderInputNode**)OFFSET_BORDERINPUTNODE;
	}
};//Size=0x0088

class InputCache
{
public:
	char _0x0000[4];
	float m_Event[223]; //0x0004 
};//Size=0x0008

enum InputDeviceMouseButtons
{
	IDB_Button_0 = 0,
	IDB_Button_1 = 1,
	IDB_Button_2 = 2,
	IDB_Button_3 = 3,
	IDB_Button_4 = 4,
	IDB_Button_5 = 5,
	IDB_Button_6 = 6,
	IDB_Button_7 = 7,
	IDB_Button_Undefined = 8
};

enum InputDeviceKeys
{
	IDK_None = 0,
	IDK_Escape = 1,
	IDK_1 = 2,
	IDK_2 = 3,
	IDK_3 = 4,
	IDK_4 = 5,
	IDK_5 = 6,
	IDK_6 = 7,
	IDK_7 = 8,
	IDK_8 = 9,
	IDK_9 = 10,
	IDK_0 = 11,
	IDK_Minus = 12,
	IDK_Equals = 13,
	IDK_Backspace = 14,
	IDK_Tab = 15,
	IDK_Q = 16,
	IDK_W = 17,
	IDK_E = 18,
	IDK_R = 19,
	IDK_T = 20,
	IDK_Y = 21,
	IDK_U = 22,
	IDK_I = 23,
	IDK_O = 24,
	IDK_P = 25,
	IDK_LeftBracket = 26,
	IDK_RightBracket = 27,
	IDK_Enter = 28,
	IDK_LeftCtrl = 29,
	IDK_A = 30,
	IDK_S = 31,
	IDK_D = 32,
	IDK_F = 33,
	IDK_G = 34,
	IDK_H = 35,
	IDK_J = 36,
	IDK_K = 37,
	IDK_L = 38,
	IDK_Semicolon = 39,
	IDK_Apostrophe = 40,
	IDK_Grave = 41,
	IDK_LeftShift = 42,
	IDK_Backslash = 43,
	IDK_Z = 44,
	IDK_X = 45,
	IDK_C = 46,
	IDK_V = 47,
	IDK_B = 48,
	IDK_N = 49,
	IDK_M = 50,
	IDK_Comma = 51,
	IDK_Period = 52,
	IDK_Slash = 53,
	IDK_RightShift = 54,
	IDK_Multiply = 55,
	IDK_LeftAlt = 56,
	IDK_Space = 57,
	IDK_Capital = 58,
	IDK_F1 = 59,
	IDK_F2 = 60,
	IDK_F3 = 61,
	IDK_F4 = 62,
	IDK_F5 = 63,
	IDK_F6 = 64,
	IDK_F7 = 65,
	IDK_F8 = 66,
	IDK_F9 = 67,
	IDK_F10 = 68,
	IDK_Numlock = 69,
	IDK_ScrollLock = 70,
	IDK_Numpad7 = 71,
	IDK_Numpad8 = 72,
	IDK_Numpad9 = 73,
	IDK_Subtract = 74,
	IDK_Numpad4 = 75,
	IDK_Numpad5 = 76,
	IDK_Numpad6 = 77,
	IDK_Add = 78,
	IDK_Numpad1 = 79,
	IDK_Numpad2 = 80,
	IDK_Numpad3 = 81,
	IDK_Numpad0 = 82,
	IDK_Decimal = 83,
	IDK_OEM_102 = 86,
	IDK_F11 = 87,
	IDK_F12 = 88,
	IDK_F13 = 100,
	IDK_F14 = 101,
	IDK_F15 = 102,
	IDK_Kana = 112,
	IDK_PTBRSlash = 115,
	IDK_Convert = 121,
	IDK_NoConvert = 123,
	IDK_Yen = 125,
	IDK_PTBRNUMPADPOINT = 126,
	IDK_NumpadEquals = 141,
	IDK_PrevTrack = 144,
	IDK_At = 145,
	IDK_Colon = 146,
	IDK_Underline = 147,
	IDK_Kanji = 148,
	IDK_Stop = 149,
	IDK_Ax = 150,
	IDK_Unlabeled = 151,
	IDK_NextTrack = 153,
	IDK_NumpadEnter = 156,
	IDK_RightCtrl = 157,
	IDK_Mute = 160,
	IDK_Calculator = 161,
	IDK_PlayPause = 162,
	IDK_MediaStop = 164,
	IDK_VolumeDown = 174,
	IDK_VolumeUp = 176,
	IDK_WebHome = 178,
	IDK_NumpadComma = 179,
	IDK_Divide = 181,
	IDK_PrintScreen = 183,
	IDK_RightAlt = 184,
	IDK_Home = 199,
	IDK_ArrowUp = 200,
	IDK_PageUp = 201,
	IDK_ArrowLeft = 203,
	IDK_ArrowRight = 205,
	IDK_End = 207,
	IDK_ArrowDown = 208,
	IDK_PageDown = 209,
	IDK_Insert = 210,
	IDK_Delete = 211,
	IDK_LeftWin = 219,
	IDK_RightWin = 220,
	IDK_AppMenu = 221,
	IDK_Power = 222,
	IDK_Sleep = 223,
	IDK_Wake = 227,
	IDK_WebSearch = 229,
	IDK_WebFavorites = 230,
	IDK_WebRefresh = 231,
	IDK_WebStop = 232,
	IDK_WebForward = 233,
	IDK_WebBack = 234,
	IDK_MyComputer = 235,
	IDK_Mail = 236,
	IDK_MediaSelect = 237,
	IDK_Pause = 197,
	IDK_Undefined = 255
};

class Mouse
{
public:

	class MouseDevice
	{
	public:

		class MouseState
		{
		public:
			long x; //0x0000
			long y; //0x0004
			long z; //0x0008
			unsigned char buttons[0x8]; //0x000C
		};

		virtual void Function0(); // 0x0000
		virtual void Read(float SampleTime, bool VisualFrame); //0x0008 
		virtual const char* Name(void); //0x0010 
		virtual const char* Name(unsigned int); // 0x0018 
		virtual unsigned int ControlCount(); // 0x0020 
		virtual bool IsConnected(); // 0x0028
		virtual bool IsAnalogue(unsigned int); // 0x0030
		virtual float GetValue(unsigned int);	// 0x0038

		char _0x0008[144];
		__int64 m_HWND; //0x0098 
		__int64 m_pWindowProcedure; //0x00A0 
		char _0x00A8[40];
		BYTE m_CursorMode; //0x00D0 
		BYTE m_UIOwnsInput; //0x00D1 
		BYTE m_ShowCursor; //0x00D2 
		BYTE m_CursorConfined; //0x00D3 
		BYTE m_HasOverflowed; //0x00D4 
		BYTE m_UseRawMouseInput; //0x00D5 
		BYTE m_WindowActive; //0x00D6 
		char _0x00D7[25];
		MouseState m_Current; //0x00F0
		MouseState m_Buffer; //0x0104
		char _0x0118[24];

	}; //Size = 0x0130

	char _0x0000[16];
	MouseDevice* m_pDevice; // 0x0010

}; //Size = 0x0100

class Keyboard
{
public:

	class KeyboardDevice
	{
	public:
		virtual void Function0(); // 0x0000
		virtual void Read(float SampleTime, bool VisualFrame); //0x0008 
		virtual const char* Name(unsigned int); //0x0010 
		virtual const char* Name(void); // 0x0018 
		virtual unsigned int ControlCount(); // 0x0020 
		virtual bool IsConnected(); // 0x0028
		virtual bool IsAnalogue(unsigned int); // 0x0030
		virtual float GetValue(unsigned int);	// 0x0038

		char _0x0008[144];
		__int64 m_HWND; //0x0098 
		__int64 m_pWindowProcedure; //0x00A0 fb::WindowProcedure( ) 
		unsigned char m_Current[0x100]; //0x00A8
		unsigned char m_Buffer[0x100]; //0x01A8
		BYTE m_TypingMode; //0x02A8 
		BYTE m_HasOverflowed; //0x02A9 
		__int32 m_CodePage; //0x02AA 
		char _0x02AE[2];
	}; //Size = 0x02B0

	class KeyboardActionData
	{
	public:
		char _0x0000[16];
		BYTE m_IsAnalog; //0x0010 
		BYTE m_NegateValue; //0x0011 
		char _0x0012[14];
	}; //Size = 0x0020

	char _0x0000[8];
	KeyboardDevice* m_pDevice; //0x0008 
	char _0x0010[136];
	KeyboardActionData* m_pActionData; //0x0098
	char _0x00A0[24];
}; //Size = 0x00B8

class ServerJobData
{
public:
	char _0x0000[136];
	Server* m_pServer; //0x0088 
	ClientGameContext* m_pGameContext; //0x0090 
	__int64 m_pParams; //0x0098 
};//Size=0x00A0

class Main
{
public:
	char _0x0000[32];
	unsigned char m_IsDedicatedServer; //0x0020 
	unsigned char m_IsDedicatedServerData; //0x0021 
	unsigned char m_IsPaused; //0x0022 
	char _0x0023[5]; //0x0023 
	char _0x0028[32];
	Server* m_pServer; //0x0048 
	Client* m_pClient; //0x0050 
	FileSystem* m_pFileSystem; //0x0058 
	GameTimer* m_pSimulationGameTimer; //0x0060 
	GameTimer* m_pUIGameTimer; //0x0068 
	char _0x0070[8];
	ServerJobData m_ServerJobData; //0x0078 

	static Main* GetInstance()
	{
		return *(Main**)OFFSET_MAIN;
	}
};//Size=0x0118

class Server
{
public:
	char _0x0000[8];
	GameSettings* m_pGameSettings; //0x0008 
	char _0x0010[120];
	ServerSettings* m_pServerSettings; //0x0088 
};//Size=0x0090

class GameSettings
{
public:
	char _0x0000[64];

};//Size=0x0040

class ServerSettings
{
public:
	char _0x0000[64];

};//Size=0x0040

class Client
{
public:
	char _0x0000[8];
	GameSettings* m_pGameSettings; //0x0008 
	char _0x0010[24];
	ClientGameContext* m_pGameContext; //0x0028 
	ClientSettings* m_pClientSettings; //0x0030 
	char _0x0038[8];

};//Size=0x0040

class ClientSettings
{
public:
	char _0x0000[64];

};//Size=0x0040

class FileSystem
{
public:
	char _0x0000[56];

};//Size=0x0038

class GameTimer
{
public:
	__int64 m_Count; //0x0000 
	__int64 m_Counts; //0x0008 
	double m_DeltaTime; //0x0010 
	double m_Time; //0x0018 
	double m_TargetDelta; //0x0020 
	__int64 m_SleepMode; //0x0028 
	GameTimeSettings* m_pGameTimeSettings; //0x0030 
	__int32 m_TickFrequency; //0x0038 
	__int32 m_Ticks; //0x003C 
	__int32 m_OldTicks; //0x0040 
	__int32 m_Frames; //0x0044 
	__int32 N00000498; //0x0048 
	__int32 N000004A2; //0x004C 
	__int32 N00000499; //0x0050 
	__int32 N000004A5; //0x0054 
	double m_DeltaTime2; //0x0058 
	double m_ActualDelta; //0x0060 
	double m_OldActualDelta; //0x0068 
	double m_ExpectedTotalTime; //0x0070 
	double m_RealTotalTime; //0x0078 
	double m_SleptTime; //0x0080 
	double m_LostTime; //0x0088 
	double m_WorstFrameTime; //0x0090 
	double m_LogTime; //0x0098 
	double m_LatestDeltaTime; //0x00A0 
	double m_TimeInTick; //0x00A8 
	double m_BaseTicks; //0x00B0 
	double m_BaseTime; //0x00B8 
	unsigned char m_HitVSync; //0x00C0 

};//Size=0x00C1

class GameTimeSettings
{
public:
	char _0x0000[56];

};//Size=0x0038

class ClientGameContext
{
public:
	char _0x0000[8];
	MessageManager* m_messageManager;
	char _0x0010[16];
	GameTime* m_pGameTime; //0x0020 
	Level* m_pLevel; //0x0028 
	char _0x0030[48];
	PlayerManager* m_pPlayerManager; //0x0060 
	Online* m_pOnline; //0x0068 
	GameView* m_pGameView; //0x0070 
	InterpolationManager* m_pInterpolationManager; //0x0078 
	char _0x0080[960];

	inline static ClientGameContext* GetInstance()
	{
		return *reinterpret_cast<ClientGameContext**>(OFFSET_GAMECONTEXT);
	}
};//Size=0x0440

class GameView
{
public:
	char _0x0000[64];

};//Size=0x0040

class Online
{
public:
	char _0x0000[56];
	Connection* m_pConnection; //0x0038 

};//Size=0x0040

class InterpolationManager
{
public:
	char _0x0000[64];

};//Size=0x0040

namespace eastl
{
	class allocator
	{
	public:
		__forceinline allocator()
		{
		}

		__forceinline allocator(eastl::allocator* __formal)
		{
		}

		__forceinline void set_name(const char* __formal)
		{
		}

		__forceinline const char* get_name()
		{
			return "EASTL";
		}

		__forceinline void* allocate(unsigned int n, int flags)
		{
			return malloc(n);
		}

		__forceinline void deallocate(void* p, unsigned int __formal)
		{
			free(p);
		}
	};

	template <class T, INT Count, INT PadSize>
	class fixed_vector
	{
	private:
		T* m_firstElement;
		T* m_lastElement;
		T* m_arrayBound;
		//LPVOID m_pad[PadSize];
		T m_data[Count];

	public:
		fixed_vector() {
			m_firstElement = (T*)m_data;
			m_lastElement = (T*)m_data;
			m_arrayBound = (T*)&m_data[Count];
		}

		void push_back(T* const value) {
			if (m_lastElement > m_arrayBound) {

				return;
			}
			*m_lastElement = *value;
			m_lastElement = m_lastElement + 1;
		};

		void clear() {
			m_firstElement = m_data;
			m_lastElement = m_data;
		}

		UINT Size() {
			return m_lastElement - m_firstElement;
		}

		T At(INT nIndex) {
			return m_firstElement[nIndex];
		}

		T operator [](INT index) { return At(index); }
	};

	template <class T>
	class vector
	{
	private:
		T* m_firstElement;
		T* m_lastElement;
		T* m_arrayBound;
		void* vftable;
	public:
		size_t size()
		{
			return (((intptr_t)m_lastElement - (intptr_t)m_firstElement) / sizeof(void*));
		}

		T At(int nIndex)
		{
			return *(T*)((intptr_t)m_firstElement + (nIndex * sizeof(T)));
		}
		T operator [](int index)
		{
			return At(index);
		}
	};
}

class PlayerManager
{
public:
	virtual void function();
	virtual eastl::vector<ClientPlayer*>* getPlayers();
	virtual eastl::vector<ClientPlayer*>* getSpectators();

	char _0x0008[1336];
	ClientPlayer* m_pLocalPlayer; //0x0540 
	ClientPlayer** m_ppPlayers; //0x0548 
	ClientPlayer* GetPlayerById(unsigned int id)
	{
		if (id < 70)
			return this->m_ppPlayers[id];
		return nullptr;
	}
};//Size=0x0550

class GameTime
{
public:
	char _0x0000[64];

};//Size=0x0040

class Level
{
public:
	char pad_0x0000[0x38]; //0x0000
	LevelData* m_pLevelData; //0x0038 
	class TeamEntityData** m_ppTeamEntityData; //0x0040 
	char* m_LevelName;
	char pad_0x0048[0xD0]; //0x0048
	HavokPhysicsManager* m_pHavokPhysics; //0x0120 
	char pad_0x0128[0x8]; //0x0128
	ClientGameWorld* m_pGameWorld; //0x0130 
	char pad_0x0138[0x308]; //0x0138

};//Size=0x0138

class HavokPhysicsManager
{
public:
	char _0x0000[80];
	HavokManager* m_pHavokManager; //0x0050 
	IPhysicsRayCaster* m_pRayCaster; //0x0058 
	char _0x0060[60];
	float m_MaxVehicleHeight; //0x009C 
	char _0x00A0[16];
	TerrainPhysicsEntity* m_pTerrainPhysicsEntity; //0x00B0 
	char _0x00B8[248];
	Physics* m_pPhysics; //0x01B0 
	char _0x01B8[8];
	PhysicsSettings* m_pPhysicsSettings; //0x01C0 
	PhysicsRenderSettings* m_pPhysicsRenderSettings; //0x01C8 

};//Size=0x01D0

class TerrainPhysicsEntity
{
public:
	char _0x0000[64];

};//Size=0x0040

class Physics
{
public:
	char _0x0000[64];

};//Size=0x0040

class PhysicsSettings
{
public:
	char _0x0000[64];

};//Size=0x0040

class PhysicsRenderSettings
{
public:
	char _0x0000[64];

};//Size=0x0040

class Material
{
public:
	enum MaterialFlag
	{
		MFSeeThrough = 0x1,
		MFClientDestructable = 0x40000000,
		MFPenetrable = 0x8
	};
	UINT32 m_flagsAndIndex;
	__forceinline bool isPenetrable()
	{
		return (m_flagsAndIndex & MFPenetrable);
	};
	__forceinline bool isSeeThrough()
	{
		return (m_flagsAndIndex & MFSeeThrough);
	};
};

class RayCastHit
{
public:
	Vector4 m_Position;  // 0x00
	Vector4 m_Normal;  // 0x10
	void* m_RigidBody; // 0x20
	Material m_Material;  // 0x28
	DWORD  m_Part;   // 0x30
	FLOAT  m_Lambda;  // 0x34
	char  _pad[200];  // 0x38
}; //Size = 0x0070

class IPhysicsRayCaster
{
public:
	enum RayCastFlags
	{
		NoCheck = 0,
		CheckDetailMesh = 0x0DB,
		IsAsyncRaycast = 0x2,
		DontCheckWater = 0x0FD,
		DontCheckTerrain = 0x07A,
		DontCheckRagdoll = 0x10,
		DontCheckCharacter = 0x20,
		DontCheckGroup = 0x40,
		DontCheckPhantoms = 0x80,
	};

	virtual bool PhysicsRayQuery(const char* identifier, Vector3* from, Vector3* to, RayCastHit* hit, int flag, eastl::fixed_vector<void const*, 8, 0>* PhysicsEntityList);
};//Size=0x0008

class HavokManager
{
public:
	char _0x0000[56];
	__int64 m_pHKPWorld; //0x0038 

};//Size=0x0040

class LevelData
{
public:
	char _0x0000[64];

};//Size=0x0040

class Connection
{
public:
	char _0x0000[64];

};//Size=0x0040

class ClientPlayer
{
public:

	virtual ~ClientPlayer();
	virtual void Function1();
	virtual void Function2();
	virtual void Function3();
	virtual bool IsInVehicle();

	char _0x0008[0x10];
	char* m_Name;
	char pad_0020[24];
	OnlineId m_onlineId;
	char pad_0058[312];
	__int32 m_SquadId1; //0x0190 
	char _0x0194[1972];
	void* m_pSelectedCustomizationAsset; //0x0948 
	char _0x0950[8];
	__int64 m_pSelectedActiveUnlocks; //0x0958 
	char _0x0960[2664];
	BYTE m_isAIPlayer; //0x13C8 
	bool m_IsSpectator; //0x13C9 
	char _0x13CA[2];
	uint32_t m_TeamId; //0x13CC 
	char _0x13D0[216];
	WeakPtr<ClientSoldierEntity> m_Corpse; //0x14A8 
	WeakPtr<ClientSoldierEntity> m_Soldier; //0x14B0 
	WeakPtr<ClientPlayer> m_Player; //0x14B8 
	ClientVehicleEntity* m_pAttachedControllable; //0x14C0 
	__int32 m_AttachedEntryId; //0x14C8 if == 0 -> driver
	char _0x14CC[4];
	ClientSoldierEntity* m_pControlledControllable; //0x14D0 
	__int32 m_ControlledEntryId; //0x14D8 

	inline ClientSoldierEntity* GetSoldier()
	{
		__try {
			return m_Soldier.GetData();
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return nullptr;
		}
	}
	inline ClientSoldierEntity* GetCorpse()
	{
		__try {
			if (m_Corpse.m_pToken) return m_Corpse.GetData();
			else return nullptr;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return nullptr;
		}
	}
	inline ClientVehicleEntity* GetVehicle()
	{
		if (this->IsInVehicle())
			return reinterpret_cast<ClientVehicleEntity*>(m_pAttachedControllable);

		return nullptr;
	}

};

class VeniceSoldierCustimizationAsset
{
public:
	char _0x0000[32];
	char* m_pUIKitIconName; //0x0020 
	char* m_pUIHudIcon; //0x0028 
	CustomizationTable* m_pAppearance; //0x0030 
	char _0x0038[16];
	CustomizationTable* m_pWeaponTable; //0x0048 

};//Size=0x0050

class CustomizationTable
{
public:
	char _0x0000[16];
	Customization* m_ppList; //0x0010 
	char _0x0018[40];

};//Size=0x0040

class Customization
{
public:
	char _0x0000[16];
	char* m_pSID; //0x0010 
	char _0x0018[8];
	__int64 m_ppUnlockParts; //0x0020 
	char _0x0028[16];

};//Size=0x0038

class LinearTransform
{
public:
	union
	{
		struct
		{
			Vector3 left;
			Vector3 up;
			Vector3 forward;
			Vector3 trans;
		};
		FLOAT data[4][4];
	};
	LinearTransform()
	{}
};

class AxisAlignedBox
{
public:
	Vector4 m_Min; //0x0000 
	Vector4 m_Max; //0x0010 

	AxisAlignedBox()
	{}
};//Size=0x0020

struct TransformAABBStruct
{
	Matrix Transform;
	AxisAlignedBox AABB;
	Matrix pad;

	TransformAABBStruct()
	{}
};

class Message : public ITypedObject
{
public:
	int m_Category; //0x0008 
	int m_Type; //0x000C 
	double m_dispatchTime;
	Message* m_next;
	int m_postedAtProcessMessageCounter;
	bool m_ownedByMessageManager;
};

class NetworkableMessage : public Message
{
public:
	char pad_0030[0x18];
};

class VeniceNetworkRequestMessage : public NetworkableMessage
{
public:
	virtual __int64 destruct(char a1);
	virtual void Function3();
	virtual void Function4();
	virtual void Function5();
	virtual void Function6();
	virtual void Function7();
	virtual void Function8();
	virtual void Function9();
	virtual VeniceNetworkRequestMessage* clone(__int64 allocator);

	int width; //0x0040
	int height; //0x0044
	char unknown48; //0x0048
	char pad_0049[3]; //0x0049
	int unknown4c; //0x004c
};

class VeniceNetworkRequestFrontBufferScreenshotMessage : public VeniceNetworkRequestMessage
{
public:

	static int Type() {
		return 0x7E7CCEDE;
	}
};

class VeniceNetworkRequestFrontBufferScreenshot2Message : public VeniceNetworkRequestMessage
{
public:

	static int Type() {
		return 0x88DD290C;
	}
};

class VeniceNetworkRequestScreenshotMessage : public VeniceNetworkRequestMessage
{
public:
	char unknown50;
	char pad_0050[7];

	static int Type() {
		return 0xE633EE7F;
	}
};

class ComponentCollectionEntry
{
public:
	WORD m_ClassId; //0x0000
	char _0x0002[0xE];
	ITypedObject* m_pComponent; //0x0010
	char _0x0018[8];
}; //0x0020

class ComponentCollection //fb::ComponentCollection
{
public:
	void* m_pOwner; //0x0000
	BYTE m_PlayerCount; //0x0008
	BYTE m_TotalCount; //0x0009
	BYTE m_OffsetCount; //0x000A
	char _0x000B[5];
	ComponentCollectionEntry m_Components[0]; //0x0010

	void SearchForComponent(const char* componentName) {
		/*for (int i = 0; i <= m_TotalCount; i++)
			printf("%s\n", m_Components[i].m_pComponent->getType()->m_InfoData->m_Name);
		*/

		/*if (!strcmp(componentName, m_Components[i].m_pComponent->getType()->m_InfoData->m_Name)) {
			printf("%p\n", m_Components[i].m_pComponent);
			return;
		}*/
	}

	template <class T>
	T* GetComponentByClassId(WORD ClassId) {
		ComponentCollectionEntry* pCurrentComponent = &m_Components[0];
		ComponentCollectionEntry* pEnd = End();

		while (pCurrentComponent->m_pComponent) {
			if (pCurrentComponent->m_ClassId == ClassId)
				return (T*)pCurrentComponent->m_pComponent;

			pCurrentComponent++;

			if (pCurrentComponent == pEnd)
				break;
		}

		return 0;
	}

	ComponentCollectionEntry* End()
	{
		return &m_Components[m_TotalCount + 1];
	}
}; //0x0010

class ClientControllableEntity
{
public:
	virtual TypeInfo* GetType();
	virtual void Function1(); //
	virtual void Function2(); //
	virtual void Function3(); //
	virtual void Function4(); //
	virtual void Function5(); //
	virtual void Function6(); //
	virtual void Function7(); //
	virtual void Function8(); //
	virtual void Function9(); //
	virtual void Function10(); //
	virtual void Function11(); //
	virtual void Function12(); //
	virtual void Function13(); //
	virtual void Function14(); //
	virtual void Function15(); //
	virtual void Function16(); //
	virtual void Function17(); //
	virtual void Function18(); //
	virtual void Function19(); //
	virtual void Function20(); //
	virtual void Function21(); //
	virtual void GetAABB(TransformAABBStruct* pAABB);
	virtual void GetTransform(Matrix* mTransform);
	virtual void Function24(); //
	virtual void Function25(); //
	virtual void Function26(); //
	virtual void Function27();
	virtual void Function28(); //
	virtual void Function29(); //
	virtual void Function30(); //
	virtual void Function31(); //
	virtual void Function32(); //
	virtual void Function33(); //
	virtual void Function34(); //
	virtual void Function35(); //
	virtual void Function36(); //
	virtual void Function37(); //
	virtual void Function38(); //
	virtual void Function39(); //
	virtual void Function40(); //
	virtual void Function41(); //
	virtual void Function42(); //
	virtual void Function43(); //
	virtual void Function44(); //
	virtual void Function45(); //
	virtual void Function46(); //
	virtual void Function47(); //
	virtual void Function48(); //
	virtual void Function49(); //
	virtual void Function50(); //
	virtual void Function51(); //
	virtual void Function52(); //
	virtual void Function53(); //
	virtual void Function54(); //
	virtual void Function55(); //
	virtual void Function56(); //
	virtual void Function57(); //
	virtual void Function58(); //
	virtual void Function59(); //
	virtual void* GetPhysiscsEntity(); //physics
	virtual Vector3* GetVelocity(); //velocity
	virtual void Function62(); //
	virtual void Function63(); //
	virtual void Function64(); //
	virtual void Function65(); //
	virtual void Function66(); //
	virtual void Function67(); //
	virtual void Function68(); //
	virtual void Function69(); //
	virtual void Function70(); //
	virtual void Function71(); //
	virtual void Function72(); //
	virtual void Function73(); //
	virtual void Function74(); //

	char _0x0000[10]; //0x0008 
	unsigned char m_OcclusionFlags; //0x0012 
	char _0x0013[13]; //0x0013 
	char _0x0020[16];
	__int64 m_Data; //0x0030 
	ComponentCollection* m_pComponents;
	char _0x0038[192];
	float m_Velocity; //0x0100 
	char _0x0104[60];
	HealthComponent* m_pHealthComp; //0x0140

	bool IsAlive();
};//Size=0x0148

class ClientVehicleEntity : public ClientControllableEntity
{
public:

};

class ClientSoldierEntity : public ClientControllableEntity
{
public:
	char _0x0148[160];
	AntAnimatableComponent* m_pAnimatable; //0x01E8 
	AntAnimatableComponent* m_pAnimatable2; //0x01F0 
	char _0x01F8[760];
	__int32 m_PoseType; //0x04F0 
	__int32 m_EngineChams; //0x04F4 
	char _0x04F8[120];
	ClientSoldierWeaponsComponent* m_pWeaponComponent; //0x0570 
	ClientSoldierBodyComponent* m_pBodyComponent; //0x0578 
	RagdollComponent* m_pRagdollComponent; //0x0580 
	char _0x0588[40];
	unsigned char m_Sprinting; //0x05B0 
	unsigned char m_Occluded; //0x05B1 
	char N00001BCE[6]; //0x05B2 
	char _0x05B8[1432];
	ParachuteComponent* m_pParachute; //0x0B50 
	char _0x0B58[152];
	ClientSpottingComponent* m_pSpottingComp; //0x0BF0 
	char _0x0BF8[24];
	ClientSpottingTargetComponent* m_pSpottingTargetComp; //0x0C10 
	char _0x0C18[88];
	SoldierSuppressionComponent* m_pSuppressionComp; //0x0C70 
	char _0x0C78[184];
	VaultComponent* m_pVaultComp; //0x0D30 
	char _0x0D38[1264];

};//Size=0x1228

class VehicleHealthZone
{
public:
	float m_MaxHealth; //0x0000
	float m_MaxShieldHealth; //0x0004
	float m_MinDamageAngle; //0x0008
	float m_DamageAngleMultiplier; //0x000C
	__int64 m_DamageAngleMultiplierCurve; //0x0010
	float m_VerticalFactor; //0x0018
	int m_UseDampingAngleCalculation; //0x001C
};//Size=0x0020

class VehicleEntityData
{
public:
	char pad_0x0000[0x148]; //0x0000
	VehicleHealthZone m_FrontHealthZone; //0x0148
	VehicleHealthZone m_RearHealthZone; //0x0168
	VehicleHealthZone m_LeftHealthZone; //0x0188
	VehicleHealthZone m_RightHealthZone; //0x01A8
	VehicleHealthZone m_TopHealthZone; //0x01C8
	char pad_0x01E8[0x60]; //0x01E8
	char* m_NameSid; //0x0248 
}; //Size=0x052C

class AntAnimatableComponent
{
public:
	char _0x0000[104];
	GameAnimatable* m_Handler; //0x0068 
	char _0x0070[16];

};//Size=0x0080

class GameAnimatable
{
public:
	char _0x0000[212];
	unsigned char m_HadVisualUpdate; //0x00D4 

};//Size=0x00D5

class AnimatedSoldierWeaponHandler
{
public:
	SoldierWeapon* m_pWeaponList[7]; //0x0000 
};//Size=0x0040

class ClientSoldierWeaponsComponent
{
public:
	char _0x0000[208];
	Matrix m_WeaponTransform; //0x00D0 
	char _0x0110[1920];
	AnimatedSoldierWeaponHandler* m_Handler; //0x0890 
	char _0x0898[48];
	ClientSoldierEntity* m_pOwner; //0x08C8 
	char _0x08D0[456];
	__int32 m_CurrentWeaponIndex; //0x0A98 
	__int32 m_LastWeaponIndex; //0x0A9C 
	__int32 m_LastGunIndex; //0x0AA0 
	char _0x0AA4[28];
	__int32 m_CurrentZoomLevel; //0x0AC0 
	__int32 m_ZoomLevelMax; //0x0AC4 
	__int32 m_ZeroingDistanceLevel; //0x0AC8 

	SoldierWeapon* GetActiveSoldierWeapon()
	{
		if (!IsValidPtr(m_Handler))
			return nullptr;

		if (m_CurrentWeaponIndex == 0 || m_CurrentWeaponIndex == 1)
			return m_Handler->m_pWeaponList[m_CurrentWeaponIndex];
		else
			return nullptr;
	};
	int GetSlot()
	{
		return m_CurrentWeaponIndex;
		//0 or 1 is primary or secondary
	}
};//Size=0x0ACC

class SoldierWeapon
{
public:
	char _0x0000[18824];
	SoldierAimingSimulation* m_pAuthoritativeAiming; //0x4988 
	char _0x4990[24];
	ClientWeapon* m_pWeapon; //0x49A8 
	char _0x49B0[16];
	WeaponFiring* m_pPrimary; //0x49C0 
	char _0x49C8[696];
};//Size=0x4C80

class SoldierAimingSimulation
{
public:
	char _0x0000[16];
	AimAssist* m_pFPSAimer; //0x0010 
	float m_Yaw; //0x0018 
	float m_Pitch; //0x001C 
	float m_AimYawTimer; //0x0020 
	float m_AimPitchTimer; //0x0024 
	Vector2 m_Sway; //0x0028 
	float m_DeltaTime; //0x0030 
	char _0x0034[8];
	Vector2 m_ViewDelta; //0x003C 
	char _0x0044[44];
	Matrix m_Transform; //0x0070 
	Vector4 m_Position; //0x00B0 
	Vector4 m_View; //0x00C0 
	Vector4 m_Velocity; //0x00D0 
	char _0x00E0[88];
	float m_Fov; //0x0138 
	char _0x013C[28];
	TypeInfo* m_RayResult; //0x0158 
	Vector4 m_RayHitPoint; //0x0160 
	__int32 m_RayLength; //0x0170 
};//Size=0x0174

class AimAssist
{
public:
	char _0x0000[20];
	Vector2 m_AimAngles;
};//Size=0x001C

class ClientWeapon
{
public:
	char N000018B4[24]; //0x0000 
	PrimaryFire* m_pWeaponFiring; //0x0018 
	WeaponModifier* m_pWeaponModifier; //0x0020 
	char _0x0028[8];
	Vector4 m_MoveSpeed; //0x0030 
	Matrix m_ShootSpace; //0x0040 
	Matrix m_ShootSpaceIdentity; //0x0080 
	char _0x00C0[464];
	float m_CameraFOV; //0x0290 
	float m_WeaponFOV; //0x0294 
	float m_FOVScaleFactor; //0x0298 
	char _0x029C[20];
	__int32 m_ZoomLevel; //0x02B0 

};//Size=0x02B4

class PrimaryFire
{
public:
	char _0x0000[16];
	FiringFunctionData* m_FiringData; //0x0010 
	char _0x0018[24];
	__int64 m_pGunSwayData; //0x0030 

};//Size=0x0038

class OverheatData
{
public:
	char _0x0000[80];
	float m_HeatPerBullet; //0x0050 
	float m_HeatDropPerSecond; //0x0054 
	float m_OverheatPenaltyTime; //0x0058 
	float m_OverheatThreshold; //0x005C 
};//Size=0x0060

class ShotConfigData
{
public:
	Vector4 m_PositionOffset; //0x0000 
	Vector4 m_Direction; //0x0010 
	Vector4 m_Speed; //0x0020 
	char _0x0030[32];
	BulletEntityData* m_ProjectileData; //0x0050 
	char _0x0058[24];
	float m_SpawnDelay; //0x0070 
	__int32 m_BulletsPerShell; //0x0074 
	__int32 m_BulletsPerShot; //0x0078 
	__int32 m_BulletsPerBurst; //0x007C 
	unsigned char m_RelativeTargetAiming; //0x0080 
	unsigned char m_ForceSpawnToCamera; //0x0081 
	unsigned char m_SpawnVisualAtWeaponBone; //0x0082 
	unsigned char m_ActiveForceSpawnToCamera; //0x0083 
	char N00001926[12]; //0x0084 
};//Size=0x0090

class FiringFunctionData
{
public:
	char _0x0000[96];
	ShotConfigData m_ShotConfigData; //0x0060 
	OverheatData m_OverHeatData; //0x00F0 
	__int64 m_FireEffects1; //0x0150 
	__int64 m_FireEffects2; //0x0158 
	__int64 m_pSound; //0x0160 
	char _0x0168[16];
};//Size=0x0178

class ProjectileEntityData
{
public:
	char _0x0000[96];
	char* m_Name; //0x0060 
	char _0x0068[40];
	float m_InitialSpeed; //0x0090 
	float m_TimeToLive; //0x0094 
	__int32 m_MaxCount; //0x0098 
	float m_InitMeshHideTime; //0x009C 
	float m_VisualConvergeDistance; //0x00A0 
	float m_VisualConvergeDelay; //0x00A4 
	float m_ProxyVisualConvergeDelay; //0x00A8 
	float m_ProxyVisualConvergeDuration; //0x00AC 
	char _0x00B0[16];
	WeaponSuppressionData* m_pSuppressionData; //0x00C0 
	char* m_AmmunitionType; //0x00C8 
	char _0x00D0[4];
	__int32 m_HitReactionWeaponType; //0x00D4 
	unsigned char m_DetonateOnTimeout; //0x00D8 
	unsigned char m_ServerProjectileDisabled; //0x00D9 
	char N0000197A[6]; //0x00DA 

};//Size=0x00E0

class MessProjectileEntityData
{
public:
	Vector4 m_AngularVelocity; //0x0000 
	char _0x0010[16]; //0x0010 
	float m_InstantAttachableTestDistance; //0x0020 
	float m_InstantAttachableVisualConvergenceDelay; //0x0024 
	float m_InstantAttachableVisualConvergenceDuration; //0x0028 
	float m_MaxAttachableInclination; //0x002C 
	float m_UnspawnAfterDetonationDelay; //0x0030 
	unsigned char m_IsAttachable; //0x0034 
	unsigned char m_InstantAttachableTestUnderReticle; //0x0035 
	unsigned char m_ExtraDamping; //0x0036 
	char N000019B5[9]; //0x0037 
};//Size=0x0040

class BulletEntityData : public ProjectileEntityData
{
public:
	MessProjectileEntityData m_MeshProjectileEntityData; //0x00E0 
	SoundAsset* m_FlyBySound; //0x0120 
	__int64 m_DudExplosion; //0x0128 
	float m_Gravity; //0x0130 
	float m_ImpactImpulse; //0x0134 
	float m_DetionationTimeVariation; //0x0138 
	float m_VehicleDetonationRadius; //0x013C 
	float m_VehicleDetonationActivationDelay; //0x0140 
	float m_FlyBySoundRadius; //0x0144 
	float m_FlyBySoundSpeed; //0x0148 
	float m_Stamina; //0x014C 
	float m_DistributeDamageOverTime; //0x0150 
	float m_StartDamage; //0x0154 
	float m_EndDamage; //0x0158 
	float m_DamageFalloffStartDist; //0x015C 
	float m_DamageFalloffEndDist; //0x0160 
	float m_TimeToArmExplosion; //0x0164 
	float m_FirstFrameTravelDistance; //0x0168 
	unsigned char m_HasVehicleDetonation; //0x016C 
	unsigned char m_InstantHit; //0x016D 
	unsigned char m_StopTrailEffectOnUnspawn; //0x016E 
	char _0x016F[1];

};//Size=0x0170

class WeaponSuppressionData
{
public:
	char _0x0000[16];
	float m_MinMultiplier; //0x0010 
	float m_MaxMultiplier; //0x0014 
	float m_MinDistance; //0x0018 
	float m_MaxDistance; //0x001C 

};//Size=0x0020

class SoundAsset
{
public:
	char _0x0000[24];
	SoundScopeData* m_Scope; //0x0018 
	SoundScopeData* m_ReferencedData; //0x0020 

};//Size=0x0028

class SoundScopeData
{
public:
	char _0x0000[16];
	char* m_Name; //0x0010 
	SoundScopeStrategyData* m_ScopeStrategyData; //0x0018 

};//Size=0x0020

class SoundScopeStrategyData
{
public:
	char _0x0000[16];
	char* m_Name; //0x0010 
	char* m_Stages; //0x0018 
	char N000019E8[32]; //0x0020 

};//Size=0x0040

class WeaponModifier
{
public:
	char _0x0000[64];
	unsigned char m_BreathControl; //0x0040 
	unsigned char m_SupportedShooting; //0x0041 
	unsigned char m_UnZoomOnBoltAction; //0x0042 
	unsigned char m_HoldBoltActionUntilZoomRelease; //0x0043 
	char _0x0044[12];
	FiringDataModifier* m_FiringDataModifier; //0x0050 
	FiringEffectsModifier* m_FiringEffectsModifier; //0x0058 
	SoundModifier* m_SoundModifier; //0x0060 
	WeaponShotModifier* m_ShotModifier; //0x0068 
	char _0x0070[56];
	AimingSimulationModifier* m_AimingSimulationModifier; //0x00A8 
	char _0x00B0[16];
	ZeroingModifier* m_ZeroingModifier; //0x00C0 
	char _0x00C8[48];
};//Size=0x0044

class WeaponFiring
{
public:
	char _0x0000[120];
	WeaponSway* m_Sway; //0x0078 
	char _0x0080[168];
	PrimaryFire* m_pPrimaryFire; //0x0128 
	char _0x0130[24];
	__int32 m_WeaponState; //0x0148 
	__int32 m_LastWeaponState; //0x014C 
	__int32 m_NextWeaponState; //0x0150 
	char _0x0154[8];
	float m_TimeToWait; //0x015C 
	float m_ReloadTimer; //0x0160 
	float m_HoldReleaseMinDelay; //0x0164 
	float m_RecoilTimer; //0x0168 
	float m_RecoilAngleX; //0x016C 
	float m_RecoilAngleY; //0x0170 
	float m_RecoilAngleZ; //0x0174 
	float m_RecoilFOVAngle; //0x0178 
	float m_RecoilTimeMultiplier; //0x017C 
	float m_OverheatDropMultiplier; //0x0180 
	char _0x0184[40];
	unsigned char m_IsNotFiring; //0x01AC 
	unsigned char m_JustShot; //0x01AD 
	unsigned char m_IsOverheated; //0x01AE 

	static WeaponFiring* GetCurrentFiringWeapon() {
		return *(WeaponFiring**)OFFSET_FIRING_WEAPON;
	}
};//Size=0x01AF

class ClientSoldierBodyComponent
{
public:
	char _0x0000[64];

};//Size=0x0040

class SkeletonAsset
{
public:
	char _0x0000[24];
	char** m_ppBoneNames; //0x0018
};//Size=0x0020

class AnimationSkeleton
{
public:
	SkeletonAsset* m_SkeletonAsset; //0x0000 
	__int32 m_BoneCount; //0x0008 
	char* GetBoneNameAt(int i)
	{
		if (i <= m_BoneCount)
			return m_SkeletonAsset->m_ppBoneNames[i];
		char t[] = "";
		return t;
	}
};//Size=0x000C

class QuatTransform
{
public:
	Vector4 m_TransAndScale; //0x0000 
	Vector4 m_Rotation; //0x0010 
};//Size=0x0020

class UpdatePoseResultData
{
public:
	enum BONES
	{
		Reference = 0,
		AITrajectory = 1,
		Trajectory = 2,
		TrajectoryEnd = 3,
		Hips = 4,
		Spine = 5,
		Spine1 = 6,
		Spine2 = 7,
		LeftShoulder = 8,
		LeftArm = 9,
		LeftArmRoll = 10,
		LeftForeArm = 11,
		LeftForeArmRoll = 12,
		LeftForeArmRoll1 = 13,
		LeftHand = 14,
		LeftHandMiddle0 = 15,
		LeftHandMiddle1 = 16,
		LeftHandMiddle2 = 17,
		LeftHandMiddle3 = 18,
		LeftHandMiddle4 = 19,
		LeftHandRing0 = 20,
		LeftHandRing1 = 21,
		LeftHandRing2 = 22,
		LeftHandRing3 = 23,
		LeftHandRing4 = 24,
		LeftHandPinky0 = 25,
		LeftHandPinky1 = 26,
		LeftHandPinky2 = 27,
		LeftHandPinky3 = 28,
		LeftHandPinky4 = 29,
		LeftHandIndex0 = 30,
		LeftHandIndex1 = 31,
		LeftHandIndex2 = 32,
		LeftHandIndex3 = 33,
		LeftHandIndex4 = 34,
		LeftHandThumb1 = 35,
		LeftHandThumb2 = 36,
		LeftHandThumb3 = 37,
		LeftHandThumb4 = 38,
		LeftHandProp1 = 39,
		LeftElbowRoll = 40,
		LeftArmCloth = 41,
		LeftShoulderPhys1 = 42,
		Neck = 43,
		Neck1 = 44,
		Head = 45,
		HeadEnd = 46,
		Face = 47,
		Jaw = 48,
		LeftLowerLip = 49,
		LeftLowerInnerLip = 50,
		LowerLip = 51,
		LowerInnerLip = 52,
		RightLowerLip = 53,
		RightLowerInnerLip = 54,
		Tongue = 55,
		TongueTip = 56,
		Chin = 57,
		LeftLowCheek = 58,
		RightLowCheek = 59,
		LeftEye = 60,
		LeftIris = 61,
		RightEye = 62,
		RightIris = 63,
		LeftUpCheek = 64,
		LeftUpInnerCheek = 65,
		RightUpInnerCheek = 66,
		RightUpCheek = 67,
		LeftCheek = 68,
		RightCheek = 69,
		LeftMouth = 70,
		LeftInnerMouth = 71,
		LeftMiddleCrease = 72,
		LeftUpperLip = 73,
		LeftUpperInnerLip = 74,
		UpperLip = 75,
		UpperInnerLip = 76,
		RightUpperLip = 77,
		RightUpperInnerLip = 78,
		RightMouth = 79,
		RightInnerMouth = 80,
		RightMiddleCrease = 81,
		LeftUpEyelid = 82,
		RightUpEyelid = 83,
		LeftLowEyelid = 84,
		RightLowEyelid = 85,
		LeftInnerEyebrow = 86,
		LeftOuterEyebrow = 87,
		RightInnerEyebrow = 88,
		RightOuterEyebrow = 89,
		LeftNose = 90,
		RightNose = 91,
		LeftCrease = 92,
		RightCrease = 93,
		LeftLowMiddleEyebrow = 94,
		RightMiddleEyebrow = 95,
		LeftLowEyelidCrease = 96,
		LeftLowOuterEyebrow = 97,
		NoseTip = 98,
		RightLowOuterEyebrow = 99,
		LeftMiddleEyebrow = 100,
		RightLowMiddleEyebrow = 101,
		RightLowEyelidCrease = 102,
		LowNose = 103,
		HeadCloth = 104,
		Throat = 105,
		Spine2Phys1 = 106,
		Spine2Prop1 = 107,
		RightShoulder = 108,
		RightArm = 109,
		RightArmRoll = 110,
		RightForeArm = 111,
		RightForeArmRoll = 112,
		RightForeArmRoll1 = 113,
		RightHand = 114,
		RightHandMiddle0 = 115,
		RightHandMiddle1 = 116,
		RightHandMiddle2 = 117,
		RightHandMiddle3 = 118,
		RightHandMiddle4 = 119,
		RightHandRing0 = 120,
		RightHandRing1 = 121,
		RightHandRing2 = 122,
		RightHandRing3 = 123,
		RightHandRing4 = 124,
		RightHandPinky0 = 125,
		RightHandPinky1 = 126,
		RightHandPinky2 = 127,
		RightHandPinky3 = 128,
		RightHandPinky4 = 129,
		RightHandIndex0 = 130,
		RightHandIndex1 = 131,
		RightHandIndex2 = 132,
		RightHandIndex3 = 133,
		RightHandIndex4 = 134,
		RightHandThumb1 = 135,
		RightHandThumb2 = 136,
		RightHandThumb3 = 137,
		RightHandThumb4 = 138,
		RightHandProp1 = 139,
		RightElbowRoll = 140,
		RightArmCloth = 141,
		RightShoulderPhys1 = 142,
		Wep_Root = 143,
		Wep_Trigger = 144,
		Wep_Slide = 145,
		Wep_Grenade1 = 146,
		Wep_Grenade2 = 147,
		Wep_Mag = 148,
		Wep_Mag_Ammo = 149,
		Wep_Mag_Extra1 = 150,
		Wep_Scope1 = 151,
		Wep_Scope2 = 152,
		Wep_Belt1 = 153,
		Wep_Belt2 = 154,
		Wep_Belt3 = 155,
		Wep_Belt4 = 156,
		Wep_Belt5 = 157,
		Wep_Belt6 = 158,
		Wep_Belt7 = 159,
		Wep_Belt8 = 160,
		Wep_Belt9 = 161,
		Wep_Belt10 = 162,
		Wep_Bipod1 = 163,
		Wep_Bipod2 = 164,
		Wep_Bipod3 = 165,
		IK_Joint_LeftHand = 166,
		IK_Joint_RightHand = 167,
		Wep_Physic1 = 168,
		Wep_Physic2 = 169,
		Wep_Physic3 = 170,
		Wep_Physic4 = 171,
		Wep_Physic5 = 172,
		Wep_Extra1 = 173,
		Wep_Extra2 = 174,
		Wep_Extra3 = 175,
		Wep_Extra4 = 176,
		Wep_Extra5 = 177,
		Wep_Aim = 178,
		LeftSpine1Phys1 = 179,
		RightSpine1Phys1 = 180,
		LeftUpLeg = 181,
		LeftUpLegRoll = 182,
		LeftLeg = 183,
		LeftFoot = 184,
		LeftToeBase = 185,
		LeftToe = 186,
		LeftLegCloth = 187,
		LeftKneeRoll = 188,
		LeftHipsRoll = 189,
		LeftUpLegCloth = 190,
		HipsCloth = 191,
		LeftHipsFrontPhys = 192,
		LeftHipsBackPhys = 193,
		LeftHipsSidePhys = 194,
		RightUpLeg = 195,
		RightUpLegRoll = 196,
		RightLeg = 197,
		RightFoot = 198,
		RightToeBase = 199,
		RightToe = 200,
		RightLegCloth = 201,
		RightKneeRoll = 202,
		RightHipsRoll = 203,
		RightUpLegCloth = 204,
		RightHipsFrontPhys = 205,
		RightHipsBackPhys = 206,
		RightHipsSidePhys = 207,
		CameraBase = 208,
		CameraJoint = 209,
		Connect = 210,
		ConnectEnd = 211,
		FacePosesAnimation = 212
	};
	QuatTransform* m_LocalTransform; //0x0000 
	QuatTransform* m_WorldTransform; //0x0008 
	QuatTransform* m_Unk; //0x0010 
	QuatTransform* m_Unk1; //0x0018 
	QuatTransform* m_Unk2; //0x0020 
	QuatTransform* m_ActiveWorldTransforms; //0x0028 
	QuatTransform* m_ActiveLocalTransforms; //0x0030 
	__int32 m_Slot; //0x0038 
	__int32 m_ReaderIndex; //0x003C 
	unsigned char m_ValidTransforms; //0x0040 
	unsigned char m_PoseUpdateEnabled; //0x0041 
	unsigned char m_PoseNeeded; //0x0042 
	char _0x0043[1];
};//Size=0x0044

class RagdollComponent
{
public:

	char _0x0000[16];
	__int64 m_pData; //0x0010 
	char _0x0018[8];
	__int64 m_pEntry; //0x0020 
	char _0x0028[32];
	__int64 m_RagdollCompData; //0x0048 
	char _0x0050[8];
	AnimationSkeleton* m_AnimationSkeleton; //0x0058 
	char _0x0060[40];
	UpdatePoseResultData m_PoseResultData; //0x0088 
	char _0x00CC[56];

	// The original author included a crashing one so I decided to rewrite it
	bool GetBone(const UpdatePoseResultData::BONES BoneId, Vector3& BoneOut)
	{
		__try
		{
			AnimationSkeleton* pSkeleton = this->m_AnimationSkeleton;
			if (!IsValidPtr(pSkeleton))
				return false;

			if (BoneId == -1)
				return false;

			UpdatePoseResultData PoseResult = this->m_PoseResultData;
			if (PoseResult.m_ValidTransforms)
			{
				QuatTransform* pQuat = PoseResult.m_ActiveWorldTransforms;
				if (!IsValidPtr(pQuat))
					return false;

				Vector4 Bone = pQuat[BoneId].m_TransAndScale;
				BoneOut = Vector3(Bone.x, Bone.y, Bone.z);
				return true;
			}
			else
			{
				return false;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}
};//Size=0x0104

class ParachuteComponent
{
public:
	char _0x0000[64];

};//Size=0x0040

class ClientSpottingComponent
{
public:
	char _0x0000[64];

};//Size=0x0040

class ClientSpottingTargetComponent
{
public:

	enum SpotType
	{
		SpotType_None,
		SpotType_Active,
		SpotType_Passive,
		SpotType_Radar,
		SpotType_Unspottable
		//....
	};

	class SpottingTargetComponentData
	{
	public:
		char pad_0x0[0x70];
		float m_ActiveSpottedTime; //+0x70
		float m_PassiveSpottedTime; //+0x74
		float m_ActiveSpottedTimeMultiplier; //+0x78
		float m_PassiveSpottedTimeMultiplier;//+0x7C
		float m_SpotOnFireMultiplier; //+0x80
		float m_RadarSpottedSpeedThresholdOverride; //+0x84
		bool m_TargetBoundingBoxCenter; //+0x88
		bool m_CalculateAngleOutsideBoundingSphere; //+0x89
	};

	char _0x0[0x10];
	SpottingTargetComponentData* m_spottingTargetData; //0x10 
	char _0x18[0x38];
	SpotType activeSpotType; //0x50
};

class VaultComponent
{
public:
	char _0x0000[64];

};//Size=0x0040

class SoldierSuppressionComponent
{
public:
	char _0x0000[64];

};//Size=0x0040

class HealthComponent
{
public:
	char _0x0000[32];
	float m_Health; //0x0020 
	float m_MaxHealth; //0x0024 
	char _0x0028[16];
	float m_VehicleHealth; //0x0038 
	float m_MaxVehicleHealth; //0x0038 
};//Size=0x003C

template <class T> class EntityIterator
{
public:
	class Element
	{
	public:

		Element* m_pFlink;
		Element* m_pBlink;
		__int32  m_Unk1;
		__int32  m_Unk2;

		T* getObject()
		{
			intptr_t pObject = reinterpret_cast<intptr_t>(this);
			pObject -= 0x40;

			return reinterpret_cast<T*>(pObject);
		}
	};

	Element* m_pFirst;
	Element* m_pCurrent;

	EntityIterator(void* pGameWorld, void* pClassInfo)
	{
		typedef Element* (__thiscall* tGetEntityList)(void* pClassInfo, void* pThis);
		tGetEntityList lpGetEntityList = reinterpret_cast<tGetEntityList>(OFFSET_GETENTITYLIST);

		this->m_pFirst = lpGetEntityList(pClassInfo, pGameWorld);
		this->m_pCurrent = this->m_pFirst;
	}

	bool hasNext()
	{
		if (m_pCurrent && m_pCurrent->m_pFlink)
			return true;

		return false;
	}

	Element* first() { return m_pFirst; }
	Element* front() { return m_pCurrent; }
	Element* next()
	{
		if (!m_pFirst)
			return nullptr;

		m_pCurrent = m_pCurrent->m_pFlink;

		return m_pCurrent;
	}
};

class WeaponShotModifier
{
public:
	char _0x0000[24];
	__int32 m_NumberOfBulletsPerShell; //0x0018 
	char _0x001C[4];
	Vector3 m_InitialSpeed; //0x0020 

};//Size=0x002C

class SoundModifier
{
public:
	char _0x0000[64];

};//Size=0x0040

class FiringEffectsModifier
{
public:
	char _0x0000[64];

};//Size=0x0040

class FiringDataModifier
{
public:
	char _0x0000[64];

};//Size=0x0040

class ProjectileModifier
{
public:
	char _0x0000[64];

};//Size=0x0040

class m_AimingSimulationModifier
{
public:
	char _0x0000[64];

};//Size=0x0040

class AimingSimulationModifier
{
public:
	char _0x0000[64];

};//Size=0x0040

class WeaponZeroingEntry
{
public:
	float m_ZeroDistance; //0x0000 
	float m_Angle; //0x0004 

	WeaponZeroingEntry(float Dist, float Angle)
	{
		m_ZeroDistance = Dist;
		m_Angle = Angle;
	}
};//Size=0x0008

class ZeroingModifier
{
public:
	char _0x0000[24];
	WeaponZeroingEntry* m_ppZeroLevels;
	float m_DefaultZeroingDistance; //0x0020 

	WeaponZeroingEntry GetZeroLevelAt(int index)
	{
		if (index > -1)
			return m_ppZeroLevels[index];
		else
			return WeaponZeroingEntry(m_DefaultZeroingDistance, 0.0f);
	}
};//Size=0x0038

class WeaponSway
{
public:
	char _0x0000[8];
	GunSwayData* m_Data; //0x0008 
};//Size=0x0010

class GunSwayData
{
public:
	char _0x0000[1088];
	float m_ShootingRecoilDecreaseScale; //0x0440 
	float m_FirstShotRecoilMultiplier; //0x0444 
	__int64 m_CamerRecoilData; //0x0448 
};//Size=0x0450

class ClientExplosionEntity
{
public:
	virtual void unkn0();
	virtual void unkn1();
	virtual void unkn2();
	virtual void unkn3();
	virtual void unkn4();
	virtual void unkn5();
	virtual void unkn6();
	virtual void unkn7();
	virtual void unkn8();
	virtual void unkn9();
	virtual void unkn10();
	virtual void unkn11();
	virtual void unkn12();
	virtual void unkn13();
	virtual void unkn14();
	virtual void unkn15();
	virtual void unkn16();
	virtual void unkn17();
	virtual void unkn18();
	virtual void unkn19();
	virtual void unkn20();
	virtual void GetAABB(TransformAABBStruct* mataabb); //22
	virtual void GetTransform(Matrix* mTransform); //23
};

template <class T>
class Tuple2
{
public:
	T m_one;
	T m_two;

public:
	Tuple2(T t_one, T t_two)
		: m_one(std::move(t_one)), m_two(std::move(t_two)) { }
};

class Color32
{
public:
	union
	{
		struct
		{
			BYTE R;
			BYTE G;
			BYTE B;
			BYTE A;
		};
		DWORD dwColor;
	};

public:
	Color32(const DWORD t_color)
		: dwColor(t_color) { }
	Color32()
		: dwColor(0) { }

	Color32(const BYTE Red, const BYTE Green, const BYTE Blue, const BYTE Alpha)
		: R(Red), G(Green), B(Blue), A(Alpha) { }
};

class DebugRenderer2
{
public:
	static DebugRenderer2* GetInstance(void)
	{
		using fb__DebugRenderManager_getThreadContext_t = DebugRenderer2 * (*)();
		auto fb__DebugRenderManager_getThreadContext = reinterpret_cast<fb__DebugRenderManager_getThreadContext_t>(OFFSET_DBRENDERER);
		return fb__DebugRenderManager_getThreadContext();
	}

	void drawText(int x, int y, Color32 color, const char* text, float scale)
	{
		using drawText_t = void(*)(DebugRenderer2*, int, int, const char*, Color32, float);
		const auto drawText = reinterpret_cast<drawText_t>(OFFSET_DRAWTEXT);
		return drawText(this, x, y, text, color, scale);
	}

	void drawText(const Vector2& t_screenpos, Color32 t_color, const char* t_text, float t_scale = 1.0f)
	{
		return drawText(static_cast<int>(t_screenpos.x), static_cast<int>(t_screenpos.y), t_color, t_text, t_scale);
	}

	void drawLine2d(float x1, float y1, float x2, float y2, Color32 color)
	{
		Tuple2<float> minpos(x1, y1);
		Tuple2<float> maxpos(x2, y2);
		using drawLine2d_t = void(*)(DebugRenderer2*, const Tuple2<float>&, const Tuple2<float>&, Color32);
		const auto drawLine2d = reinterpret_cast<drawLine2d_t>(OFFSET_DRAWLINE);
		return drawLine2d(this, minpos, maxpos, color);
	}

	void drawLine2d(const Vector2& point_1, const Vector2& point_2, Color32 color)
	{
		return drawLine2d(point_1.x, point_1.y, point_2.x, point_2.y, color);
	}

	void drawLineRect2d(float x1, float y1, float x2, float y2, Color32 color)
	{
		Tuple2< float > minpos = Tuple2< float >(x1, y1);
		Tuple2< float > maxpos = Tuple2< float >(x2, y2);
		typedef void(*tdrawLineRect2d)(DebugRenderer2*, Tuple2<float>*, Tuple2<float>*, Color32);
		tdrawLineRect2d mdrawLineRect2d = (tdrawLineRect2d)OFFSET_DRAWLINERECT;
		mdrawLineRect2d(this, &minpos, &maxpos, color);
	}

	void drawRect2d(float x1, float y1, float x2, float y2, Color32 color)
	{
		Tuple2< float > minpos = Tuple2< float >(x1, y1);
		Tuple2< float > maxpos = Tuple2< float >(x2, y2);
		typedef void(*tdrawRect2d)(DebugRenderer2*, Tuple2<float>*, Tuple2<float>*, Color32);
		tdrawRect2d mdrawRect2d = (tdrawRect2d)OFFSET_DRAWFILLRECT;
		mdrawRect2d(this, &minpos, &maxpos, color);
	}

	void drawBox2d(float x1, float y1, float x2, float y2, float width, Color32 color) {
		drawRect2d(x1, y1, x2, y1 + width, color); // Top
		drawRect2d(x1, y2, x2, y2 - width, color); // Bot
		drawRect2d(x1, y1, x1 + width, y2, color); // Left
		drawRect2d(x2, y2, x2 - width, y1, color); // Right
	}

	void drawBox2d(Vector2 from, Vector2 to, float width, Color32 color) {
		drawBox2d(from.x, from.y, to.x, to.y, width, color);
	}
};
