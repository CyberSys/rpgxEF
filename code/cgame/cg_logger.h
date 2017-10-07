#ifndef CG_LOGGER_H_
#define CG_LOGGER_H_

#include "../base_game/q_shared.h"

enum {
	LL_ALWAYS,
	LL_ERROR,
	LL_WARN,
	LL_INFO,
	LL_DEBUG,
	LL_TRACE
};

#ifdef DEBUG
#define CG_LogFuncBegin() CG_LocLogger(LL_TRACE, "%s - Begin\n", __FUNCTION__)
#define CG_LogFuncEnd() CG_LocLogger(LL_TRACE, "%s - End\n", __FUNCTION__)
#else
#define CG_LogFuncBegin()
#define CG_LogFuncEnd()
#endif

#define CG_LocLogger(X,...) _CG_LocLogger(__FILE__, __LINE__, X, __VA_ARGS__) 
void CG_Logger(int level, char* fmt, ...) __attribute__ ((format (printf, 2, 3)));
void _CG_LocLogger(const char* file, int line, int level, char* fmt, ...) __attribute__ ((format (printf, 4, 5)));

#endif /* CG_LOGGER_H_ */
