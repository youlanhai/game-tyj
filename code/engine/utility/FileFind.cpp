#include "StdAfx.h"
#include "FileFind.h"

namespace Lazy
{


bool cFindFile::findFirst(const tstring & name)
{
	reset();

	ZeroMemory(&m_data, sizeof(m_data));
	m_handle = ::FindFirstFile(name.c_str(), &m_data);
	if (INVALID_HANDLE_VALUE == m_handle)
	{
		m_handle = NULL;
	}
	return m_handle != NULL;
}

bool cFindFile::findNext()
{
	if (!valid())
	{
		return false;
	}
	return !!::FindNextFile(m_handle, &m_data);
}

void cFindFile::reset()
{
	if (valid())
	{
		::FindClose(m_handle);
		m_handle = NULL;
	}
}

}//namespace Lazy

