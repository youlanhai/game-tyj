
#include "Commen.h"
#include "Log.h"
#include "Utilities.h"

#ifdef USE_OLD_UTILITY

const int MAX_SIZE_BUFFER = 10000;
char g_szGlobleBuffer[MAX_SIZE_BUFFER];
static CLog g_globle_log_ ;
//////////////////////////////////////////////////////////////////////////

CLog::CLog(void)
{
	m_bUsefull=false;
}

CLog::~CLog(void)
{
    unload();
}
bool CLog::init(LPCSTR szLogName)
{
	if(m_bUsefull)
	{
		return false;
	}
	m_fLog.open(szLogName);
	if(!m_fLog.good())
	{
		return false;
	}
	m_bUsefull=true;
	GetLocalTime(&m_systm);
	char buffer[50];
	sprintf_s(buffer,50,"---[%d-%d-%d]日志系统启动---\n",m_systm.wYear,m_systm.wMonth,m_systm.wDay);
	write(buffer);
	return true;
}
void CLog::write(LPCSTR szBuffer,bool bApendTime,bool bNewLine)
{
    if (!m_bUsefull)
    {
        OutputDebugStringA(szBuffer);
        return;
    }
    //assert(m_bUsefull && "日志系统未初始化！");

	GetLocalTime(&m_systm);
	char buffer[64];
	sprintf_s(buffer,64,"[%2.2d:%2.2d:%2.2d]: ",m_systm.wHour,m_systm.wMinute,m_systm.wSecond);
	if (bApendTime)
	{
		m_fLog<<buffer;
	}
		
    m_fLog<<szBuffer;
	
    if (bNewLine)
    {
        m_fLog<<std::endl;
    }
}

void  CLog::write(LPCWSTR szBuffer)
{
    LZUtility::WCharToChar(g_szGlobleBuffer,szBuffer);
    write(g_szGlobleBuffer);
}

void CLog::writex(LPCSTR format, ...  )
{
    assert(m_bUsefull && "日志系统未初始化！");

    va_list vl;
    va_start (vl, format);

    vsnprintf_s (g_szGlobleBuffer, MAX_SIZE_BUFFER,MAX_SIZE_BUFFER, format, vl);
    if (!m_bUsefull)
    {
        OutputDebugStringA(g_szGlobleBuffer);
        return;
    }
    write(g_szGlobleBuffer);

    va_end (vl);     
}

void CLog::unload(void)
{
    if(m_bUsefull)
    {
        GetLocalTime(&m_systm);
        char buffer[50];
        sprintf_s(buffer,50,"---[%d-%d-%d]日志系统停止---\n",m_systm.wYear,m_systm.wMonth,m_systm.wDay);
        write(buffer);
        m_fLog.close();
        m_bUsefull = false;
    }
}

//////////////////////////////////////////////////////////////////////////

LZDLL_API bool INIT_LOG(LPCSTR szFileName)
{
    return g_globle_log_.init(szFileName);
}

LZDLL_API void WRITE_LOG(LPCSTR szBuffer)
{
    g_globle_log_.write(szBuffer);

#ifdef _DEBUG
    OutputDebugStringA(szBuffer);
#endif
}

LZDLL_API void WRITE_LOG(LPCWSTR szBuffer)
{
    g_globle_log_.write(szBuffer);

#ifdef _DEBUG
    OutputDebugStringW(szBuffer);
#endif
}

LZDLL_API void XWRITE_LOGA(LPCSTR format, ...)
{
    va_list vl;
    va_start (vl, format);

    vsnprintf_s (g_szGlobleBuffer, MAX_SIZE_BUFFER,MAX_SIZE_BUFFER, format, vl);
    g_globle_log_.write(g_szGlobleBuffer);

#ifdef _DEBUG
    OutputDebugStringA(g_szGlobleBuffer);
#endif

    va_end (vl); 
}

LZDLL_API void UNLOAD_LOG()
{
    g_globle_log_.unload();
}



/**写debug日志*/
LZDLL_API void DebugMsg_(LPCSTR format, ...)
{
    va_list vl;
    va_start (vl, format);

    vsnprintf_s (g_szGlobleBuffer, MAX_SIZE_BUFFER, MAX_SIZE_BUFFER, format, vl);
    //g_globle_log_.write(g_szGlobleBuffer);
    OutputDebugStringA(g_szGlobleBuffer);

    va_end (vl); 
}

#endif