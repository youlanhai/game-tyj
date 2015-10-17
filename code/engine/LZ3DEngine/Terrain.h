/** 
* @file Terrain.h
* @author 游蓝海
* @data 2011-10-8
*/

#pragma  once


#include "Square.h"

/** 
* @class CTerrain 地形类
*/
class LZDLL_API CTerrain : public CSquare
{
public :
    ///构造函数
    CTerrain(void);
    
    ///析构函数
    ~CTerrain();

   /**初始化。
    * @param [in] pDevice:direct3D设备对象指针。
    * @param [in] rawFileName:高度图文件名。
    * @param [in] rowVertexs:一行的顶点数。
    * @param [in] colVertexs:一列的顶点数。
    * @param [in] size:地图方格尺寸。
    * @param [in] heightScale：高度缩放系数。
    * @param [in] bOneTex:只使用一张纹理。
    */
	bool init(IDirect3DDevice9 *pDevice,
        const std::string & szRawFile,
        int rowVertexs, 
        int colVertexs, 
        int size,
        float fHeightScale,
        bool bOnTexture = true);

    /** 渲染 */
    void render(IDirect3DDevice9 *Device);


    /**获得物理高度。*/
    float getPhysicalHeight(float x, float z);


public://属性

    ///设置纹理
    void setTexture(IDirect3DTexture9 * pTexture){ m_pTexture = pTexture; }

protected:
    /**获得索引高度。*/
    float getHeight(int r, int c);

    /**更新缓冲区中的数据*/
    bool updateVertex(IDirect3DDevice9 *pDevice); 

    /**计算法线。*/
    void computeTriNormal(
        SquareVertex &v1,
        SquareVertex &v2,
        SquareVertex &v3,
        D3DXVECTOR3 &normal
        );

    /**插值运算*/
    float lerp(float a, float b, float t);

protected:
    bool                    m_bOneTexture;  //< 只使用一张纹理
    std::vector<float>      m_vecRaw;       //< 地形高度数据
    IDirect3DTexture9       *m_pTexture;	//< 纹理
};