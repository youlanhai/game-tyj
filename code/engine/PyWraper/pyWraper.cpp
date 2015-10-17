
#include "stdafx.h"
#include "Header.h"
#include "..\LZ3DEngine\LZ3DEngine.h"

#include "pyWraper.h"
#include "WrapperDataLoader.h"
#include "WrapperUI.h"
#include "WrapperTool.h"
#include "WrapperMisc.h"
#include "WrapperEntity.h"
#include "WrapperMusic.h"

//////////////////////////////////////////////////////////////////////////
//生成导出模块
BOOST_PYTHON_MODULE(Lazy)
{
    //包装数据加载器
    wrapperDataLoader();
    //包装工具
    wrapperTool();
    //包装实体
    wrapperEntity();
    //包装音乐
    wrapperMusic();
    //包装杂项
    wrapperMisc();
    //包装UI
    wrapperUI();
} 
