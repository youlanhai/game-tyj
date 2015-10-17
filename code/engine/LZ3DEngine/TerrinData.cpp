

#include "Commen.h"
#include "TerrinData.h"
#include "Utilities.h"


DWORD TerrinVertex::FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;//<灵活顶点格式
int TerrinVertex::SIZE = sizeof(TerrinVertex);

TerrinData::TerrinData(void)
{
    m_vrows = 0;	    //< 正方形网格行顶点数
    m_vcols = 0;	    //< 正方形网格列顶点数
    m_squareSize = 0; //< 每个方格大小
    m_width = 0;
    m_height = 0;
    m_heightScale = 1.0f;   //< 高度缩放系数
    m_useOneTex = false; 
}

TerrinData::~TerrinData(void)
{
}

/** 加载高度图数据。*/
bool TerrinData::loadHeightMap(
        std::string name, 
        int vrow, 
        int vcol,
        float squaresize,
        float heightScale,
        bool useOneTex)
{
    m_mapName = name;
    m_vrows = vrow;
    m_vcols = vcol;
    m_squareSize = squaresize;
    m_heightScale = heightScale;
    m_useOneTex = useOneTex;
    m_width = m_vcols*m_squareSize;
    m_height = m_vrows*m_squareSize;

    debugMessage(_T("INFO: TerrinData::loadHeightMap width=%f, height=%f, vrows=%d, vcols=%d, squaresize=%f"), 
        m_width, m_height, m_vrows, m_vcols, m_squareSize);
    assert(m_width && m_height && "数据错误！");

    //从文件中读取地形数据。
    std::ifstream fin;
    fin.open(name.c_str(), std::ios::binary);
    if (!fin.good())
    {
        XWRITE_LOGA("打开高度图%s失败！", name.c_str());
        return false;
    }

    int vertexCount = m_vrows*m_vcols;

    //重置raw数组的大小。
    m_vecRaw.resize(vertexCount);

    fin.read( (char*)&m_vecRaw[0] , vertexCount);
    if (fin.bad())
    {
        WRITE_LOG(_T("读取高度数据失败！"));
        return false;
    }
    fin.close();

    return true;
}


/** 是否在地图范围中。*/
bool TerrinData::isInBound(float x, float z)
{
    x += m_width/2.0f;
    z = m_height/2.0f - z;
    if (x>=0.0f && x<m_width && z>=0.0f && z<m_height)
    {
        return true;
    }
    return false;
}

/** 获得物理高度*/
float TerrinData::getPhysicalHeight(float x, float z)
{
    if(!isInBound(x, z))
    {
        return 0.0f;
    }

    //将地图移动到原点,方便计算
    x = (m_width / 2.0f) + x;
    z = (m_height / 2.0f) - z;

    x /= m_squareSize;
    z /= m_squareSize;

    //计算x,z坐标所在的行列值
    int col = int(x);//向下取整
    int row = int(z);

    // 获取如下图4个顶点的高度
    // 
    //  A   B
    //  *---*
    //  | / |
    //  *---*  
    //  C   D

    float A = getHeight(row,   col);
    float B = getHeight(row,   col+1);
    float C = getHeight(row+1, col);
    float D = getHeight(row+1, col+1);

    float dx = x - col;
    float dz = z - row;


    float height = 0.0f;
    if(dz < 1.0f - dx)//(x,z)点在ABC三角形上
    {
        float uy = B - A;
        float vy = C - A;

        height = A + lerp(0.0f, uy, dx) + lerp(0.0f, vy, dz);//线形插值得到高度
    }
    else//(x,z)点在BCD三角形上
    {
        float uy = C - D;
        float vy = B - D;

        height = D + lerp(0.0f, uy, 1.0f - dx) + lerp(0.0f, vy, 1.0f - dz);
    }

    return height;
}

/** 获得高度图高度。*/
float TerrinData::getHeight(int r, int c)
{
    float y;
    if(r >= 0 && c >= 0 && r <m_vrows && c < m_vcols)
    {
        y =  (float)m_vecRaw[r*m_vcols + c];
    }
    else
    {
        //DebugMsg("getHeight out of bound(%d, %d)", r, c);
        y = 0;
    }
    return y*m_heightScale;
}

/** 线性插值*/
float TerrinData::lerp(float a, float b, float t)
{
    return a - (a*t) + (b*t);
}

//////////////////////////////////////////////////////////////////////////
