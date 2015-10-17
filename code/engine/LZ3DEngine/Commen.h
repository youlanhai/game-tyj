//Comment.h
#ifndef LZ3DENGINE_COMMON_H
#define LZ3DENGINE_COMMON_H

#pragma once

#include <windows.h>
#include <Dbghelp.h>
#include <d3dx9.h>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <string>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <typeinfo>

#define SafeRelease(pObject) if(pObject != NULL){pObject->Release(); pObject = NULL;}
#define SafeDelete(pObject) if(pObject != NULL){delete pObject; pObject = NULL;}
#define SafeDeleteArray(pObject) if(pObject != NULL){delete [] pObject; pObject = NULL;}

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
#define ISKEYDOWN(keyboard,key) (keyboard[key]&0x80)
#define ISKEYUP(keyboard,key)	(!(keyboard[key]&0x80))

#define NOT_IMPLEMENT assert(NULL && "method not implement!")

#include "Base.h"

//#include "Log.h"

#include "Tools2d.h"

class CApp;
LZDLL_API CApp*  getApp(void);

#endif //LZ3DENGINE_COMMON_H
