#pragma once

namespace Lazy
{

//////////////////////////////////////////////////////////////////////////
//文件搜索
//////////////////////////////////////////////////////////////////////////
class cFindFile
{
public:
	cFindFile() : m_handle(NULL) {   }
	~cFindFile(){ reset(); }

	//执行第一次查找
	bool findFirst(const tstring & name);

	//后续查找。必须调用一次findFirst才可用。
	bool findNext();

	bool valid(){ return NULL != m_handle; }

	tstring fileName(){ return m_data.cFileName; }

	bool isDot()
	{ 
		tstring fname = fileName();
		return fname==_T(".") || fname==_T(".."); 
	}

	bool isDir()
	{
		return (m_data.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) != 0;
	}

private:

	void reset();

private:
	WIN32_FIND_DATA m_data;
	HANDLE          m_handle;
};

}//namespace Lazy