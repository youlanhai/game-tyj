
#include "Commen.h"

#include "FileTool.h"
#include "Utilities.h"

//////////////////////////////////////////////////////////////////////////
//文件操作
//////////////////////////////////////////////////////////////////////////

void formatSlash(std::string & name)
{
    std::replace(name.begin(), name.end(), '\\', '/');
}

void formatDirName(std::string & dirName)
{
    if (dirName.empty())
    {
        return;
    }
    formatSlash(dirName);
    if (dirName[dirName.length()-1] != SLASH_USE)
    {
        dirName += SLASH_USE;
    }
}

std::string getPathFile(const std::string & filename)
{
    std::string tempName(filename);
    formatSlash(tempName);
    size_t pos = tempName.find_last_of(SLASH_USE);
    if (pos == tempName.npos)
    {
        return tempName;
    }
    return tempName.substr(pos+1);
}

std::string getFileName(const std::string & filename)
{
    std::string tempName(filename);
    size_t pos = tempName.find_last_of('.');
    if (pos == tempName.npos)
    {
        return tempName;
    }
    return tempName.substr(0, pos);
}

std::string getFileExt(const std::string & filename)
{
    std::string tempName(filename);
    size_t pos = tempName.find_last_of('.');
    if (pos == tempName.npos)
    {
        return "";
    }
    return tempName.substr(pos+1);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

bool cFindFile::findFirst(const std::string & name)
{
    reset();

    ZeroMemory(&m_data, sizeof(m_data));
    m_handle = ::FindFirstFileA(name.c_str(), &m_data);

    XWRITE_LOGA("last error %d", GetLastError());

    return m_handle != INVALID_HANDLE_VALUE;
}

//必须调用一次findNext才可用。
bool cFindFile::findNext()
{
    if (!valid())
    {
        return false;
    }
    return !!::FindNextFileA(m_handle, &m_data);
}

void cFindFile::reset()
{
    if (valid())
    {
        ::FindClose(m_handle);
        m_handle = NULL;
    }
}


bool findFiles(StringPool & vect, 
               const std::string & path, 
               const std::string & ext,
               bool unionPath)
{
    //XWRITE_LOGA("xxxxx %s", path.c_str());

    vect.clear();

    std::string formatedPath = path;
    formatDirName(formatedPath);

    std::string tempName = formatedPath + ext;
    //XWRITE_LOGA("xxxxx %s", tempName.c_str());

    cFindFile finder;
    if(!finder.findFirst(tempName))
    {
        return false;
    }
    while(finder.findNext())
    {
        if(finder.isDot() || finder.isDir())
        {
            continue;
        }
        if (unionPath)
        {
            vect.push_back(formatedPath + finder.fileName());
        }
        else
        {
            vect.push_back(finder.fileName());
        }
        //XWRITE_LOGA("xxxxx %s", vect.back().c_str());
        std::string nameall = getPathFile(vect.back());
        std::string name = getFileName(nameall);
        XWRITE_LOGA("%s - %s", nameall.c_str(), name.c_str());
    }

    return !vect.empty();
}