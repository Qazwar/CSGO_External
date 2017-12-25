#include "main.h"
#include "Defines.h"
#include "NetVarManager.h"
#include "HaxModuleIndex.h"
#include "XOR.h"

// calls GetProp wrapper to get the absolute offset of the prop
int CCSGOData::GetNetVar(const char *tableName, const char *propName)
{
	int offset = GetProp(tableName, propName);
	if (!offset)
	{
		// FileLogf("netvartest.txt", "%s not found!\n", propName);
		return 0;
	}

	// FileLogf("netvartest.txt", "%s - %s: 0x%02X\n", tableName, propName, offset);
	return offset;
}


// wrapper so we can use recursion without too much performance loss
int CCSGOData::GetProp(const char *tableName, const char *propName, RecvProp **prop)
{
	RecvTable *recvTable = GetTable(tableName);
	if (!recvTable)
		return 0;

	int offset = GetProp(recvTable, propName, prop);
	if (!offset)
		return 0;

	return offset;
}

// uses recursion to return a the relative offset to the given prop and sets the prop param
int CCSGOData::GetProp(RecvTable *recvTable, const char *propName, RecvProp **prop)
{
	int extraOffset = 0;
	for (int i = 0; i < recvTable->m_nProps; ++i)
	{
		RecvProp *recvProp = &recvTable->m_pProps[i];
		RecvTable *child = recvProp->GetDataTable();

		if (child && (child->m_nProps > 0))
		{
			int tmp = GetProp(child, propName, prop);
			if (tmp)
				extraOffset += (recvProp->GetOffset() + tmp);
		}

		if (stricmp(recvProp->m_pVarName, propName))
			continue;

		if (prop)
			*prop = recvProp;

		return (recvProp->GetOffset() + extraOffset);
	}

	return extraOffset;
}


RecvTable * CCSGOData::GetTable(const char *tableName)
{
	if (m_NetvarTables.empty())
		return 0;

	for each (RecvTable *table in m_NetvarTables)
	{
		if (!table)
			continue;

		if (stricmp(table->m_pNetTableName, tableName) == 0)
			return table;
	}

	return 0;
}

void CCSGOData::DumpTable(RecvTable *table)
{
	for (int i = 0; i < table->m_nProps; i++)
	{
		RecvProp *prop = &table->m_pProps[i];
		if (!strcmp(prop->GetName(), XOR("baseclass")))
			continue;

		// FileLogf("netvartest.txt", "%s-%s [0x%02X]", table->GetName(), prop->GetName(), prop->GetOffset());
		if (prop->GetDataTable()) {
			m_NetvarTables.push_back(prop->GetDataTable());
			DumpTable(prop->GetDataTable()); //recursive call 
		}
	}
}

void CCSGOData::DumpNetvars()
{
	m_NetvarTables.clear();

	for (auto pList = m_pClient->GetList(); pList; pList = pList->m_pNext)
	{
		auto table = pList->m_pRecvTable;
		m_NetvarTables.push_back(table);
		DumpTable(table);
	}
}
