#ifndef LZ3DENGINE_RENDER_STATE_H
#define LZ3DENGINE_RENDER_STATE_H

#include "Commen.h"

namespace DefaultRS
{
    const DWORD ZEnable = TRUE;
    const DWORD CullMode = D3DCULL_NONE;

    const DWORD AlphaTestEnable = TRUE;
    const DWORD AlphaRef = 0;
    const DWORD AlphaFunc = D3DCMP_GREATER;

    const DWORD TextureMinFilter = D3DTEXF_LINEAR;
    const DWORD TextureMagFilter = D3DTEXF_LINEAR;
    const DWORD TextureAdressU = D3DTADDRESS_WRAP;
    const DWORD TextureAdressV = D3DTADDRESS_WRAP;

    const DWORD AlphaBlendEnable = TRUE;
    const DWORD SrcBlend = D3DBLEND_SRCALPHA;
    const DWORD DstBlend = D3DBLEND_INVSRCALPHA;

    const DWORD LightingEnable = TRUE;
    const DWORD SpecularEnable = FALSE;

    const DWORD ShadeMode = D3DSHADE_GOURAUD;

    const DWORD FillMode = D3DFILL_SOLID;

}

#endif //LZ3DENGINE_RENDER_STATE_H
