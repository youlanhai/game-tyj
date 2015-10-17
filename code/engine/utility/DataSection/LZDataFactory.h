#pragma once

#include "LZDataBase.h"

namespace Lazy
{

class LZDataFactory
{
public:
    LZDataFactory();
    virtual ~LZDataFactory();

    static LZDataPtr create(int type);
};

}//namespace Lazys