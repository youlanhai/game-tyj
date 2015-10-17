#pragma once

#define LZ3DENGINE_STATIC_LINK

#ifdef LZ3DENGINE_STATIC_LINK
#   define LZDLL_API
#else
#   ifdef LZ3DENGINE_EXPORTS
#       define LZDLL_API __declspec(dllexport)
#   else
#       define LZDLL_API __declspec(dllimport)
#   endif
#endif

#include <windows.h>
#include <tchar.h>

#include "../utility/SmartPtr.h"

using Lazy::IBase;
using Lazy::RefPtr;
