//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#ifndef LAZY_UTILITY_CONFIG_H
#define LAZY_UTILITY_CONFIG_H

#include <Windows.h>

#include <tchar.h>
#include <string>
#include <list>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cstdlib>

namespace Lazy
{

typedef int					int32;
typedef unsigned int        uint32;
typedef __int64             int64;
typedef unsigned __int64    uint64;

#ifdef _UNICODE

typedef wchar_t         tchar;
typedef std::wstring    tstring;
typedef std::wostream   tostream;
typedef std::wifstream  tifstream;
typedef std::wofstream  tofstream;

#else

typedef char            tchar;
typedef std::string     tstring;
typedef std::ostream    tostream;
typedef std::ifstream   tifstream;
typedef std::ofstream	tofstream;

#endif

typedef std::vector<tstring> StringPool;
const tstring EmptyStr = _T("");

}//namespace Lazy


#define SLASH_USE _T('/')
#define SLASH_OLD _T('\\')

#define SAFE_CLOSE_HANDLE(h) if(h){ CloseHandle(h); h = NULL; }
#define SAFE_RELEASE(pObj) if(pObj){ pObj->Release(); pObj=NULL; }

#define FOR_EACH(pool, it) for(it=pool.begin(); it!=pool.end(); ++it)

#define MAKE_UNCOPY(cl) \
private:            \
    cl(const cl&);  \
    const cl& operator=(const cl&);


/*日志是否需要线程安全*/
#define  LOG_NEED_THREAD_SAFE

/*打印内存检测的所有信息*/
//#define DEBUG_ALL_MEMORY_CHECK


#endif //LAZY_UTILITY_CONFIG_H