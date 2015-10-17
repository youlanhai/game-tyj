// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "../Utility.h"
#include "../MemoryPool.h"

#include <cassert>
#include <iostream>
#include <sstream>
using namespace std;

#ifdef _UNICODE
#	define TCout wcout
#else
#	define TCout cout
#endif

void printAllocater(const Lazy::Allocater & alloc)
{
    cout<<"allocater size:"<<alloc.bytes()<<endl;
    cout<<"allocater free:"<<alloc.numFreeNode()<<endl;
    cout<<"allocater num allocater:"<<alloc.numAllocater()<<endl;
}

void testTrim(Lazy::tstring str, const Lazy::tstring & o)
{
    TCout<<_T("src str:'")<<str;
    trimString(str);
    TCout<<_T("' trim:'")<<str<<_T("'")<<endl;

    assert(str == o);
}

int _tmain(int argc, _TCHAR* argv[])
{
    Lazy::defaultCodePage = CP_UTF8;
#if 1
    //test log
	INIT_LOG(_T("test.log"));

	XWRITE_LOG(_T("测试log:%d, %f, %s"), 12, 3.4, _T("哈哈,abc"));
#endif

#if 1
    //test guid
	TCout<<_T("generateGUID:")<<Lazy::generateGUID()<<endl;
#endif

#if 1
    //test trim
	Lazy::tstring strTest(_T(" \t hello world \r\n "));
	TCout<<_T("strTest before trim: '")<<strTest<<_T("'")<<endl;

	trimString(strTest);
	TCout<<_T("strTest after trim: '")<<strTest<<_T("'")<<endl;

    Lazy::eraseToEnd(strTest, _T('o'));
    TCout<<_T("eraseToEnd 'o':")<<strTest<<endl;

    testTrim(_T(""), _T(""));
    testTrim(_T("x"), _T("x"));
    testTrim(_T("x "), _T("x"));
    testTrim(_T(" x"), _T("x"));
    testTrim(_T(" x "), _T("x"));
    testTrim(_T("\t\r\nx\n"), _T("x"));
#endif

#if 1
    //test format string
	formatString(strTest, _T("formatString:%d, %0.2f, %s"), 12, 3.4, _T("hello,world!"));
	TCout<<strTest<<endl;

    Lazy::StringConvTool convTool;
    convTool.formatFrom(_T("formatFrom: %d, %0.2f, %s"), 12, 3.4, _T("hello,world!"));
    TCout<<convTool.asString()<<endl;
#endif

#if 1
    //test format file name
    Lazy::tstring path = _T("F:\\workspace\\utility\\Test.cpp");
    TCout<<_T("\ntest format file name")<<std::endl;
    TCout<<_T("orignal path:")<<path<<std::endl;

    Lazy::formatSlash(path);
    TCout<<_T("formatSlash:")<<path<<std::endl;

    Lazy::tstring dir = Lazy::getFilePath(path);
    TCout<<_T("getFilePath:")<<dir<<std::endl;
    TCout<<_T("    getFilePath:")<<Lazy::getFilePath(dir)<<std::endl;
    
    Lazy::formatDirName(dir);
    TCout<<_T("    fomatDirName:")<<dir<<endl;

    TCout<<_T("getPathFile:")<<Lazy::getPathFile(path)<<endl;
    TCout<<_T("getFileExt:")<<Lazy::getFileExt(path)<<endl;
    
    Lazy::tstring temp(path);
    Lazy::removeFilePath(temp);
    TCout<<_T("removeFilePath:")<<temp<<endl;

    temp = path;
    Lazy::removeFileExt(temp);
    TCout<<_T("removeFileExt:")<<temp<<endl;

#endif

#if 1
    //test file system
    Lazy::getfs()->addResPath(_T("res"));

    Lazy::tstring fname = _T("data/test.txt");
    std::string content = "hello world中文啊abcd.";

    Lazy::getfs()->writeString(fname, content);
    content.clear();

    //fname += _T("hh");
    if(!Lazy::getfs()->readString(fname, content))
    {
        TCout<<_T("open file failed!")<<fname<<endl;
    }
    else
    {
        cout<<"read from file:" << content <<endl;
    }
    
#endif

#if 1
    //test memory pool
    Lazy::MemoryPool::init();
    Lazy::tstring memoryInfo;
    Lazy::MemoryPool::instance()->report(memoryInfo);
    TCout<<L"\n---"<<memoryInfo;


    Lazy::Allocater alloc0(0);
    Lazy::Allocater alloc1(11);
    Lazy::Allocater alloc2(1020);

    //Lazy::Allocater alloc3(1024+1);

    //normal test
    Lazy::Allocater alloc(sizeof(int));
    cout<<"\nallocater init"<<endl;
    printAllocater(alloc);

    int * pi = (int*)alloc.alloc();
    *pi = 2;
    alloc.free(pi);
    cout<<"\nallocater alloc and free one"<<endl;
    printAllocater(alloc);

    for(int i=0; i<32; ++i) alloc.alloc();
    cout<<"\nallocater alloc 32"<<endl;
    printAllocater(alloc);

    alloc.alloc();
    cout<<"\nallocater alloc 1"<<endl;
    printAllocater(alloc);

    //free invalid memory
    alloc.free(&alloc1);

    std::vector<void*> memory;

    for(size_t i=1; i<=100000; ++i)
    {
        memory.push_back(Lazy::MemoryPool::instance()->alloc(i % Lazy::MaxNodeSize));
    }

    Lazy::MemoryPool::instance()->report(memoryInfo);
    TCout<<L"\n---"<<memoryInfo;

    for(size_t i=0; i<memory.size(); ++i)
    {
        Lazy::MemoryPool::instance()->free(memory[i]);
    }

    Lazy::MemoryPool::instance()->report(memoryInfo);
    TCout<<L"\n---"<<memoryInfo;

    TCout<<L"delete Free Allocator"<<endl;
    Lazy::MemoryPool::instance()->deleteFreeAllocator();

    Lazy::MemoryPool::instance()->report(memoryInfo);
    TCout<<L"\n---"<<memoryInfo;

#endif

#if 1
    //test data section
    Lazy::LZDataPtr root = new Lazy::lzd();
    if(root->load(_T("test.lzd")))
    {
        root->print(wcout);
        if(!root->save(_T("test2.lzd")))
        {
            cout<<"save data failed!"<<endl;
        }
    }
    else
    {
        cout<<"open lzd file failed!"<<endl;
    }

    Lazy::MemoryPool::instance()->report(memoryInfo);
    TCout<<L"\n---"<<memoryInfo;

    root = NULL;//force delete

#endif


    Lazy::MemoryPool::fini();
    Lazy::MemoryCheck::instance()->release();

	return 0;
}

