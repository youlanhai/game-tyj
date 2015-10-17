#pragma once

//#define USE_OLD_UTILITY

#include "Commen.h"
#include "../utility/Utility.h"

namespace LZUtility
{
    extern std::string DefaultResPath;

#ifdef USE_OLD_UTILITY
    //宽字符串与字符串的转换
    LZDLL_API LPCWSTR CharToWChar(LPWSTR szWChar, LPCSTR szChar);

    LZDLL_API LPCWSTR CharToWChar(std::string str);

    LZDLL_API LPCSTR WCharToChar(LPSTR szChar, LPCWSTR szWChar);

    LZDLL_API std::string WCharToChar(LPCWSTR szWChar);
#endif

    ///查找文件
    LZDLL_API bool SearchFile(LPSTR szDest, LPCSTR szFilename);

    ///查找文件
    LZDLL_API bool searchFile(std::string & strDest, const std::string& strSrc, const std::string & dir = DefaultResPath);

    ///改变工作路径
    LZDLL_API bool ChangeCurDirectory(LPCTSTR szPath = NULL);

    ///从路径中获取文件名
    LZDLL_API LPCSTR GetTitleFromPath(LPSTR szName,LPCSTR szPath);

    LZDLL_API LPD3DXFONT createFont(IDirect3DDevice9* pDevice, std::string name, int size);

    LZDLL_API LPD3DXFONT createFont(IDirect3DDevice9* pDevice, std::string name, int w, int h, bool bold);

    LZDLL_API HFONT createHFont(LPCSTR lzfont);

}

/** lz格式字体名称解析。
    lz格式字体名：字体名/字体宽度/字体高度/是否粗体。“是否粗体”忽略时认为不是粗体，
    否则无论什么字符，都被认为是粗体。
    例如：宋体/20/20/b，宋体/20/20/x，宋体/20
    */
class LZDLL_API cLZFontNameParser
{
public:
    cLZFontNameParser(LPCSTR lzfontName);

    std::string fontNameA(){ return m_fontName.c_str(); }

    std::wstring fontNameW(){  return Lazy::charToWChar(m_fontName);}

#ifdef _UNICODE
#   define fontName fontNameW
#else
#   define fontName fontNameA
#endif

    int width(){ return m_width; }

    int height(){ return m_height; }

    bool bold(){ return m_bold; }

protected:

    void parse();

protected:
    LPCSTR      m_lzfont;       //< lz格式字体字符串
    std::string m_fontName;     //< 字体名称
    int         m_width;        //< 字体宽度
    int         m_height;       //< 字体高度
    bool        m_bold;         //< 是否粗体
};

#ifdef USE_OLD_UTILITY

class LZDLL_API cMemoryCheck
{
public:
    typedef std::map<std::string, int>  Container;
    typedef Container::iterator         Iterator;

    static cMemoryCheck* instance(void);

    static void release(void);

    void construct(std::string name, void* ptr);

    void destruct(std::string name, void* ptr);

private: 

    cMemoryCheck();

    ~cMemoryCheck();

    std::map<std::string, int> m_map;
    static  cMemoryCheck* m_pInstance;
};


#define DEBUG_CONSTRUCT(classname) cMemoryCheck::instance()->construct(classname, this)
#define DEBUG_DESTRUCT(classname) cMemoryCheck::instance()->destruct(classname, this)

#define MEMORY_CHECK_CONS  DEBUG_CONSTRUCT(typeid(*this).name())
#define MEMORY_CHECK_DEST  DEBUG_DESTRUCT(typeid(*this).name())

#endif