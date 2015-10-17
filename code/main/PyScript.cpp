
#include "LZ3DEngine\LZ3DEngine.h"
#include "PyScript.h"
#include "PyWraper\pyWraper.h"


#include <string>

#define ENTITIES_PATH "res\\entities"
#define PYTHON_HOME "..\\code\\third_part\\python"

using namespace boost::python;

UINT msgFilter[] = {
    WM_LBUTTONDOWN, WM_LBUTTONUP, WM_LBUTTONDBLCLK,
    WM_RBUTTONDOWN, WM_RBUTTONUP, WM_RBUTTONDBLCLK,
    WM_MOUSEMOVE, WM_MOUSEWHEEL, WM_KEYDOWN, WM_KEYUP};
int fileterCount = 10;

cPyScript::cPyScript(void)
{
}

cPyScript::~cPyScript(void)
{
}


bool cPyScript::init(void)
{
    Py_SetPythonHome(PYTHON_HOME);

    Py_Initialize();
    if(!Py_IsInitialized())
    {
        WRITE_LOG(_T("------python初始化失败"));
        return false;
    }
    WRITE_LOG(_T("------python 初始化成功"));
    try
    {
        initLazy();
        object mainModule = import("__main__");
        object mainNamespace = mainModule.attr("__dict__");
        exec("import sys", mainNamespace, mainNamespace);

        std::string cmd;
        Lazy::formatStringA(cmd, "sys.path.append('%s')", ENTITIES_PATH);
        exec(cmd.c_str(), mainNamespace, mainNamespace);

        object entry = import("Entry");
        object enter = entry.attr("enter");
        return extract<bool>(enter());
    }
    catch(...)
    {
        if (PyErr_Occurred())
            PyErr_Print();
        return false;
    }
    return true;
}

void cPyScript::release(void)
{
    
    try
    {
        object entry = import("Entry");
        object leave = entry.attr("leave");
        leave();
    }
    catch(...)
    {
        if (PyErr_Occurred())
            PyErr_Print();
    }
}

//窗口过程，内部使用
UINT cPyScript::msgProc(UINT uMsg, WPARAM wParam,LPARAM lParam)
{
    if (uMsg == WM_CLOSE)
    {
        endGame();
        return 1;
    }
    int i = 0;
    for (; i<fileterCount; ++i)
    {
        if (msgFilter[i]==uMsg)
        {
            break;
        }
    }
    if (i>=fileterCount)
    {
        return 0;
    }
    try
    {
        CPoint pt;
        pt.formatLParam(lParam);
        object mod = import("Game");
        object foo = mod.attr("messageProc");
        foo(uMsg, wParam, lParam);
    }
    catch(...)
    {
        if (PyErr_Occurred())
            PyErr_Print();
    }

    if (uMsg==WM_KEYUP && wParam==VK_ESCAPE)
    {
        return 1;
    }
    return 0;
}

CSceneFog* cPyScript::getFog(void)
{
    object mod = import("Game");
    object foo = mod.attr("getFog");
    return extract<CSceneFog*>(foo());
}


void cPyScript::endGame(void)
{
    try
    {
        object mod = import("Game");
        object foo = mod.attr("endGame");
        foo();
    }
    catch(...)
    {
        if (PyErr_Occurred())
            PyErr_Print();
    }
}