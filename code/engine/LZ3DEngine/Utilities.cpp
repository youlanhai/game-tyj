//Utilities.cpp 
#include "Commen.h"
#include "Utilities.h"
#pragma  comment(lib,"Dbghelp.lib")

#define MAX_SIZE 1024
static WCHAR szWideProgID[MAX_SIZE];

extern const int MAX_SIZE_BUFFER = 10000;
extern char g_szGlobleBuffer[MAX_SIZE_BUFFER];


#pragma warning(push)
#pragma warning(disable: 4996)

namespace LZUtility
{
    std::string DefaultResPath = "";

#ifdef USE_OLD_UTILITY
    //宽字符串与字符串的转换
    LZDLL_API LPCWSTR CharToWChar(LPWSTR szWChar, LPCSTR szChar)
    {
        long lLen = MultiByteToWideChar(CP_ACP,0,
            szChar,strlen(szChar),
            szWideProgID,sizeof(szWideProgID)); 
        szWideProgID[lLen] = '\0';
        if(szWChar != NULL)
        {
            wcscpy(szWChar,szWideProgID);
        }
        return szWideProgID;
    }

    LZDLL_API LPCWSTR CharToWChar(std::string str)
    {
        return CharToWChar(NULL, str.c_str());
    }

    LZDLL_API LPCSTR WCharToChar(LPSTR szChar, LPCWSTR szWChar)
    {
        int len=WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,
            szWChar,-1,
            g_szGlobleBuffer,MAX_SIZE,NULL,NULL); 
        g_szGlobleBuffer[len]=0;
        if(szChar != NULL)
        {
            strcpy(szChar,g_szGlobleBuffer);
        }
        return g_szGlobleBuffer;
    }

    LZDLL_API std::string WCharToChar(LPCWSTR szWChar)
    {
        int len=WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,
            szWChar,-1,
            g_szGlobleBuffer,MAX_SIZE,NULL,NULL); 
        g_szGlobleBuffer[len]=0;
        return g_szGlobleBuffer;
    }
#endif

    ///查找文件
    LZDLL_API bool SearchFile(LPSTR szDest, LPCSTR szFilename)
    {
        if (NULL==szDest || NULL==szFilename || 0==szFilename[0])
        {
            return false;
        }

        char szCurPath[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, szCurPath);
        return !!SearchTreeForFile(szCurPath, szFilename, szDest);
    }

    bool fileExist(const std::string file)
    {
        return GetFileAttributesA(file.c_str()) != -1;
    }

    void formatFilePath(std::string & filePath)
    {
        std::replace(filePath.begin(), filePath.end(), '\\', '/');
    }

    void formatDirPath(std::string & dirPath)
    {
        if (dirPath.empty())
        {
            return;
        }
        formatFilePath(dirPath);
        if (dirPath[dirPath.length()-1] != '/')
        {
            dirPath += '/';
        }
    }

    bool _searchFile(std::string & strDest, const std::string & path, const std::string & filename)
    {
        std::string tempPath = path;
        formatDirPath(tempPath);

        std::string tempName = tempPath + filename;
        if(fileExist(tempName))
        {
            strDest = tempName;
            return true;
        }

        WIN32_FIND_DATAA data;
        ZeroMemory(&data, sizeof(data));
        HANDLE handle = ::FindFirstFileA((tempPath + "*").c_str(), &data);
        if (handle == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        bool exist = false;
        do
        {
            if (strcmp(data.cFileName, ".")==0 || strcmp(data.cFileName, "..")==0)
            {
                continue;
            }
            else if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if(_searchFile(strDest, tempPath + data.cFileName, filename))
                {
                    exist = true;
                    break;
                }
            }
        }while(::FindNextFileA(handle, &data));
        ::FindClose(handle);
        return exist;
    }


    bool searchFile(std::string & strDest, const std::string& strSrc, const std::string & dir/* = DefaultResPath*/)
    {
        std::string filename = strSrc;
        formatFilePath(filename);
        size_t pos = filename.find_last_of('/');
        if (pos != filename.npos) //名称带路径
        {
            if(fileExist(strSrc))
            {
                strDest = strSrc;
                return true;
            }

            //去掉路径搜索
            filename = filename.substr(pos+1);
        }
        //DebugMsg("searchFile: src=%s, fname=%s", strSrc.c_str(), filename.c_str());
        return _searchFile(strDest, dir, filename);
    }


    ///改变工作路径
    LZDLL_API bool ChangeCurDirectory(LPCTSTR szPath)
    {
        WCHAR szBuffer[MAX_PATH];
        if (szPath != NULL)
        {
            wcscpy_s(szBuffer,MAX_PATH,szPath);
        }
        else
        {
            GetModuleFileName(NULL,szBuffer,MAX_PATH);
            LPTSTR p = wcsrchr(szBuffer,L'/');
            if (p == NULL)
            {
                p = wcsrchr(szBuffer,L'\\');
                if (p == NULL)
                {
                    return false;
                }
            }
            (*p) = 0;
        }
        SetCurrentDirectory(szBuffer);
        return true;
    }

    ///从路径中获取文件名
    LZDLL_API LPCSTR GetTitleFromPath(LPSTR szName,LPCSTR szPath)
    {
        if (szName == NULL || szPath == NULL)
        {
            return NULL;
        }

        const char *pFirst = strrchr(szPath,'/');
        if (pFirst == NULL)
        {
            pFirst = strrchr(szPath,'\\');
            if (pFirst == NULL)
            {
                pFirst = szPath;
            }
            else
            {
                ++pFirst;
            }
        }
        else
        {
            ++pFirst;
        }

        int i = 0;
        while(pFirst[i]!='.' && pFirst[i]!=0 )
        {
            szName[i] = pFirst[i];
            ++i;
        }
        szName[i] = 0;
        return szName;
    }

    LZDLL_API LPD3DXFONT createFont(IDirect3DDevice9* pDevice, std::string name, int size)
    {
        return createFont(pDevice, name, size, 0, false);
    }

    LZDLL_API LPD3DXFONT createFont(IDirect3DDevice9* pDevice, std::string name, int w, int h, bool bold)
    {
        LPD3DXFONT d3dfont = NULL;
        int weight = FW_NORMAL;
        if (bold)
        {
            weight = FW_BOLD;
        }
        if(FAILED(D3DXCreateFont(
            pDevice, 
            w,
            h, 
            weight, 
            1, 
            FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, 
            DEFAULT_QUALITY, 
            DEFAULT_PITCH | FF_DONTCARE, 
            Lazy::charToWChar(name).c_str(),
            &d3dfont )))
        {
            d3dfont = NULL;
        }
        return d3dfont;
    }
    
    LZDLL_API HFONT createHFont(LPCSTR lzfont)
    {
        if (NULL == lzfont)
        {
            return NULL;
        }
        cLZFontNameParser parser(lzfont);
        HFONT fn = CreateFont(
            parser.width(),           // nHeight
            parser.height(),          // nWidth
            0,                         // nEscapement
            0,                         // nOrientation
            FW_NORMAL,                 // nWeight
            FALSE,                     // bItalic
            FALSE,                     // bUnderline
            0,                         // cStrikeOut
            DEFAULT_CHARSET,            // nCharSet
            OUT_DEFAULT_PRECIS,        // nOutPrecision
            CLIP_DEFAULT_PRECIS,       // nClipPrecision
            DEFAULT_QUALITY,           // nQuality
            DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
            parser.fontName().c_str()); // lpszFacename
        return fn;
    }
}//namespace LZUtility

#pragma warning(pop)

cLZFontNameParser::cLZFontNameParser(LPCSTR lzfontName)
{
    m_lzfont = lzfontName;   
    m_fontName = "宋体"; 
    m_width = 20; 
    m_height = 0;
    m_bold = false;
    parse();
}


void cLZFontNameParser::parse()
{
    if (NULL == m_lzfont)
    {
        return;
    }
    char *names[4];
    int index = 0;
    char name[256];
    strcpy_s(name, 256, m_lzfont);
    char* p = name;
    names[index++] = p;
    while(*p != 0)
    {
        if (*p == '/')
        {
            *p = 0;
            names[index++] = p+1;
            if (index >= 4)
            {
                break;
            }
        }
        ++p;
    }
    m_fontName = names[0];
    if (index >= 2)
    {
        m_width = atoi(names[1]);
    }
    if (index >= 3)
    {
        m_height = atoi(names[2]);
    }
    if (index >= 4)
    {
        m_bold = true;
    }
}

#ifdef USE_OLD_UTILITY

//////////////////////////////////////////////////////////////////////////
cMemoryCheck* cMemoryCheck::m_pInstance = NULL;

cMemoryCheck* cMemoryCheck::instance(void)
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new cMemoryCheck();
    }
    return m_pInstance;
}

void cMemoryCheck::release(void)
{
    delete m_pInstance;
    m_pInstance = NULL;
}

cMemoryCheck::cMemoryCheck()
{

}

cMemoryCheck::~cMemoryCheck()
{
    for (Iterator it = m_map.begin();
        it != m_map.end();
        ++it)
    {
        if (it->second != 0)
        {
            DebugMsg("!!!!!!error : %s memory leak with [%d]!!!!!!", it->first.c_str(), it->second);
        }
    }
}

void cMemoryCheck::construct(std::string name, void* ptr)
{
    //DebugMsg("%s construct at 0x%8.8d", name.c_str(), DWORD(ptr));
    if (m_map.find(name) == m_map.end())
    {
        m_map[name] = 0;
    }
    ++m_map[name];
}

void cMemoryCheck::destruct(std::string name, void* ptr)
{
    //DebugMsg("%s destruct at 0x%8.8d", name.c_str(), DWORD(ptr));
    if (m_map.find(name) != m_map.end())
    {
        --m_map[name];
    }
}

#endif