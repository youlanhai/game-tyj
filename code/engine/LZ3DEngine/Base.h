#pragma once

#if 1
#   pragma warning(disable:4251)
#   pragma warning(disable:4275)
#endif

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

#include "RefPtr.h"