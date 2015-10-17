//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "lzdParser.h"
#include "lzd.h"

namespace Lazy
{

#define PARSE_TEST

#ifdef PARSE_TEST
#include <iostream>
void testOutput(tstring str)
{
    //std::cout<<str<<std::endl;
    OutputDebugString(str.c_str());
}
#endif

namespace
{
    const tstring NullString = _T("");
    const tstring WhiteSpace = _T(" \t\r\n");//空白符

    const tstring WhiteSpaceNoRetrun = _T(" \t\r");//空白符，不包括换行符
    const tstring EndString = _T("}#;\n");//结束符
    const tstring TransferedString = _T("}#;\\");//转义字符

}

bool isWhiteSpace(tchar ch)
{
    return WhiteSpace.find(ch) != WhiteSpace.npos;
}

bool isVarNameValid(const tstring & name)
{
    return true;
}

size_t findCh(const char *buffer, size_t n, char ch)
{
    for(size_t i=0; i<n; ++i)
    {
        if(buffer[i] == ch) return i;
    }
    return n;
}

///解析编码头
/*static*/ size_t cParser::parseCodingHeader(const char *buffer, size_t len)
{
    //找出第一行
    len = findCh(buffer, len, '\n');

    //找出#号
    size_t head = findCh(buffer, len, '#') + 1;
    if(head >= len) return 0;

    //找出等号
    size_t eq = findCh(buffer, len, '=');
    if(eq >= len) return 0;

    std::string name(buffer+head, buffer+eq);
    trimStringA(name);
    if(name != "coding") return 0;

    size_t n = len - eq - 1;

    tstring code(n, 0);
    for(size_t i=0; i<n; ++i)
    {
        code[i] = (tchar)buffer[eq+i+1];
    }

    trimString(code);
    return codeToCp(code);
}

//////////////////////////////////////////////////////////////////////////

cParser::cParser(StrStreamPtr stream, LZDataPtr curNode, int lineNo)
    : m_stream(stream)
    , m_pCurNode(curNode)
    , m_lineNo(lineNo)
    , m_errorNo(0)
    , m_errorLine(0)
    , m_bEnd(false)
{
}

cParser::~cParser(void)
{
}

void cParser::parse(void)
{
    m_bEnd = false;

    while(!error() && !m_stream->empty() && !m_bEnd)
    {
        tchar ch = m_stream->getchar();

        switch(ch)
        {
        case _T('{') :
            onLBraket();
            break;
        case _T('}') :
            onRBraket();
            break;
        case _T('=') :
            onEqual();
            break;
        case _T('\n') :
            onReturn();
            break;
        case _T('#') :
            onComment();
            break;
        case _T(';'):
            break;
        default:
            onOther(ch);
            break;
        }
    }
}

void cParser::onLBraket(void)
{
    trimString(m_tempStr);
    if(m_tempStr.empty())
    {
        if(m_cacheName.empty())
        {
            setError(ErrorCode::InvalidLBraket, m_lineNo);
            return;
        }
        m_tempStr = m_cacheName;
    }

    LZDataPtr node = new lzd(m_tempStr);
    m_pCurNode->addChild(node);

    cParser parser(m_stream, node, m_lineNo);
    parser.parse();

    m_lineNo = parser.getLineNo();
    m_errorNo = parser.getErrorNo();
    m_errorLine = parser.getErrorLine();

    if(error()) return;

    m_tempStr.clear();
    m_cacheName.clear();
}

void cParser::onRBraket(void)
{
    m_bEnd = true;
}

void cParser::onEqual(void)
{
    trimString(m_tempStr);
    if(m_tempStr.empty())
    {
        setError(ErrorCode::InvalidAssignment, m_lineNo);
        return;
    }

    m_pCurNode->addChild( new lzd(m_tempStr, getAStr()) );

    m_tempStr.clear();
    m_cacheName.clear();
}

void cParser::onReturn(void)
{
    trimString(m_tempStr);
    if(!m_tempStr.empty())
    {
        m_cacheName = m_tempStr;//缓存名称
        m_tempStr.clear();
    }

    ++m_lineNo;
}

///注释‘#’
void cParser::onComment(void)
{
    skipToReturn();
}

///分号
void cParser::onSemicolon(void)
{
    m_tempStr.clear();
    m_cacheName.clear();
}

void cParser::onOther(tchar ch)
{
    m_tempStr += ch;
    //testOutput(m_curVariable);
}

void cParser::skipToEnd(void)
{
    while(!m_stream->empty())
    {
        tchar ch = m_stream->curchar();
        if(ch == _T('\n') || ch == _T('}')) break;

        m_stream->ignore(1);
    }
}

///跳的回车符处
void cParser::skipToReturn(void)
{
    while(!m_stream->empty())
    {
        tchar ch = m_stream->curchar();
        if(ch == _T('\n')) break;

        m_stream->ignore(1);
    }
}

/** 从当前位置起，分析出一个合法的字符串（即遇到结束符[}，#，\n，；]位置 ）。
    */
tstring cParser::getAStr(void)
{
    tstring str;
    tchar ch;

    while(!m_stream->empty())
    {
        ch = m_stream->curchar();

        if(ch == _T('\\'))//转义符
        {
            m_stream->ignore(1);
            str += m_stream->getchar();
        }
        else if(EndString.find(ch) != EndString.npos) //结束符
        {
            break;
        }
        else
        {
            str += m_stream->getchar();
        }
    }
    trimString(str);

    return str;
}

}//namespace Lazy