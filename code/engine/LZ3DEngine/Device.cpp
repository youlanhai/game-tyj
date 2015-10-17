#include "Commen.h"
#include "Device.h"

static DevStack s_stack;
DevStack * getDevStack()
{
    return &s_stack;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

DevStack::DevStack()
    : m_pDevice(NULL)
{
}

DevStack::~DevStack(void)
{
    SafeRelease(m_pDevice);
}

void DevStack::setDevice(LPDIRECT3DDEVICE9 pDevice)
{
    SafeRelease(m_pDevice);

    m_pDevice = pDevice;
    if (m_pDevice)
    {
        m_pDevice->AddRef();
    }
}

void DevStack::pushRender(D3DRENDERSTATETYPE state, DWORD value)
{
    DWORD old;
    m_pDevice->GetRenderState(state, &old);
    m_renderState.push_back(old);

    m_pDevice->SetRenderState(state, value);
}
void DevStack::popRender(D3DRENDERSTATETYPE state)
{
    m_pDevice->SetRenderState(state, m_renderState.back());
    m_renderState.pop_back();
}

void DevStack::pushSampler(DWORD sampler, D3DSAMPLERSTATETYPE state, DWORD value)
{
    DWORD old;
    m_pDevice->GetSamplerState(sampler, state, &old);
    m_samplerState.push_back(old);

    m_pDevice->SetSamplerState(sampler, state, value);
}
void DevStack::popSampler(DWORD sampler, D3DSAMPLERSTATETYPE state)
{
    m_pDevice->SetSamplerState(sampler, state, m_samplerState.back());
    m_samplerState.pop_back();
}

void DevStack::pushTextureStage(DWORD stage, D3DTEXTURESTAGESTATETYPE state, DWORD value)
{
    DWORD old;
    m_pDevice->GetTextureStageState(stage, state, &old);
    m_textureState.push_back(old);

    m_pDevice->SetTextureStageState(stage, state, value);
}

void DevStack::popTextureStage(DWORD stage, D3DTEXTURESTAGESTATETYPE state)
{
    m_pDevice->SetTextureStageState(stage, state, m_textureState.back());
    m_textureState.pop_back();
}