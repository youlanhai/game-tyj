//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Log.h"
#include "Misc.h"

namespace Lazy
{

cLog        g_globle_log_ ;

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

cLog::cLog(void)
{
	m_bUsefull=false;
}

cLog::~cLog(void)
{
    unload();
}

bool cLog::init(const tstring & fileName)
{
	if(m_bUsefull)
	{
		return false;
	}

	m_fLog.open(fileName.c_str());
	if(!m_fLog.good())
	{
		return false;
	}
	m_bUsefull = true;
    m_fLog.imbue(std::locale(std::locale(""), "", LC_CTYPE));
    
	GetLocalTime(&m_systm);
	tstring strInfo;
	formatString(strInfo, _T("---[%d-%d-%d] LOG SYSTEM START ---\n"), 
        m_systm.wYear, m_systm.wMonth, m_systm.wDay);
	write(strInfo);

	return true;
}

void cLog::unload(void)
{
    if(!m_bUsefull)
    {
        return ;
    }


    GetLocalTime(&m_systm);
	tstring strInfo;
	formatString(strInfo, _T("***[%d-%d-%d] LOG SYSTEM STOPED ***\n\n"), 
        m_systm.wYear, m_systm.wMonth, m_systm.wDay);
    write(strInfo);

    m_fLog.close();

    m_bUsefull = false;
}

void cLog::generateTimeHead(tstring & header)
{
    GetLocalTime(&m_systm);
    
    formatString(header, _T("[%2.2d:%2.2d:%2.2d]: "), 
        m_systm.wHour, m_systm.wMinute, m_systm.wSecond);
}

void cLog::write(const tstring & buffer)
{
    if (!m_bUsefull)
    {
        return;
    }

#ifdef _DEBUG
	debugMessage(buffer.c_str());
#endif

    tstring header;
	generateTimeHead(header);

#ifdef LOG_NEED_THREAD_SAFE
	ZLockHolder holder(&m_writeMutex);
#endif
    m_fLog<<header<<buffer<<std::endl;
}

void cLog::writeA(const std::string & buffer)
{
#ifdef _UNICODE
    write(charToWChar(buffer));
#else
    write(buffer);
#endif
}

void  cLog::writeW(const std::wstring & buffer)
{
#ifdef _UNICODE
    write(buffer);
#else
    write(wcharToChar(buffer));
#endif
}

void cLog::xwriteA(LPCSTR format, ...  )
{
	std::string buffer;
	
	va_list vl;
	va_start (vl, format);
	formatStringVSA(buffer, format, vl);
	va_end (vl);  

    writeA(buffer);
}


void cLog::xwriteW(LPCWSTR format, ...  )
{
	std::wstring buffer;

	va_list vl;
	va_start (vl, format);
	formatStringVSW(buffer, format, vl);
	va_end (vl);  

	writeW(buffer);
}

}//namespace Lazy