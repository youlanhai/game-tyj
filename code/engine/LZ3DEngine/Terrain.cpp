#include "Commen.h"
#include "Utilities.h"
#include "Terrain.h"
#include <fstream>

CTerrain::CTerrain(void)
{
    m_pTexture = NULL;
}

CTerrain::~CTerrain()
{

}

/**初始化。
* pDevice:direct3D设备对象指针。
* rawFileName:高度图文件名。
* rowVertexs:一行的顶点数。
* colVertexs:一列的顶点数。
* size:地图方格尺寸。
* heightScale：高度缩放系数。
* bOneTex:只使用一张纹理。
*/
bool CTerrain::init(IDirect3DDevice9 *pDevice,
    const std::string & szRawFile,
    int rowVertexs, 
    int colVertexs, 
    int size,
    float fHeightScale,
    bool bOnTexture)
{
    m_bOneTexture = bOnTexture; 
    m_nVertexCount = rowVertexs * colVertexs;
    
    //从文件中读取地形数据。
    std::ifstream fin;
    fin.open(szRawFile.c_str(), std::ios::binary);
    if (!fin.good())
    {
        XWRITE_LOGA("打开高度图%s失败！",szRawFile);
        return false;
    }


    std::vector<BYTE> vecHeightMap(m_nVertexCount);

    fin.read( (char*)&vecHeightMap[0] , m_nVertexCount);
    if (fin.bad())
    {
        WRITE_LOG(_T("读取高度数据失败！"));
        return false;
    }
    fin.close();

    //重置raw数组的大小。
    m_vecRaw.resize(m_nVertexCount);
    //缩放地形高度。
    for(size_t i=0; i<(size_t)m_nVertexCount; ++i)
    {
        m_vecRaw[i] = vecHeightMap[i] * fHeightScale;//获得高度数据,乘以缩放系数
    }

    return CSquare::init(pDevice,size,size,rowVertexs-1,colVertexs-1);
}

/*更新缓冲区中的数据*/
bool CTerrain::updateVertex(IDirect3DDevice9 *pDevice)
{
    float dx = - m_nCols * m_nWidth * 0.5F;
    float dy = m_nRows * m_nHeight * 0.5F;

    int verIndex = 0;
    if (m_bOneTexture)//仅使用一张纹理
    {
        float su = 1.0f/(m_nCols * m_nWidth);
        float sv = 1.0f/(m_nRows * m_nHeight);
        //以下是创建网格数据
        for(int y=0; y<m_nRows+1; ++y)
        {
            for(int x=0;x<m_nCols+1;++x)
            {
                m_vecVertex[verIndex].x = x * m_nWidth + dx;
                m_vecVertex[verIndex].y = getHeight(y,x);
                m_vecVertex[verIndex].z = -y * m_nHeight + dy;
                m_vecVertex[verIndex].nx = 0.0f;
                m_vecVertex[verIndex].ny = 0.0f;
                m_vecVertex[verIndex].nz = 0.0f;
                m_vecVertex[verIndex].u = x * m_nWidth * su;
                m_vecVertex[verIndex].v = y * m_nHeight * sv;
                ++verIndex;
            }
        }
    }
    else
    {
        //以下是创建网格数据
        for(int y=0; y<m_nRows+1; ++y)
        {
            for(int x=0;x<m_nCols+1;++x)
            {
                m_vecVertex[verIndex].x = x * m_nWidth + dx;
                m_vecVertex[verIndex].y = getHeight(y,x);
                m_vecVertex[verIndex].z = -y * m_nHeight + dy;
                m_vecVertex[verIndex].nx = 0.0f;
                m_vecVertex[verIndex].ny = 0.0f;
                m_vecVertex[verIndex].nz = 0.0f;
                m_vecVertex[verIndex].u = float(y%2);
                m_vecVertex[verIndex].v = float(x%2);
                ++verIndex;
            }
        }
    }

    //创建索引数据
    verIndex=0;
    for(int y=0; y<m_nRows; ++y)//最右边的点不遍历
    {
        for(int x=0;x<m_nCols; ++x)
        {
            D3DXVECTOR3 normal;
            m_vecIndex[verIndex] = y*(m_nCols+1)+x;
            m_vecIndex[verIndex+1] = m_vecIndex[verIndex]+1;
            m_vecIndex[verIndex+2] = (y+1)*(m_nCols+1)+x;

            computeTriNormal(m_vecVertex[m_vecIndex[verIndex]],
                m_vecVertex[m_vecIndex[verIndex+1]],
                m_vecVertex[m_vecIndex[verIndex+2]],
                normal);

            m_vecVertex[m_vecIndex[verIndex]].nx = normal.x;
            m_vecVertex[m_vecIndex[verIndex]].ny = normal.y;
            m_vecVertex[m_vecIndex[verIndex]].nz = normal.z;

            m_vecVertex[m_vecIndex[verIndex+1]].nx = normal.x;
            m_vecVertex[m_vecIndex[verIndex+1]].ny = normal.y;
            m_vecVertex[m_vecIndex[verIndex+1]].nz = normal.z;

            m_vecVertex[m_vecIndex[verIndex+2]].nx = normal.x;
            m_vecVertex[m_vecIndex[verIndex+2]].ny = normal.y;
            m_vecVertex[m_vecIndex[verIndex+2]].nz = normal.z;


            m_vecIndex[verIndex+3] = m_vecIndex[verIndex]+1;
            m_vecIndex[verIndex+4] = m_vecIndex[verIndex+2]+1;
            m_vecIndex[verIndex+5] = m_vecIndex[verIndex+2];

            computeTriNormal(m_vecVertex[m_vecIndex[verIndex+3]],
                m_vecVertex[m_vecIndex[verIndex+4]],
                m_vecVertex[m_vecIndex[verIndex+5]],
                normal);

            m_vecVertex[m_vecIndex[verIndex+3]].nx = normal.x;
            m_vecVertex[m_vecIndex[verIndex+3]].ny = normal.y;
            m_vecVertex[m_vecIndex[verIndex+3]].nz = normal.z;

            m_vecVertex[m_vecIndex[verIndex+4]].nx = normal.x;
            m_vecVertex[m_vecIndex[verIndex+4]].ny = normal.y;
            m_vecVertex[m_vecIndex[verIndex+4]].nz = normal.z;

            m_vecVertex[m_vecIndex[verIndex+5]].nx = normal.x;
            m_vecVertex[m_vecIndex[verIndex+5]].ny = normal.y;
            m_vecVertex[m_vecIndex[verIndex+5]].nz = normal.z;

            verIndex += 6;
        }
    }

    {//锁定顶点缓冲区
        void* pp = NULL;
        if(FAILED(m_pVertexBuffer->Lock( 0,
            sizeof(SquareVertex)*m_nVertexCount,
            (void**)&pp, 0)))
        {
            return false;
        }
        memcpy(pp,&m_vecVertex[0],sizeof(SquareVertex)*m_nVertexCount);
        m_pVertexBuffer->Unlock();
    }

    {//锁定索引缓冲区
        void *pp = NULL;
        if(FAILED(m_pIndexBuffer->Lock(0,
            m_nIndexCount*sizeof(WORD),
            (void**)&pp,0)))
        {
            return false;
        }
        memcpy(pp,&m_vecIndex[0],m_nIndexCount*sizeof(WORD));
        m_pIndexBuffer->Unlock();
    }

    return true;
}

void CTerrain::render(IDirect3DDevice9 *pDevice)
{
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    pDevice->SetTransform(D3DTS_WORLD, &matWorld);

    pDevice->SetTexture(0,m_pTexture);

    CSquare::render(pDevice);
}


float CTerrain::getPhysicalHeight(float x, float z)
{
    CRect bounds = getBounds();
    if (!bounds.isIn(int(x),int(z)))
    {
        return 0.0f;
    }
    
    //将地图移动到原点,方便计算
    x = (m_nCols * m_nWidth / 2.0f) + x;
    z = (m_nRows * m_nWidth / 2.0f) - z;
    //将地图大小单位化，方便计算
    x /= m_nWidth;
    z /= m_nWidth;
    //计算x,z坐标所在的行列值
    float col = ::floorf(x);//向下取整
    float row = ::floorf(z);

    // 获取如下图4个顶点的高度
    // 
    //  A   B
    //  *---*
    //  | / |
    //  *---*  
    //  C   D

    float A = getHeight((int)row,   (int)col);
    float B = getHeight((int)row,   (int)col+1);
    float C = getHeight((int)row+1, (int)col);
    float D = getHeight((int)row+1, (int)col+1);

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

//线性插值
float CTerrain::lerp(float a, float b, float t)
{
    return a - (a*t) + (b*t);
}

//获得地形高度
float CTerrain::getHeight(int r, int c)
{
    float y;
    if(r >= 0 && c >= 0 && r <= m_nRows && c <= m_nCols)
    {
        y =  (float)m_vecRaw[r*(m_nCols+1) + c];
    }
    else
    {
        y = 0;
    }
    return y;
}


//计算法线
void CTerrain::computeTriNormal(SquareVertex &v1,
                                SquareVertex &v2,
                                SquareVertex &v3,
                                D3DXVECTOR3 &normal)
{
    D3DXVECTOR3 vec1(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);//v2-v1
    D3DXVECTOR3 vec2(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);//v3-v1
    D3DXVec3Cross(&normal, &vec1, &vec2);   //差乘
    D3DXVec3Normalize(&normal, &normal);    //单位化
}