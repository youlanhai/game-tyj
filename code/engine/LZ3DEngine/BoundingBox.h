/** 
* @file BoundingBox.h 
* @author 游蓝海
* @data 2011-10-9
*/
#pragma once
#include "base.h"
#include "Mesh.h"

///BoundingBox.h 
/**包围盒类*/
class LZDLL_API CBoundingBox : public IBase
{
public:
    ///构造函数
    CBoundingBox(void);

    ///析构函数
    ~CBoundingBox(void);

    ///创建包围盒
    bool create(ID3DXMesh *pMesh);

    ///渲染包围盒
    void render(IDirect3DDevice9 *pDevice,D3DXMATRIX * pMat);

    ///点是否在包围盒中
    bool isPointIn(const D3DXVECTOR3 & pos);

    ///设置缩放系数。控制包围盒的缩放，用于检测点在包围盒内。
    void setScale(float s){ m_fScale = s; }

protected:
    D3DXVECTOR3 m_vecMin;   //< 最小坐标
    D3DXVECTOR3 m_vecMax;   //< 最大坐标
    float m_fScale;         //< 缩放系数

public:
    static ID3DXMesh *m_pMeshBox;//< 包围盒网格模型

    ///创建包围盒网格模型
    static bool createMeshBox(IDirect3DDevice9 *pDevice);

    ///销毁包围盒网格模型
    static void destoryMeshBox(void);
};
