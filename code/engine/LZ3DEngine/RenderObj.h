
#pragma once
#include <d3dx9.h>
#include "Base.h"

/*可渲染对象接口*/
class LZDLL_API IRenderObj 
{
public:
    virtual void render(IDirect3DDevice9 * pDevice) = 0;
    virtual void update(float fElapse) = 0;
};

class LZDLL_API IUpdate : public IRenderObj
{
public:
    virtual void render(IDirect3DDevice9 * pDevice){}
};

class LZDLL_API IRender : public IRenderObj
{
public:
    virtual void update(float fElapse){}
};