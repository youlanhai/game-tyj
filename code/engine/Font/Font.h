#pragma once

#include "FontTextrue.h"

namespace Lazy
{

//文字对齐方式
namespace TextAlign
{
    const int left      = 1;
    const int center    = 2;
    const int right     = 4;
    const int top       = 8;
    const int bottom    = 16;
    const int vcenter   = 32;
    const int mutiLine  = 64;
}

//文字信息。用于渲染一个文字。
struct WordInfo
{
    //纹理坐标
    float x1, y1;
    float x2, y2;

    //排版偏移坐标
    float dx, dy;
    float dw, dh;

    //排版步进尺寸
    float ax, ay;

    //文字纹理
    LPDIRECT3DTEXTURE9 pTexture;
};

//////////////////////////////////////////////////////////////////////////

//存储一个字符的图像纹理信息
struct CharGlyphData
{
    short advanceX;//本字符在文本排版中的步进宽度。
    short advanceY;//本字符在文本排版中的步进高度。

    //字符的步进宽度和步进高度构成了外围Rect，像素数据Bitmap构成了内围Rect，
    //外围Rect和内围Rect之间的空隙就是左边距，右边距，上边距，下边距。
    //这里只需要记录左边距和上边距即可。
    short leftMargin;
    short topMargin;
    short bitmapWidth;//本字符像素数据的宽度。
    short bitmapHeight;//本字符像素数据的高度。

    //纹理坐标
    short texU;
    short texV;

    //文字纹理
    LPDIRECT3DTEXTURE9 pTexture;

    //是否合法
    bool valid;
};

//////////////////////////////////////////////////////////////////////////
//字体信息 将字体文件名、字体大小等信息，转换成字体名
//字体名称中的信息必须以#连接
//////////////////////////////////////////////////////////////////////////
struct FontInfo
{
    tstring faceName;
    int     height;
    int     bold;

    FontInfo();

    FontInfo(const tstring & fontName);

    void from(const tstring & fontName);

    void to(tstring & fontName);

    bool valid() const;
};
//////////////////////////////////////////////////////////////////////////
//字体
//////////////////////////////////////////////////////////////////////////
class Font
{
public:
    typedef std::map<wchar_t, CharGlyphData> GlyphMap;

    Font(void *pFace, int height);
    ~Font(void);

    bool getWord(wchar_t ch, WordInfo & word);

    CharGlyphData* getGlyph(wchar_t ch);

    bool drawWord(float x, float y, wchar_t ch, DWORD cr, float & ax, float & ay);
    bool drawText(float x, float y, const std::wstring & text, DWORD cr);
    bool drawText(const std::wstring & text, const CRect & rc, DWORD style, DWORD cr);

    bool getTextSize(int & width, int & height, const std::wstring & text);
    bool getMaxSize(int & width, int & height, const std::wstring & text, DWORD style);

    void parseTextToLines(
        std::vector<std::wstring> & textLines, 
        std::vector<float> & widthLines,
        const std::wstring & text,
        const CRect & rc,
        DWORD style);

    bool getCursorPos(const std::wstring & text, const CRect & rc, DWORD style, 
        DWORD cursor, float & x, float & y, float & h);
    bool getCursorByPos(const std::wstring & text, const CRect & rc, DWORD style, 
        DWORD & cursor, float x, float y);

    bool fillWordToTexture(LPDIRECT3DTEXTURE9 pDestTex, int dx, int dy, wchar_t ch, D3DXCOLOR color, int & ax, int & ay);
    bool fillTextToTexture(LPDIRECT3DTEXTURE9 pDestTex, int dx, int dy, const std::wstring & text, DWORD color);

private:

    CharGlyphData* createGlyph(wchar_t ch);

    Font(const Font &);
    const Font & operator=(const Font &);
private:
    void            *m_pFace;
    int             m_height;
    FontTextureMgr  m_texMgr;
    GlyphMap        m_glyphMap;
};
    
typedef std::shared_ptr<Font> FontPtr;

//////////////////////////////////////////////////////////////////////////
//字体管理器
//////////////////////////////////////////////////////////////////////////
class FontMgr
{
public:
    typedef std::map<tstring, void*> FaceMap;
    typedef std::map<tstring, FontPtr> FontMap;

    /** 由于字体文件名可能过长，使用key来代替文件名会更方便。*/
    bool registerFontFile(const tstring & fontFile, const tstring & key);

    FontPtr getFont(const tstring & fontName);

    /** 初始化字体 */
    bool init(LPDIRECT3DDEVICE9 pDevice);

    /** 释放资源 */
    void fini();

    LPDIRECT3DDEVICE9 device(){ return m_pd3dDevice; }

    /** 唯一实例 */
    static FontMgr * instance();

private:
    FontMgr();
    ~FontMgr();
    FontMgr(const FontMgr &);
    const FontMgr & operator = (const FontMgr &);

    void* getFace(const tstring & faceName);

    void* createFace(const tstring & fontFile);

private:
    FontMap m_fontMap;
    FaceMap m_faceMap;
    LPDIRECT3DDEVICE9 m_pd3dDevice;

    static FontMgr *s_pMgr;
};

}//namespace Lazy