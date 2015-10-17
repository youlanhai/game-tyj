#pragma once

#include <Windows.h>
#include <d3dx9.h>

struct Vector3 : public D3DXVECTOR3
{
    float length(){ return D3DXVec3Length(this); }
    float lengthSq(){ return D3DXVec3LengthSq(this); }
    void normal(){ D3DXVec3Normalize(this, this); }
};