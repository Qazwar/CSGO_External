#pragma once
#include "valve\dt_recv.h"

class CList
{
public:
	void*		m_pCreateFn;
	void*		m_pCreateEventFn;
	const char* m_pNetworkName;
	RecvTable*	m_pRecvTable;
	CList*		m_pNext;
	int			m_iIndex;
};

class CHLClient
{
public:
	CList* GetList()
	{
		typedef CList* (__thiscall* GetListFn)(void*);
		return GetMethod<GetListFn>(this, 8)(this);
	}
};

class CEngineClient
{
public:

	void* ClientCmd()
	{
		typedef void* (__thiscall* ClientCmdFn)(void*);
		return GetMethod<ClientCmdFn>(this, 7)(this);
	}

	void* WorldToScreenMatrix()
	{
		typedef void* (__thiscall* WorldToScreenMatrixFn)(void*);
		return GetMethod<WorldToScreenMatrixFn>(this, 37)(this);
	}
};

typedef void* (*CreateInterfaceFn)(const char*, int*);
