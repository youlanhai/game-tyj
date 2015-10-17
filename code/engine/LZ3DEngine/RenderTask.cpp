
#include "Commen.h"
#include "RenderTask.h"

#if 0
//////////////////////////////////////////////////////////////////////////
CRenderTask::CRenderTask(void)
{
}

CRenderTask::~CRenderTask(void)
{
}

void CRenderTask::update(float fElapse)
{
    lock(true);
    for (VisitIterator it = begin();  it != end();  ++it )
    {
        if (it->canOperate())
        {
            (it->obj())->update(fElapse);
        }
    }
    lock(false);
    removeDead();
}


void CRenderTask::render(IDirect3DDevice9 * pDevice)
{
    lock(true);
    for (VisitIterator it = begin();  it != end();  ++it )
    {
        if (it->canOperate())
        {
            (it->obj())->render(pDevice);
        }
    }
    lock(false);
    removeDead();
}
#endif