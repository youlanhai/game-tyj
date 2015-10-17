#include "StdAfx.h"
#include "LZDataFactory.h"
#include "lzd.h"
#include "xml.h"

namespace Lazy
{

LZDataFactory::LZDataFactory(void)
{
}

LZDataFactory::~LZDataFactory(void)
{
}

/*static*/ LZDataPtr LZDataFactory::create(int type)
{
    return NULL;
}

}//namespace Lazy