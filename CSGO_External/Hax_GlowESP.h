#pragma once
#include "Defines.h"
#include "SourceEngine\\Vector.hpp"

extern class CEntity;
extern class CCSGOManager;

// https://github.com/MarkHC/CSGOSimple/blob/b3620cc652d801bcb16cc37432c4f2f68e1b43f3/CSGOSimple/valve_sdk/misc/glow_outline_effect.hpp#L6
class GlowObjectDefinition_t
{
public:
    GlowObjectDefinition_t()
	{
		memset(this, 0, sizeof(*this));
	}

    class IClientEntity* m_pEntity;    //0x0000

    union {
        se::Vector m_vGlowColor;       //0x0004
        struct
        {
            float   m_flRed;           //0x0004
            float   m_flGreen;         //0x0008
            float   m_flBlue;          //0x000C
        };
    };

    float   m_flAlpha;                 //0x0010
    uint8_t pad_0014[4];               //0x0014
    float   m_flSomeFloat;             //0x0018
    uint8_t pad_001C[4];               //0x001C
    float   m_flAnotherFloat;          //0x0020
    bool    m_bRenderWhenOccluded;     //0x0024
    bool    m_bRenderWhenUnoccluded;   //0x0025
    bool    m_bFullBloomRender;        //0x0026
    uint8_t pad_0027[5];               //0x0027
    int32_t m_nGlowStyle;              //0x002C
    int32_t m_nSplitScreenSlot;        //0x0030
    int32_t m_nNextFreeSlot;           //0x0034

	bool IsUnused() const { 
		return m_nNextFreeSlot != GlowObjectDefinition_t::ENTRY_IN_USE; 
	};

    static const int END_OF_FREE_LIST = -1;
    static const int ENTRY_IN_USE = -2;

}; //Size: 0x0038 (56)

class CGlowESP
{
	public:
		CGlowESP(const std::shared_ptr<CCSGOManager> & csgoManager);
		~CGlowESP();

		// common
		void	LoadConfigs();
		void	OnUpdate();

		// dynamics
		void	Enable()			{ m_bEnabled = true;	};
		void	Disable()			{ m_bEnabled = false;	};
		bool	IsEnabled() const	{ return m_bEnabled;	};

		void	Suspend()			{ m_bSuspended = true;	};
		void	Resume()			{ m_bSuspended = false;	};
		bool	IsSuspended() const	{ return m_bSuspended;	};

		DWORD	GetEnableKey()		{ return m_dwEnableKey; };

	protected:
		// color helper
		void	ConvertFromColor(DWORD dwColor, std::array <float, 4> * pRgba);
		DWORD	ConvertToColor(std::array <float, 4> rgba);

	private:
		// data
		std::shared_ptr <CEntity>			m_pLocalEntity;
		std::shared_ptr <CEntity>			m_pCurrentEntity;

		// helper
		std::shared_ptr <CCSGOManager>		m_cCSGOManager;

		// settings
		bool								m_bEnabled;
		bool								m_bSuspended;

		bool								m_bRadarIsEnabled;
		bool								m_bEnableGlow;
		DWORD								m_dwEnableKey;
		DWORD								m_dwUpdateInterval;
		bool								m_bIsEnemyOnly;
		bool								m_bIsVisibleOnly;
		bool								m_bShowC4;
		bool								m_bShowC4Host;
		bool								m_bShowChicken;
		bool								m_bChamsIsEnabled;

		// glow color arrays
		std::array <float, 4>				m_FriendColorArray;
		std::array <float, 4>				m_EnemyColorArray;
		std::array <float, 4>				m_C4ColorArray;
		std::array <float, 4>				m_C4HostColorArray;
		std::array <float, 4>				m_ChickenColorArray;

#if HAX_MULTI_THREAD == true
	public:
		bool					CreateThread();
		bool					TerminateThread();

		void					SendStopSignal();

	protected:
		DWORD					ThreadRoutine(void);
		static DWORD WINAPI		StartThreadRoutine(LPVOID lpParam);

	private:
		HANDLE					m_hThread;
		bool					m_bThreadIsStopped;
#endif
};
