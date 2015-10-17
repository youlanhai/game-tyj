#pragma once


///自定义顶点格式
struct TerrinVertex
{
    D3DXVECTOR3 pos;	//< 未经过坐标变换后的点
    D3DXVECTOR3 nml;	//< 顶点法向量
    float u,v;		//< 纹理坐标
    static DWORD FVF;//<灵活顶点格式
    static int SIZE;
};


/** 地形数据。高度图。*/
class TerrinData : public IBase
{
public:
    TerrinData(void);
    ~TerrinData(void);

    /** 加载高度图数据。*/
    bool loadHeightMap(
        std::string name, 
        int vrow, 
        int vcol,
        float squaresize,
        float heightScale,
        bool useOneTex);

    /** 是否在地图范围中。*/
    bool isInBound(float x, float z);

    /** 获得物理高度*/
    float getPhysicalHeight(float x, float z);

    /** 获得高度图高度。*/
    float getHeight(int r, int c);

    /** 得到地图实际宽度。*/
    float width(void){ return m_width; }

    /** 得到地图实际高度。*/
    float height(void){ return m_height; }

    /** 顶点行数。*/
    int vrows(void){ return m_vrows; }

    /** 顶点列数。*/
    int vcols(void){ return m_vcols; }

    float squareSize(void){ return m_squareSize; }

    bool useOneTex(void){ return m_useOneTex; }

    /** 线性插值*/
    float lerp(float a, float b, float t);

protected:
    std::string         m_mapName;
    std::vector<BYTE>   m_vecRaw;    //< 地形高度数据
    int     m_vrows;	    //< 正方形网格行顶点数
    int     m_vcols;	    //< 正方形网格列顶点数
    float   m_squareSize;   //< 每个方格大小
    float   m_width;
    float   m_height;
    float   m_heightScale;   //< 高度缩放系数
    bool    m_useOneTex;
};
