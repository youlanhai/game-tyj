// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "../lzdheader.h"

#ifdef LZDATA_DYNAMIC
#   ifdef _DEBUG
#       pragma comment(lib,"../dynamic_debug/LZDatad_d.lib")
#   else
#       pragma comment(lib,"../dynamic_release/LZDatad.lib")
#   endif
#endif

#include <iostream>
#include <cstdlib>
using namespace std;


void safePrint(LZDataPtr ptr)
{
    if (ptr)
    {
        cout<<ptr->tag()<<" = "<<ptr->asString()<<endl;
    }
    else cout<<"get faild."<<endl;
}

void safeGet(LZDataPtr root, std::string cmd)
{
    cout<<"get "<<cmd<<": ";
    safePrint(root->read(cmd));
}


char pstr1[] = "name = test.lzd\n\
path = c:/data/readme.txt\n\
node1\n\
{\n\
    node2\n\
    { \n\
        node3{ a =100 }\n\
        a = 10\n\
        b = 9 #this is comment\n\
        c = abcde; d = 1213 ;\n\
    } \n\
    d = {{{\\#\\#\\# all you see, is a string. \\#\\}\\}\\}\\} #this is comment\n\
    path = c:\\a\\b\\c\\d #comment\n\
}\n\
wrong wrong2 = 10,20";

class TestSmartPtr
{
public:
	TestSmartPtr(int id=0)
		: id_(id)
	{
		cout<<"TestSmartPtr construct at "<<(unsigned int)this<<endl;
	}

	virtual ~TestSmartPtr()
	{
		cout<<"TestSmartPtr destruct at "<<(unsigned int)this<<endl;
	}

	void test()
	{
		cout<<"TestSmartPtr::test"<<endl;
	}

	int id_;
};

class TestA : public TestSmartPtr
{
public:
	TestA(int id, int msg)
		: TestSmartPtr(id)
		, msg_(msg)
	{

	}
	~TestA()
	{

	}

	int msg_;

};

int _tmain(int argc, _TCHAR* argv[])
{
    
#if 1
	//测试智能指针
	SmartPtr<TestSmartPtr> ptr1(new TestSmartPtr(100));
	SmartPtr<TestSmartPtr> ptr2 = ptr1;
	SmartPtr<TestSmartPtr> ptr3;

	//ptr3->test(); //test assert

	ptr2 = ptr2; //test self copy
	ptr2 = ptr1; 

	cout<<"ptr1->id: "<<ptr1->id_<<endl;
	ptr1->test();

	cout<<"ptr1 ref: "<<ptr1.getRefCount()<<endl;
	cout<<"ptr2 ref: "<<ptr2.getRefCount()<<endl;
	cout<<"ptr3 ref: "<<ptr2.getRefCount()<<endl;

	cout<<"ptr1 == ptr2: "<<(ptr1 == ptr2)<<endl;
	cout<<"ptr1 == ptr3: "<<(ptr1 == ptr3)<<endl;

	SmartPtr<TestA> ptr4(new TestA(99, 100));
	SmartPtr<TestSmartPtr> ptr5 = ptr4;
	
	ptr5->test();

	ptr4 = ptr5;
	cout<<"ptr4 == ptr5: "<<(ptr4 == ptr5)<<endl;

	SmartPtr<int> ptr6(new int(9));
	//cout<<"ptr1 != ptr6:"<<(ptr1 != ptr6)<<endl;
	//ptr4 = ptr6;

#else
    LZDataPtr root = lzd::createFromBuffer(pstr1, strlen(pstr1));
    
    cout<<"\n-----load from buffer----"<<endl;
    root->print(cout);
    cout<<"------------------------\n"<<endl;
    

    safeGet(root, "name");
    safeGet(root, "node1");
    safeGet(root, "node1/node2/b");
    safeGet(root, "node1/node2/node3");
    safeGet(root, "node1/node2/node3/a");
    cout<<"node1/node2/node3/a + 11"<<root->readInt("node1/node2/node3/a")+11<<endl;

    root->writeString("xxx","nihao");
    root->writeString("node1/path", "e:\\a\\b\\c\\d");
    root->writeBool("noden/off", true);
    root->writeInt("noden/year", 2012);
    root->writeFloat("noden/week", 4.0f);
    root->writeDouble("noden/time", 13.06);

	if (root->save(root->readString("name")))
	{
		cout<<"save to file success."<<endl;
	}
	else 
	{
		cout<<"save to file faild."<<endl;
		return 0;
	}
    
    root = lzd::createFromFile("test.lzd");
    if (!root)
    {
        cout<<"open test file error!"<<endl;
        return 0;
    }
    cout<<"\n-----load from file----"<<endl;
    root->print(cout);
    cout<<"------------------------\n"<<endl;

    safeGet(root, "name");
    safeGet(root, "node1/d");
    safeGet(root, "node1/path");
    safeGet(root, "node1/node2/b");
    safeGet(root, "node1/node2/node3/a");

    LZDataPtr xmlRoot = xml::createFromFile("test.xml");
    if (!xmlRoot)
    {
        cout<<"load from file faild!"<<endl;
        return 0;
    }
    cout<<"\n-----load from file----"<<endl;
    xmlRoot->print(cout);
    cout<<"------------------------\n"<<endl;

    xmlRoot->save("test.xml");
#endif
    //system("pause");
	return 0;
}

