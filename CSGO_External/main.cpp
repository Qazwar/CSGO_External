#include "main.h"
#include "HaxMain.h"
#include "XOR.h"

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "winmm.lib")

std::shared_ptr <CHaxApp> g_haxApp = nullptr;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	__asm
	{
		xor eax, eax
		jz valid
		__asm __emit(0xea)
	}

	return 0;

valid:
	g_haxApp = std::make_shared<CHaxApp>();

	if (g_haxApp == nullptr || g_haxApp.get() == nullptr)
	{
		OutputDebugStringA(XOR("Init fail!\n"));
		abort();
		return 0;
	}

	g_haxApp->PreInitialize();
	g_haxApp->Initialize();

#ifdef _DEBUG
	printf(XOR(" # PROCESS WORK COMPLETED! # \n"));
	getchar();
#endif
	return 0;
}

