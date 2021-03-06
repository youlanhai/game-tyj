﻿//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "lzd.h"
#include "lzdParser.h"
#include "../FileTool.h"
#include <sstream>

namespace Lazy
{

//////////////////////////////////////////////////////////////////////////

//写入文件时，需要对#、}、；转义
tstring convertValue(tstring src)
{
    tstring dest;
    for (size_t i=0; i<src.size(); ++i)
    {
        tchar ch = src[i];
        if (ch==_T('#') || ch==_T('}') || ch==_T(';'))
        {
            dest += _T('\\');
        }
        dest += ch;
    }
    return dest;
}


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/*static*/ LZDataPtr lzd::createFromFile(const tstring & fname)
{
    LZDataPtr p = new lzd();
    if(!p->load(fname))
    {
        p = NULL;
    }
    return p;
}

bool lzd::loadFromBuffer(const tchar* buffer, int length)
{
    cParser parser(new StrStream(buffer, length), this, 1);
    parser.parse();

    if(parser.error())
    {
        debugMessage(_T("parse laz file failed! eno:%d, line:%d"),
            parser.getErrorNo(), parser.getErrorLine());
    }

    return !parser.error();
}

lzd::lzd(void)
{
}

lzd::lzd(const tstring & tag)
    : LZDataBase(tag)
{
}

lzd::lzd(const tstring & tag, const tstring & value_)
    : LZDataBase(tag, value_)
{
}

lzd::~lzd(void)
{
}

LZDataPtr lzd::newOne(const tstring & tag, const tstring & value_)
{
    return new lzd(tag, value_);
}


void lzd::print(tostream & out, int depth)
{
    for (DataConstIterator it = m_children.begin(); it != m_children.end(); ++it)
    {
        LZDataPtr ptr = *it;
        if ( ptr->countChildren() > 0 || ptr->value().empty())
        {
            printTable(out, depth);
            out<<ptr->tag()<<std::endl;

            printTable(out, depth);
            out<<_T("{")<<std::endl;

            ptr->print(out, depth+1);

            printTable(out, depth);
            out<<_T("}")<<std::endl;
        }
        else
        {
            printTable(out, depth);
            out<<ptr->tag()<<_T(" = ")<<convertValue(ptr->asString())<<std::endl;
        }
    }
}


bool lzd::load(const tstring & fname)
{
    std::string buffer;
    if(!getfs()->readString(fname, buffer)) return false;

    size_t cp = cParser::parseCodingHeader(buffer.c_str(), buffer.size());

    std::wstring fileBuffer;
    if(!charToWChar(fileBuffer, buffer, cp)) return false;

    return loadFromBuffer(&fileBuffer[0], fileBuffer.size());
}

bool lzd::save(const tstring & fname)
{
    std::wostringstream stream;
    stream<<_T("#coding = ")<<Encode::utf8<<_T("\n\n");
    print(stream, 0);

    std::string buffer;
    if(!wcharToChar(buffer, stream.str(), CP::utf8)) return false;

    return getfs()->writeString(fname, buffer);
}


}//namespace Lazy