#pragma once

#include "LZ3DEngine\Base.h"

#include <boost/python.hpp>

class CSceneFog;

class cPyScript : public IBase
{
public:
    cPyScript(void);
    ~cPyScript(void);

    bool init(void);

    void release(void);

    UINT msgProc(UINT uMsg, WPARAM wParam,LPARAM lParam);

    CSceneFog* getFog(void);

    void endGame(void);
};
