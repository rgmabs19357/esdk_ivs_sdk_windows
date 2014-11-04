/********************************************************************
filename	: 	IVS_Trace.h
author		:	w00210470
created		:	2013/01/14	
description	:	TRACE 功能
copyright	:	Copyright (C) 2011-2015
history		:	2012/10/23 初始版本;
*********************************************************************/
#include "bp_log.h"
#include <stdarg.h>
#include <stdio.h>

#if defined _MSC_VER  || defined WIN32
# define SNPRINTF  _vsnprintf
#endif 

#if defined __GNUC__ || defined LINUX
# define SNPRINTF  vsnprintf
#endif

class CIVSTrace {
public:
	CIVSTrace(LOG_LEVEL level, const char *file, int line, const char *func, const char *fmt, ...)
		:m_enLevel(level), m_pFunc(func), m_pFile(file), m_nLine(line)
	{
		char msg[1024];
		msg[1023] = '\0';
		va_list args;
		va_start(args, fmt);
		(void)SNPRINTF(msg, sizeof(msg) - 1, fmt, args);
		va_end(args);
		IVS_LogRun(m_enLevel, "Enter ", m_pFile, m_nLine, m_pFunc, "%s %s", m_pFunc, msg);
	}

	~CIVSTrace()
	{
		try {
			IVS_LogRun(m_enLevel, "Leave ", m_pFile, m_nLine, m_pFunc, "%s", m_pFunc);
		} 
		catch(...){} ; //lint !e1740
	}
private:
	CIVSTrace();
    LOG_LEVEL   m_enLevel;
	const char *m_pFunc;
	const char *m_pFile;
	int         m_nLine;
};

#define IVS_INFO_TRACE(fmt, ...) CIVSTrace IvsTrace(IVSLOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define IVS_DEBUG_TRACE(fmt, ...) CIVSTrace IvsTrace(IVSLOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
