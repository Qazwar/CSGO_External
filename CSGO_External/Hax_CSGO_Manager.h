#pragma once
#include "Common.h"
#include "HaxData.h"
#include "HaxConfigParser.h"
#include "FastMemoryHelper.h"

#include "Hax_GlowESP.h"
#include "Hax_BunnyHop.h"
#include "Hax_TriggerBot.h"
#include "Hax_NoRecoil.h"
#include "Hax_CSGO_Misc.h"
#include "Hax_AimBot.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3x4.h"

#ifdef GetClassName
	#undef GetClassName
#endif

static enum EEntityTypes
{
	ENTITY_BY_INDEX,
	ENTITY_BY_WEAPON,
	ENTITY_LOCAL
};

class Simple_Vector2 {
	public:
		float x, y;
};

class Simple_Vector3 {
	public:
		float x, y, z;	
};

struct WorldToScreenMatrix_t {
	float flMatrix[4][4];
};

typedef struct _player_infos
{
	int64_t __pad0;
	union {
		int64_t xuid;
		struct {
			int xuidlow;
			int xuidhigh;
		};
	};
	char name[128];
	int userid;
	char guid[33];
	unsigned int friendsid;
	char friendsname[128];
	bool fakeplayer;
	bool ishltv;
	unsigned int customfiles[4];
	unsigned char filesdownloaded;
} SPlayerInfos, *PPlayerInfos;

extern class CEngine;
extern class CEntity;

class CCSGOManager : public std::enable_shared_from_this <CCSGOManager>
{
	// Common 
	public:
		CCSGOManager(DWORD dwTargetPID, const std::shared_ptr <CHaxData> & cHaxData);
		~CCSGOManager();
		
		// Routine
		bool			InitHax();
		void			OnUpdate();
		void			OnKeyChange();

		// Helpers
		auto			GetMemoryHelper()	{ return m_cMemHelper; };
		auto			GetProcessHelper()	{ return m_cProcHelper; };
		auto			GetCSGODataHelper() { return m_cHaxData; };
		auto			GetEngineHelper()	{ return m_cEngineHelper; };

		// Data
		auto			GetLocalEntity()	{ return pLocalEntity; };
		auto			GetEntitiesArray()	{ return pEntitiesArray; };

		// Proc
		auto			GetTargetPID()					{ return m_dwTargetPID; };
		auto			GetClientBase()					{ return m_dwClientBase; };
		auto			GetEngineBase()					{ return m_dwEngineBase; };
		auto			GetEnginePointer()				{ return m_dwEnginePointer; };
		auto			GetClientStateStatePointer()	{ return m_dwClientStateStatePointer; };

	private:
		// common
		DWORD			m_dwTargetPID;
		DWORD			m_dwClientBase;
		DWORD			m_dwEngineBase;
		DWORD			m_dwEnginePointer;
		DWORD			m_dwClientStateStatePointer;

		// data
		std::shared_ptr <CEntity>									pLocalEntity;
		std::array < std::shared_ptr <CEntity>, g_dwEntityCount >	pEntitiesArray;

		// helpers
		std::shared_ptr <CHaxData>			m_cHaxData;
		std::unique_ptr <CFastMemory>		m_cFastMemoryHelper;
		std::shared_ptr <CProcessHelper>	m_cProcHelper;
		std::shared_ptr <CMemoryHelper>		m_cMemHelper;
		std::shared_ptr <CEngine>			m_cEngineHelper;

		// haxs
		std::unique_ptr <CGlowESP>			m_GlowESPHelper;
		std::unique_ptr <CBunnyHop>			m_BunnyHopHelper;
		std::unique_ptr <CTriggerBot>		m_TriggerBotHelper;
		std::unique_ptr <CNoRecoil>			m_NoRecoilHelper;
		std::unique_ptr <CCSGOMisc>			m_CSGOMiscHelper;
		std::unique_ptr <CAimBot>			m_AimBotHelper;

	// Thread
	public:
		bool					CreateThread();
		bool					TerminateThread();

		void					Enable()	{ m_bDisabled = false; };
		void					Disable()	{ m_bDisabled = true; };
		bool					IsStarted() { return m_bIsStarted; };

		void					Suspend();
		void					Resume();

	protected:
		DWORD					ThreadRoutine(void);
		static DWORD WINAPI		StartThreadRoutine(LPVOID lpParam);

	private:
		HANDLE					m_hThread;
		bool					m_bDisabled;
		bool					m_bIsStarted;
		bool					m_bIsSuspended;
};

class CEngine : public std::enable_shared_from_this <CEngine>
{
	public:
		CEngine(const std::shared_ptr <CCSGOManager> & csgoManager) : 
			m_csgoManager(csgoManager), m_cMemHelper(csgoManager->GetMemoryHelper()), m_cHaxData(csgoManager->GetCSGODataHelper())
		{ }
		~CEngine(void) = default;

	public:
		bool ForceUpdate()
		{
			return m_csgoManager->GetMemoryHelper()->Write<int>
			(
				m_csgoManager->GetEnginePointer() + g_dwForceUpdate,
				-1
			);
		}

		template<typename _Ty>
		inline _Ty Random(Vector2 vRange)
		{
			if (vRange.m_flXPos == vRange.m_flYPos)
				return (_Ty)vRange.m_flXPos;

			std::random_device Random;
			std::mt19937 RandomGen(Random());
			std::uniform_real<> randomDistribution(vRange.m_flXPos, vRange.m_flYPos);

			double result = randomDistribution(RandomGen);

			if (result > vRange.m_flYPos)
				result = vRange.m_flYPos;

			if (result < vRange.m_flXPos)
				result = vRange.m_flXPos;

			return (_Ty)result;
		}

		template <typename _Ty>
		_Ty GetRandomizedValue(_Ty realValue, _Ty minValue, _Ty maxValue, bool bRandomizationEnabled = true)
		{
			auto value = realValue;
			if (bRandomizationEnabled)
				value += Random<_Ty>(Vector2(static_cast<float>(minValue), static_cast<float>(maxValue)));
			return value;
		}

		auto IsAngleNAN(Vector3 vAngle)
		{
			return (
				vAngle.m_flXPos > 89.f || vAngle.m_flXPos < -89.f ||
				vAngle.m_flYPos > 180.f || vAngle.m_flYPos < -180.f ||
				vAngle.m_flZPos != 0.f ||
				(!std::isfinite(vAngle.m_flXPos) || std::isnan(vAngle.m_flXPos) ||
				(!std::isfinite(vAngle.m_flYPos) || std::isnan(vAngle.m_flYPos)))
			);
		}

		auto AngleNormalizeClamp(Vector3 & vAngle)
		{
			vAngle.m_flXPos = clamp(std::remainder(vAngle.m_flXPos, 180.f), -89.f, 89.f);
			vAngle.m_flYPos = clamp(std::remainder(vAngle.m_flYPos, 360.f), -180.f, 180.f);
			vAngle.m_flZPos = 0.f;
		}

		auto CalculatePunchReduction(Vector2 vPitch, Vector2 vYaw)
		{
			return Vector3(Random<float>(vPitch), Random<float>(vYaw), 0.f);
		}

		auto CalculateSmoothAngle(Vector3 vSrcAngle, Vector3 vDstAngle, float smoothAmount)
		{
			smoothAmount = clamp(smoothAmount, 0.f, 100.f);

			Vector3 vSmoothAngle(0.f);
			vSmoothAngle = vSrcAngle + ((vDstAngle - vSrcAngle) * (Vector3(smoothAmount) / Vector3(100.f)));
			vSmoothAngle.m_flZPos = 0.f;

			AngleNormalizeClamp(vSmoothAngle);

			return vSmoothAngle;
		}

		auto CalculateAngle(Vector3 vSrcPosition, Vector3 vDstPosition)
		{
			auto vAimAngle = (vDstPosition - vSrcPosition).ToAngle();
			AngleNormalizeClamp(vAimAngle);
			return vAimAngle;
		}

		auto IsInFOV(Vector3 vSrcPosition, Vector3 vDstPosition, float flInFOV, float & flFOV)
		{
			auto vAimAngle = CalculateAngle(vSrcPosition, vDstPosition);
			auto vViewAngle = GetViewAngle();
			auto vDelta = vViewAngle - vAimAngle;

			auto vFOV = Vector3(sin(M_DEG2RAD(vDelta.m_flXPos)), sin(M_DEG2RAD(vDelta.m_flYPos)), 0) * Vector3(vSrcPosition.Distance(vDstPosition));

			flFOV = vFOV.Length();

			if (std::isnan(flFOV))
			{
				flFOV = 360.f;
			}

			return flFOV < flInFOV;
		}

		Vector3 AngleToDirection(Vector3 angle)
		{
			// Convert angle to radians 
			angle.m_flXPos = (float)M_DEG2RAD(angle.m_flXPos);
			angle.m_flYPos = (float)M_DEG2RAD(angle.m_flYPos);

			float sinYaw = sin(angle.m_flYPos);
			float cosYaw = cos(angle.m_flYPos);

			float sinPitch = sin(angle.m_flXPos);
			float cosPitch = cos(angle.m_flXPos);

			Vector3 direction;
			direction.m_flXPos = cosPitch * cosYaw;
			direction.m_flYPos = cosPitch * sinYaw;
			direction.m_flZPos = -sinPitch;

			return direction;
		}

		float DotProduct(Vector3 &v1, float* v2)
		{
			return v1.m_flXPos * v2[0] + v1.m_flYPos * v2[1] + v1.m_flZPos * v2[2];
		}
		void VectorTransform(Vector3 &in1, Matrix3x4& in2, Vector3 &out)
		{
			out.m_flXPos = DotProduct(in1, in2.m[0]) + in2.m[0][3];
			out.m_flYPos = DotProduct(in1, in2.m[1]) + in2.m[1][3];
			out.m_flZPos = DotProduct(in1, in2.m[2]) + in2.m[2][3];
		}

		// pTarget = ClientCMD pointer;
		void SendClientCMD(LPVOID pTarget, const char* c_szCommand)
		{
			m_cMemHelper->LoadShellCode(pTarget, -1, (LPVOID)c_szCommand, strlen(c_szCommand) + 1);
		}

		Vector3 GetViewAngle()
		{
			return m_cMemHelper->Read<Vector3>
			(
				m_cMemHelper->Read<DWORD>(m_csgoManager->GetEngineBase() + m_cHaxData->GetCSGODataInstance()->GetClientState_Pointer()) +
				m_cHaxData->GetCSGODataInstance()->GetClientStateViewAngles_Pointer()
			);
		}

		auto SetViewAngle(Vector3 vAngle)
		{
			AngleNormalizeClamp (vAngle);
			m_cMemHelper->Write<Vector3>
			(
				m_cMemHelper->Read<DWORD>(m_csgoManager->GetEngineBase() + m_cHaxData->GetCSGODataInstance()->GetClientState_Pointer()) + m_cHaxData->GetCSGODataInstance()->GetClientStateViewAngles_Pointer(),
				vAngle
			);
		}

		auto GetGameState()
		{
			auto iState = m_cMemHelper->Read<int>(m_csgoManager->GetEnginePointer() + m_cHaxData->GetCSGODataInstance()->GetClientStateState_Pointer());
			return iState;
		}

		auto IsInGame()
		{
			auto bIngame = GetGameState() == SIGNONSTATE_FULL;
			return bIngame;
		}

		auto IsMouseEnabled()
		{
			if (!m_cMemHelper->Read<bool>(m_csgoManager->GetClientBase() + m_cHaxData->GetCSGODataInstance()->GetInput_Pointer() + 0x6) ||
				!(m_cMemHelper->Read<byte>(m_csgoManager->GetClientBase() + m_cHaxData->GetCSGODataInstance()->GetMouseEnable_Pointer()) & 1))
			{
				return true;
			}
			return false;
		}

		auto GetSensitivity()
		{
			return m_cMemHelper->Read<float>(m_csgoManager->GetClientBase() + m_cHaxData->GetCSGODataInstance()->GetSensitivity_Pointer());
		}

		auto SetSensitivity(float flNewValue)
		{
			return m_cMemHelper->Write<float>(m_csgoManager->GetClientBase() + m_cHaxData->GetCSGODataInstance()->GetSensitivity_Pointer(), flNewValue);
		}

		auto SetSendPackets(bool bValue)
		{
			return m_cMemHelper->Write<bool>(m_csgoManager->GetEngineBase() + m_cHaxData->GetCSGODataInstance()->GetSendPackets_Pointer(), bValue);
		}

		auto GetGlobalVars(void) const
		{
			return m_cMemHelper->Read<DWORD>
			(
				m_csgoManager->GetEngineBase() + 
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlobalVars_Pointer()
			);
		}

		auto GetMaxClients(void) const
		{
			// FIXME
			auto dwGlobalVars = GetGlobalVars();
			if (dwGlobalVars)
				return m_cMemHelper->Read<int>(dwGlobalVars + 0x18);
			return 0;
		}

		auto GetTickCount(void) const
		{
			auto dwGlobalVars = GetGlobalVars();
			if (dwGlobalVars)
				return m_cMemHelper->Read<int>(dwGlobalVars + 0x1C);
			return 0;
		}

		auto GetIntervalPerTick(void) const
		{
			auto dwGlobalVars = GetGlobalVars();
			if (dwGlobalVars)
				return m_cMemHelper->Read<float>(dwGlobalVars + 0x20);
			return 0.f;
		}

		auto GetAttack(void) const
		{		
			auto iForceAttackRet = m_cMemHelper->Read<int>
			(
				m_csgoManager->GetClientBase() + 
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetForceAttack_Pointer()
			);
			return iForceAttackRet == 5;
		}

		auto GetPlayerResource(void) const
		{
			return m_cMemHelper->Read<DWORD>
			(
				m_csgoManager->GetClientBase() + 
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetPlayerResource_Pointer()
			);
		}

		auto GetC4Host(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<int>
			(
				GetPlayerResource() +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetPlayerC4_Netvar()
			);
		}

		auto WorldToScreen(HWND hWnd, Simple_Vector3 ThreeDCoordinates, Simple_Vector2 & TwoDCoordinates)
		{
			RECT rc;
			if (!GetWindowRect(hWnd, &rc))
				return false;

			WorldToScreenMatrix_t WorldToScreenMatrix;
			WorldToScreenMatrix = m_csgoManager->GetMemoryHelper()->Read<WorldToScreenMatrix_t>(m_csgoManager->GetClientBase() + m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetViewMatrix_Pointer());
			float w = 0.0f;

			TwoDCoordinates.x = WorldToScreenMatrix.flMatrix[0][0] * ThreeDCoordinates.x + WorldToScreenMatrix.flMatrix[0][1] * ThreeDCoordinates.y + WorldToScreenMatrix.flMatrix[0][2] * ThreeDCoordinates.z + WorldToScreenMatrix.flMatrix[0][3];
			TwoDCoordinates.y = WorldToScreenMatrix.flMatrix[1][0] * ThreeDCoordinates.x + WorldToScreenMatrix.flMatrix[1][1] * ThreeDCoordinates.y + WorldToScreenMatrix.flMatrix[1][2] * ThreeDCoordinates.z + WorldToScreenMatrix.flMatrix[1][3];
			w = WorldToScreenMatrix.flMatrix[3][0] * ThreeDCoordinates.x + WorldToScreenMatrix.flMatrix[3][1] * ThreeDCoordinates.y + WorldToScreenMatrix.flMatrix[3][2] * ThreeDCoordinates.z + WorldToScreenMatrix.flMatrix[3][3];

			if (w < 0.01f)
				return false;

			float invw = 1.0f / w;
			TwoDCoordinates.x *= invw;
			TwoDCoordinates.y *= invw;

			int width = (int)(rc.right - rc.left);
			int height = (int)(rc.bottom - rc.top);

			float x = float(width / 2);
			float y = float(height / 2);

			x += float(0.5 * TwoDCoordinates.x * width + 0.5);
			y -= float(0.5 * TwoDCoordinates.y * height + 0.5);

			TwoDCoordinates.x = x + rc.left;
			TwoDCoordinates.y = y + rc.top;

			return true;
		}

		double GetDistanceFromCrosshair(Vector2 EntityCoordinates)
		{
			Simple_Vector3 myscreen = { (float)GetSystemMetrics(SM_CXSCREEN) / 2, (float)GetSystemMetrics(SM_CYSCREEN) / 2, 0 };
			return sqrt(pow(double(EntityCoordinates.GetX() - myscreen.x), 2.0) + pow(double(EntityCoordinates.GetY() - myscreen.y), 2.0));
		}

	private:
		std::shared_ptr <CCSGOManager>		m_csgoManager;
		std::shared_ptr <CMemoryHelper>		m_cMemHelper;
		std::shared_ptr <CHaxData>			m_cHaxData;
};

class CEntity
{
	public:
		CEntity(const std::shared_ptr <CCSGOManager> & csgoManager) :
			m_csgoManager(csgoManager), m_pBaseEntity(0), m_iEntityIndex(0),
			m_dwClientBase((csgoManager && csgoManager->GetProcessHelper() && m_csgoManager->GetProcessHelper()->GetModule(XOR("client.dll")).IsValid()) ? (DWORD)m_csgoManager->GetProcessHelper()->GetModule("client.dll").GetAddress() : 0)
		{
		}
		~CEntity(void) = default;

		auto SetEntityBase(DWORD dwBase)
		{
			m_pBaseEntity = dwBase;
		}
		auto GetEntityBase() const
		{
			return m_pBaseEntity;
		}

		auto Update(EEntityTypes entityType, int iEntityIndex = 0)
		{
			m_iEntityIndex = iEntityIndex;

			auto dwTargetAddress = 0;
			if (entityType == EEntityTypes::ENTITY_BY_INDEX)
				dwTargetAddress = m_dwClientBase + m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetEntityList_Pointer() + (iEntityIndex * 0x10);
			else if (entityType == EEntityTypes::ENTITY_BY_WEAPON)
				dwTargetAddress = m_dwClientBase + m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetEntityList_Pointer() + (-0x10 + (iEntityIndex * 0x10));
			else /*	 entityType == EEntityTypes::ENTITY_LOCAL */
				dwTargetAddress = m_dwClientBase + m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetLocalPlayer_Pointer();

			m_pBaseEntity = m_csgoManager->GetMemoryHelper()->Read<DWORD>(dwTargetAddress);
			return m_pBaseEntity != 0;
		}

		auto UpdateByHandle(EEntityTypes entityType, DWORD dwHandle)
		{
			return Update(entityType, dwHandle & 0x7FFF);
		}

		auto GetLoopIndex()
		{
			return m_iEntityIndex;
		}

		auto GetIndex(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<int>(m_pBaseEntity + 0x64);
		}

		auto GetPlayerInfo(PPlayerInfos pPlayerInfo_t) const
		{
			// FIXME
			return false;

			/*
			SPlayerInfos playerInfos;
			auto bGetPlayerInfos = m_pCurrentEntity->GetPlayerInfo(&playerInfos);
			if (bGetPlayerInfos)
			DEBUG_LOG(LL_SYS, "%d-%d) id: %d name: %s fake: %d guid: %s", i, m_pCurrentEntity->GetIndex(), playerInfos.userid, playerInfos.name, playerInfos.fakeplayer, playerInfos.guid)
			else
			DEBUG_LOG(LL_SYS, "player infos can not fetch");
			*/

			if (pPlayerInfo_t)
			{
				SPlayerInfos PlayerInfo_t;
				auto ECX = m_csgoManager->GetMemoryHelper()->Read<DWORD_PTR>(m_csgoManager->GetEnginePointer() + m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetPlayerInfo_Pointer());
				if (ECX)
				{
					auto EDX = m_csgoManager->GetMemoryHelper()->Read<DWORD_PTR>(ECX + 0x40); // stringtable
					if (EDX)
					{
						auto EAX = m_csgoManager->GetMemoryHelper()->Read<DWORD_PTR>(EDX + 0xC); // pointer to array of player info classes
						if (EAX)
						{
							auto iPlayerIndex = GetIndex();
							if (iPlayerIndex > 0 && iPlayerIndex <= g_dwEntityCount)
							{
								auto infoObj = m_csgoManager->GetMemoryHelper()->ReadAddress(LPVOID(EAX + 0x28 + (iPlayerIndex * 0x34)), &PlayerInfo_t, sizeof(PlayerInfo_t)); // first playerinfo is at 0x28
								if (infoObj)
								{
									*pPlayerInfo_t = PlayerInfo_t;
									return true;
								}
							}
						}
					}
				}
			}
			return false;
		}
		
		// Get methods
		auto GetFlags(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<int>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetFlags_Netvar()
			);
		}

		auto GetGlowIndex(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<int>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlowIndex_Netvar()
			);
		}

		auto GetClassID(void) const
		{
			auto a = m_csgoManager->GetMemoryHelper()->Read<int>(m_pBaseEntity + 0x8);
			auto b = m_csgoManager->GetMemoryHelper()->Read<int>(a + 0x2 * 0x4);
			auto c = m_csgoManager->GetMemoryHelper()->Read<int>(b + 0x1);
			auto d = m_csgoManager->GetMemoryHelper()->Read<int>(c + 0x14);
			return d;
		}

		auto GetLifeState(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<int>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetLifeState_Netvar()
			);
		}

		auto GetCrossHairID(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<int>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetCrossHairID_Netvar()
			) - 1;
		}

		auto GetAimPunch(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<Vector3>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetAimPunch_Netvar()
			);
		}

		auto GetTeamNumber(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<int>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTeamNum_Netvar()
			);
		}

		auto GetHealth(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<int>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetHealth_Netvar()
			);
		}

		auto GetFlashMaxAlpha(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<float>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetFlashMaxAlpha_Netvar()
			);
		}

		auto GetWeaponHandle(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<DWORD>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetActiveWeapon_Netvar()
			);
		}

		auto GetWeaponEntity(void) const
		{
			auto hActiveWeaponHandle = GetWeaponHandle();
			if (hActiveWeaponHandle)
			{
				auto dwWeaponEntity = hActiveWeaponHandle & 0xFFF;
				if (dwWeaponEntity)
					return dwWeaponEntity;
			}
			return DWORD(0);
		}

		auto GetWeaponBase(void) const
		{
			auto dwWeaponEntity = GetWeaponEntity();
			if (dwWeaponEntity)
			{
				auto pCurrentEntity = std::make_unique<CEntity>(m_csgoManager);
				if (pCurrentEntity && pCurrentEntity.get())
				{
					if (pCurrentEntity->Update(EEntityTypes::ENTITY_BY_WEAPON, dwWeaponEntity))
					{
						return pCurrentEntity->GetEntityBase();
					}
				}
			}		
			return DWORD(0);
		}

		auto GetWeaponIndex(void) const
		{
			auto dwWeaponBase = GetWeaponBase();
			if (dwWeaponBase)
			{
				return m_csgoManager->GetMemoryHelper()->Read<int>
				(
					dwWeaponBase +
					m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetItemDefinitionIndex_NetVar()
				);
			}
			return 0;
		}
		
		auto GetWeaponID(void) const
		{
			auto dwWeaponBase = GetWeaponBase();
			if (dwWeaponBase)
			{
				return m_csgoManager->GetMemoryHelper()->Read<int>
				(
					dwWeaponBase +
					m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetAttributeManager_NetVar() + 
					m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetItem_NetVar() +
					m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetItemDefinitionIndex_NetVar()
				);
			}
			return 0;
		}

		auto GetZoomLevel(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<int>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetZoomLevel_NetVar()
			);
		}

		auto GetFov(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<int>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetWorldModelIndex_NetVar()
			);
		}

		auto GetShotsFired(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<int>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetShotsFired_Netvar()
			);
		}
		
		auto GetAmmo(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<int>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetClip1_NetVar()
			);
		}

		auto GetNextPrimaryAttack(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<float>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetNextPrimaryAttack_NetVar()
			);
		}

		auto GetTick(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<int>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTickBase_Netvar() +
				(int)m_csgoManager->GetEngineHelper()->GetIntervalPerTick()
			);
		}

		auto GetVelocity(void) const
		{	
			return m_csgoManager->GetMemoryHelper()->Read<Vector3>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetVelocity_Netvar()
			);
		}

		auto GetClassName(void) const
		{
			DWORD PlayerBaseVT = m_csgoManager->GetMemoryHelper()->Read<DWORD>(m_pBaseEntity + 0x8);
			if (PlayerBaseVT)
			{
				DWORD PlayerBaseFN = m_csgoManager->GetMemoryHelper()->Read<DWORD>(PlayerBaseVT + 2 * 0x4);
				if (PlayerBaseFN)
				{
					DWORD PlayerBaseCLS = m_csgoManager->GetMemoryHelper()->Read<DWORD>(PlayerBaseFN + 1);
					if (PlayerBaseCLS)
					{
						DWORD PlayerBaseCLSName = m_csgoManager->GetMemoryHelper()->Read<DWORD>(PlayerBaseCLS + 8);
						if (PlayerBaseCLSName)
						{
							char szClassName[32] = "\0";
							for (int i = 0; i < 32; i++)
							{
								szClassName[i] = m_csgoManager->GetMemoryHelper()->Read<char>(PlayerBaseCLSName + i * sizeof(char));
								if (!szClassName[i])
									break;
							}
							return std::string(szClassName);
						}
					}
				}
			}
			return std::string("");
		}

		auto GetPosition(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<Vector3>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetVecOrigin_Netvar()
			);
		}

		auto GetViewOffset(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<Vector3>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetViewOffset_Netvar()
			);
		}

		auto GetEyePosition(void) const
		{
			return GetPosition() + GetViewOffset();
		}

		auto GetBoneMatrix(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<DWORD>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetBoneMatrix_Netvar()
			);
		}

		auto GetBonePosition(CSPlayerBones eBone) const
		{
			auto dwBoneMatrix = GetBoneMatrix();
			return Vector3(
				m_csgoManager->GetMemoryHelper()->Read<float>(dwBoneMatrix + 0x30 * eBone + 0x0C),
				m_csgoManager->GetMemoryHelper()->Read<float>(dwBoneMatrix + 0x30 * eBone + 0x1C),
				m_csgoManager->GetMemoryHelper()->Read<float>(dwBoneMatrix + 0x30 * eBone + 0x2C)
			);
		}

		auto GetBoneMatrix3x4(void) const
		{
			auto dwBoneMatrix = GetBoneMatrix();

			Matrix3x4 m_BoneMatrix[128];
			memset(m_BoneMatrix, NULL, sizeof(Matrix3x4[128]));

			m_csgoManager->GetMemoryHelper()->ReadAddress((LPVOID)dwBoneMatrix, m_BoneMatrix, sizeof(Matrix3x4[128]));
			return m_BoneMatrix;
		}

		auto GetBoneMatrix3x4FromBone(int iBone) const
		{
			Matrix3x4 boneMatrix[128];
			memset(boneMatrix, NULL, sizeof(Matrix3x4[128]));

			memcpy(boneMatrix, GetBoneMatrix3x4(), sizeof(boneMatrix));

			return boneMatrix[iBone];
		}

		// Set Methods
		auto SetFlashMaxAlpha(float fDuration)
		{
			m_csgoManager->GetMemoryHelper()->Write<float>
			(
				m_pBaseEntity + m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetFlashMaxAlpha_Netvar(),
				fDuration
			);
		}

		auto SetModelIndex(int iNewModelIndex)
		{
			m_csgoManager->GetMemoryHelper()->Write<int>
			(
				m_pBaseEntity + m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetModelIndex_Netvar(),
				iNewModelIndex
			);
		}

		auto SendForceAttack(bool bPreDelay = true, DWORD dwDelay = 10UL, bool bSilent = true)
		{
			if (bSilent)
				m_csgoManager->GetEngineHelper()->SetSendPackets(false);

			if (bPreDelay)
				Sleep(10);

			m_csgoManager->GetMemoryHelper()->Write<int>(m_csgoManager->GetClientBase() + m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetForceAttack_Pointer(), 1);
			Sleep(dwDelay);
			m_csgoManager->GetMemoryHelper()->Write<int>(m_csgoManager->GetClientBase() + m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetForceAttack_Pointer(), 0);

			if (bSilent)
				m_csgoManager->GetEngineHelper()->SetSendPackets(true);
		}

		auto SetFov(int iNewValue)
		{
			m_csgoManager->GetMemoryHelper()->Write<int>
			(
				m_pBaseEntity + m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetWorldModelIndex_NetVar(),
				iNewValue
			);
		}

		auto SetSpotted(bool bNewValue)
		{
			m_csgoManager->GetMemoryHelper()->Write<bool>
			(
				m_pBaseEntity + m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetSpotted_Netvar(),
				bNewValue
			);
		}

		auto SetVisualPunch(Vector3 vPunch)
		{		
			m_csgoManager->GetMemoryHelper()->Write<Vector3>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetViewPunchAngle_NetVar(),

				vPunch
			);
		}

		auto SetClrBase(Vector3 vColor)
		{
			m_csgoManager->GetMemoryHelper()->Write<Vector3>
			(
				m_pBaseEntity + m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetClrRender_Netvar(),
				vColor
			);
		}


		// Is Methods
		auto IsDormant(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<bool>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetDormant_Pointer()
			);
		}

		auto IsSpotted(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<bool>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetSpotted_Netvar()
			);
		}

		auto IsSpottedByMask(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<bool>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetSpottedByMask_Netvar()
			);
		}

		auto IsShotsFired(void) const
		{
			return GetShotsFired() >= 0;
		}

		auto IsScoped(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<bool>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetIsScoped_NetVar()
			);
		}

		auto IsAlive_State(void) const
		{
			return GetLifeState() == LIFE_ALIVE;
		}

		auto IsAlive_Hp(void) const
		{
			return GetHealth() > 0;
		}

		auto IsSpawnProtected(void) const
		{
			return m_csgoManager->GetMemoryHelper()->Read<bool>
			(
				m_pBaseEntity +
				m_csgoManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGunGameImmunity_Netvar()
			);
		}

		auto IsJumping(void) const
		{
			return GetFlags() & IN_JUMP;
		}

		auto IsZooming(void) const
		{
			return GetFlags() & IN_ZOOM;
		}

		auto IsMoving(void) const
		{
			auto vVelocity = GetVelocity();
			return !(vVelocity == Vector3(0, 0, 0));
		}

		auto IsPlayer(void) const
		{
			return GetClassID() == ClassId_CCSPlayer;
		}

		auto IsValidPlayer(void) const
		{
			if (!m_pBaseEntity)
				return false;

			if (GetHealth() < 1)
				return false;

			if (IsDormant())
				return false;

			return true;
		}

		auto OnGround(void) const 
		{
			return GetFlags() & FL_ONGROUND;
		}

		auto CanAttack(void) const
		{
			if (!IsAlive_Hp() || IsDormant())
				return false;

			auto dwWeaponEntity = GetWeaponEntity();
			if (!dwWeaponEntity)
				return false;
			
			auto pCurrentEntity = std::make_unique<CEntity>(m_csgoManager);
			if (!pCurrentEntity || !pCurrentEntity.get())
				return false;
			
			if (!pCurrentEntity->Update(EEntityTypes::ENTITY_BY_WEAPON, dwWeaponEntity))
				return false;
			
			if (pCurrentEntity->GetAmmo() <= 0)
				return false;

			if (std::floor(pCurrentEntity->GetNextPrimaryAttack()) - GetTick() > 0)
				return false;

			return true;
		}

		// Weapon checks
		auto WeaponIsPistol(int _iWeaponID = 0) const
		{
			auto iWeaponID = _iWeaponID;
			if (!iWeaponID) iWeaponID = GetWeaponID();
			switch (iWeaponID)
			{
				case ItemDefinitionIndex::WEAPON_DEAGLE:
				case ItemDefinitionIndex::WEAPON_ELITE:
				case ItemDefinitionIndex::WEAPON_FIVESEVEN:
				case ItemDefinitionIndex::WEAPON_TEC9:
				case ItemDefinitionIndex::WEAPON_GLOCK:
				case ItemDefinitionIndex::WEAPON_P250:
				case ItemDefinitionIndex::WEAPON_HKP2000:
				case ItemDefinitionIndex::WEAPON_USP_SILENCER:
				//case ItemDefinitionIndex::WEAPON_REVOLVER:
				//case ItemDefinitionIndex::WEAPON_CZ75A:
					return true;
			}
			return false;
		}

		auto WeaponIsSMG(int _iWeaponID = 0) const
		{
			auto iWeaponID = _iWeaponID;
			if (!iWeaponID) iWeaponID = GetWeaponID();
			switch (iWeaponID)
			{
				case ItemDefinitionIndex::WEAPON_BIZON:
				case ItemDefinitionIndex::WEAPON_MAC10:
				case ItemDefinitionIndex::WEAPON_P90:
				case ItemDefinitionIndex::WEAPON_UMP45:
				case ItemDefinitionIndex::WEAPON_MP7:
				case ItemDefinitionIndex::WEAPON_MP9:
					return true;
			}
			return false;
		}

		auto WeaponIsRifle(int _iWeaponID = 0) const
		{
			auto iWeaponID = _iWeaponID;
			if (!iWeaponID) iWeaponID = GetWeaponID();
			switch (iWeaponID)
			{
				case ItemDefinitionIndex::WEAPON_AK47:
				case ItemDefinitionIndex::WEAPON_AUG:
				case ItemDefinitionIndex::WEAPON_M4A1:
				case ItemDefinitionIndex::WEAPON_M4A1_SILENCER:
				case ItemDefinitionIndex::WEAPON_FAMAS:
				case ItemDefinitionIndex::WEAPON_GALILAR:
				case ItemDefinitionIndex::WEAPON_SG556:
					return true;
			}
			return false;
		}

		auto WeaponIsSniper(int _iWeaponID = 0) const
		{
			auto iWeaponID = _iWeaponID;
			if (!iWeaponID) iWeaponID = GetWeaponID();
			switch (iWeaponID)
			{
				case ItemDefinitionIndex::WEAPON_AWP:
				case ItemDefinitionIndex::WEAPON_SSG08:
					return true;
			}
			return false;
		}

		auto WeaponIsAutoSniper(int _iWeaponID = 0) const
		{
			auto iWeaponID = _iWeaponID;
			if (!iWeaponID) iWeaponID = GetWeaponID();
			switch (iWeaponID)
			{
				case ItemDefinitionIndex::WEAPON_G3SG1:
				case ItemDefinitionIndex::WEAPON_SCAR20:
					return true;
			}
			return false;
		}

		auto WeaponIsShotgun(int _iWeaponID = 0) const
		{
			auto iWeaponID = _iWeaponID;
			if (!iWeaponID) iWeaponID = GetWeaponID();
			switch (iWeaponID)
			{
				case ItemDefinitionIndex::WEAPON_XM1014:
				case ItemDefinitionIndex::WEAPON_NOVA:
				case ItemDefinitionIndex::WEAPON_SAWEDOFF:
				case ItemDefinitionIndex::WEAPON_MAG7:
					return true;
			}
			return false;
		}
		
		auto WeaponIsMG(int _iWeaponID = 0) const
		{
			auto iWeaponID = _iWeaponID;
			if (!iWeaponID) iWeaponID = GetWeaponID();
			switch (iWeaponID)
			{
				case ItemDefinitionIndex::WEAPON_M249:
				case ItemDefinitionIndex::WEAPON_NEGEV:
					return true;
			}
			return false;
		}

		auto WeaponIsKnife(int _iWeaponID = 0) const
		{
			auto iWeaponID = _iWeaponID;
			if (!iWeaponID) iWeaponID = GetWeaponID();
			switch (iWeaponID)
			{
				case ItemDefinitionIndex::WEAPON_KNIFE:
				case ItemDefinitionIndex::WEAPON_KNIFE_T:
				case ItemDefinitionIndex::WEAPON_KNIFE_BAYONET:
				case ItemDefinitionIndex::WEAPON_KNIFE_GUT:
				case ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL:
				case ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT:
				case ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET:
				case ItemDefinitionIndex::WEAPON_KNIFE_FLIP:
				case ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY:
				case ItemDefinitionIndex::WEAPON_KNIFE_FALCHION:
				case ItemDefinitionIndex::WEAPON_KNIFE_SURVIVAL_BOWIE:
				case ItemDefinitionIndex::WEAPON_KNIFE_PUSH:
					return true;
			}
			return false;
		}

		auto WeaponIsMisc(int _iWeaponID = 0) const
		{
			auto iWeaponID = _iWeaponID;
			if (!iWeaponID) iWeaponID = GetWeaponID();
			switch (iWeaponID)
			{
				case ItemDefinitionIndex::WEAPON_FLASHBANG:
				case ItemDefinitionIndex::WEAPON_HEGRENADE:
				case ItemDefinitionIndex::WEAPON_SMOKEGRENADE:
				case ItemDefinitionIndex::WEAPON_MOLOTOV:
				case ItemDefinitionIndex::WEAPON_DECOY:
				case ItemDefinitionIndex::WEAPON_INCGRENADE:
				case ItemDefinitionIndex::WEAPON_C4:
					return true;
			}
			return false;
		}

		auto WeaponCanApplyRCS(int _iWeaponID = 0) const
		{
			auto iWeaponID = _iWeaponID;
			if (!iWeaponID) iWeaponID = GetWeaponID();
			switch (iWeaponID)
			{
				case ItemDefinitionIndex::WEAPON_BIZON:
				case ItemDefinitionIndex::WEAPON_MAC10:
				case ItemDefinitionIndex::WEAPON_P90:
				case ItemDefinitionIndex::WEAPON_UMP45:
				case ItemDefinitionIndex::WEAPON_MP7:
				case ItemDefinitionIndex::WEAPON_MP9:
				case ItemDefinitionIndex::WEAPON_AK47:
				case ItemDefinitionIndex::WEAPON_AUG:
				case ItemDefinitionIndex::WEAPON_M4A1:
				case ItemDefinitionIndex::WEAPON_M4A1_SILENCER:
				case ItemDefinitionIndex::WEAPON_FAMAS:
				case ItemDefinitionIndex::WEAPON_GALILAR:
				case ItemDefinitionIndex::WEAPON_SG556:
				case ItemDefinitionIndex::WEAPON_M249:
				case ItemDefinitionIndex::WEAPON_NEGEV:
					return true;
			}
			return false;
		}

		auto WeaponCanBurst(int _iWeaponID = 0) const
		{
			auto iWeaponID = _iWeaponID;
			if (!iWeaponID) iWeaponID = GetWeaponID();
			switch (iWeaponID)
			{
				case ItemDefinitionIndex::WEAPON_CZ75A:
				case ItemDefinitionIndex::WEAPON_BIZON:
				case ItemDefinitionIndex::WEAPON_MAC10:
				case ItemDefinitionIndex::WEAPON_P90:
				case ItemDefinitionIndex::WEAPON_UMP45:
				case ItemDefinitionIndex::WEAPON_MP7:
				case ItemDefinitionIndex::WEAPON_MP9:
				case ItemDefinitionIndex::WEAPON_AK47:
				case ItemDefinitionIndex::WEAPON_AUG:
				case ItemDefinitionIndex::WEAPON_M4A1:
				case ItemDefinitionIndex::WEAPON_M4A1_SILENCER:
				case ItemDefinitionIndex::WEAPON_FAMAS:
				case ItemDefinitionIndex::WEAPON_GALILAR:
				case ItemDefinitionIndex::WEAPON_SG556:
				case ItemDefinitionIndex::WEAPON_M249:
				case ItemDefinitionIndex::WEAPON_NEGEV:
					return true;
			}
			return false;
		}

		auto WeaponIsShootable(int _iWeaponID = 0) const
		{
			auto iWeaponID = _iWeaponID;
			if (!iWeaponID) iWeaponID = GetWeaponID();
			switch (iWeaponID)
			{
				case 0:
				case ItemDefinitionIndex::WEAPON_KNIFE:
				case ItemDefinitionIndex::WEAPON_KNIFE_T:
				case ItemDefinitionIndex::WEAPON_KNIFE_BAYONET:
				case ItemDefinitionIndex::WEAPON_KNIFE_GUT:
				case ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL:
				case ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT:
				case ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET:
				case ItemDefinitionIndex::WEAPON_KNIFE_FLIP:
				case ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY:
				case ItemDefinitionIndex::WEAPON_KNIFE_FALCHION:
				case ItemDefinitionIndex::WEAPON_KNIFE_SURVIVAL_BOWIE:
				case ItemDefinitionIndex::WEAPON_KNIFE_PUSH:
				case ItemDefinitionIndex::WEAPON_FLASHBANG:
				case ItemDefinitionIndex::WEAPON_HEGRENADE:
				case ItemDefinitionIndex::WEAPON_SMOKEGRENADE:
				case ItemDefinitionIndex::WEAPON_MOLOTOV:
				case ItemDefinitionIndex::WEAPON_DECOY:
				case ItemDefinitionIndex::WEAPON_INCGRENADE:
				case ItemDefinitionIndex::WEAPON_C4:
				case ItemDefinitionIndex::WEAPON_TASER:
					return false;
			}
			return true;
		}

		// Utils
		
		auto RayTrace(Vector3 leftBottom, Vector3 rightTop, float & fDistance)
		{
			auto vEyePosition = GetEyePosition();
			auto vAngle = m_csgoManager->GetEngineHelper()->GetViewAngle();
			m_csgoManager->GetEngineHelper()->AngleNormalizeClamp(vAngle);

			auto vDirection = vAngle.ToVector();
			auto vDirectionInverse = (Vector3(1.f) / vDirection);

			if (vDirection.m_flXPos == 0.0f && (vEyePosition.m_flXPos < std::min(leftBottom.m_flXPos, rightTop.m_flXPos) || vEyePosition.m_flXPos > std::max(leftBottom.m_flXPos, rightTop.m_flXPos)))
				return false;

			if (vDirection.m_flYPos == 0.0f && (vEyePosition.m_flYPos < std::min(leftBottom.m_flYPos, rightTop.m_flYPos) || vEyePosition.m_flYPos > std::max(leftBottom.m_flYPos, rightTop.m_flYPos)))
				return false;

			if (vDirection.m_flZPos == 0.0f && (vEyePosition.m_flZPos < std::min(leftBottom.m_flZPos, rightTop.m_flZPos) || vEyePosition.m_flZPos > std::max(leftBottom.m_flZPos, rightTop.m_flZPos)))
				return false;

			float t1 = (leftBottom.m_flXPos - vEyePosition.m_flXPos) * vDirectionInverse.m_flXPos;
			float t2 = (rightTop.m_flXPos - vEyePosition.m_flXPos) * vDirectionInverse.m_flXPos;
			float t3 = (leftBottom.m_flYPos - vEyePosition.m_flYPos) * vDirectionInverse.m_flYPos;
			float t4 = (rightTop.m_flYPos - vEyePosition.m_flYPos) * vDirectionInverse.m_flYPos;
			float t5 = (leftBottom.m_flZPos - vEyePosition.m_flZPos) * vDirectionInverse.m_flZPos;
			float t6 = (rightTop.m_flZPos - vEyePosition.m_flZPos) * vDirectionInverse.m_flZPos;

			float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
			float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

			if (tmax < 0)
			{
				fDistance = tmax;
				return false;
			}

			if (tmin > tmax)
			{
				fDistance = tmax;
				return false;
			}
			fDistance = tmin;
			return true;
		}

	private:
		std::shared_ptr <CCSGOManager>	m_csgoManager;
		DWORD							m_pBaseEntity;
		DWORD							m_dwClientBase;
		int								m_iEntityIndex;
};

