#pragma once

#include "Base.h"

#include "Utilities.h"
#ifdef USE_OLD_UTILITY

/**日志系统*/
class LZDLL_API CLog
{
public:

	CLog(void);

	~CLog(void);

	bool init(LPCSTR szLogName);

	void write(LPCSTR szBuffer,bool bApendTime=true,bool bNewLine=true);

    void write(LPCWSTR szBuffer);

    void writex(LPCSTR format, ...);

    void unload(void);

    bool isUsefull(void){ return m_bUsefull; }
private:
	std::ofstream	m_fLog;		//输出文件流
	bool			m_bUsefull;	//日志系统是否可用
	SYSTEMTIME		m_systm;
};

/**初始日志系统。*/
LZDLL_API bool INIT_LOG(LPCSTR szFileName);

/**写日志*/
LZDLL_API void WRITE_LOG(LPCSTR szBuffer);

/**写日志*/
LZDLL_API void WRITE_LOG(LPCWSTR szBuffer);

/**写日志*/
LZDLL_API void XWRITE_LOGA(LPCSTR format, ...);

/**卸载日志系统*/
LZDLL_API void UNLOAD_LOG();

/**写debug日志*/
LZDLL_API void DebugMsg_(LPCSTR format, ...);


#ifdef _DEBUG
#   define DebugMsg(format, ...) DebugMsg_(format, __VA_ARGS__)
#else
//#   define DebugMsg(format, ...)
#define DebugMsg(format, ...) DebugMsg_(format, __VA_ARGS__)
#endif

#endif