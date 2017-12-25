#pragma once
#include "main.h"

#ifndef HAX_MULTI_THREAD
	#define HAX_MULTI_THREAD true
#endif

#ifndef USE_SELF_SECURITY
	#define USE_SELF_SECURITY false
#endif

// TODO: for release mode, xor'd debug view logs
#ifndef DEBUG_LOG
	#ifdef _DEBUG
		#define DEBUG_LOG(level, log, ...) g_haxApp->LogHelperInstance()->sys_log(__FUNCTION__,  level, log, __VA_ARGS__);
	#else
		#define DEBUG_LOG(level, log, ...)
	#endif
#endif
