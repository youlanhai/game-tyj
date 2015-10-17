#pragma once

typedef std::vector<DWORD> StateStack;

class DevStack
{
    DevStack(const DevStack &);
    const DevStack & operator = (const DevStack &);

public:
    DevStack();
    ~DevStack(void);

    void setDevice(LPDIRECT3DDEVICE9 pDevice);

    void pushRender(D3DRENDERSTATETYPE state, DWORD value);
    void popRender(D3DRENDERSTATETYPE state);

    void pushSampler(DWORD sampler, D3DSAMPLERSTATETYPE state, DWORD value);
    void popSampler(DWORD sampler, D3DSAMPLERSTATETYPE state);

    void pushTextureStage(DWORD stage, D3DTEXTURESTAGESTATETYPE state, DWORD value);
    void popTextureStage(DWORD stage, D3DTEXTURESTAGESTATETYPE state);

private:
    LPDIRECT3DDEVICE9 m_pDevice;

    StateStack m_renderState;
    StateStack m_textureState;
    StateStack m_samplerState;
};

DevStack * getDevStack();
