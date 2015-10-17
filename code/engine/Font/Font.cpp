#include "StdAfx.h"
#include "Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_ERRORS_H
#include FT_OUTLINE_H

namespace Lazy
{

    struct WordVertex
    {
        float x, y, z;
        DWORD color;
        float u, v;

        const static DWORD FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE | D3DFVF_TEX1;
        static DWORD SIZE;
    };
    DWORD WordVertex::SIZE = sizeof(WordVertex);


    void FillPixelData(IDirect3DTexture9* pTexture, 
        const RECT& dest_rect,
        unsigned char* pPixelBuffer, 
        int nWidth,
        int nHeight)
    {
        if (!pTexture || !pPixelBuffer)
        {
            return;
        }

        //Lock顶层贴图。
        UINT uiLevel = 0;
        D3DLOCKED_RECT locked_rect;
        if (pTexture->LockRect(uiLevel, &locked_rect, &dest_rect, D3DLOCK_DISCARD) != D3D_OK)
        {
            return;
        }

        int nIndex = 0;
        for (int y = 0; y < nHeight; ++y)
        {
            unsigned char* pDestBuffer = ((unsigned char*)locked_rect.pBits) + locked_rect.Pitch * y;
            for (int x = 0; x < nWidth; ++x)
            {
                pDestBuffer[x] = pPixelBuffer[nIndex];
                ++nIndex;
            }
        }
        pTexture->UnlockRect(uiLevel);
    }

    void DrawGlyphToTexture(CharGlyphData & stData, unsigned char* pPixelBuffer)
    {
        RECT dest_rect;
        dest_rect.left = stData.texU ;
        dest_rect.right = dest_rect.left + stData.bitmapWidth;
        dest_rect.top = stData.texV;
        dest_rect.bottom = dest_rect.top + stData.bitmapHeight;
        FillPixelData(stData.pTexture, dest_rect, pPixelBuffer, stData.bitmapWidth, stData.bitmapHeight);
    }
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    FontInfo::FontInfo()
        : height(0)
        , bold(0)
    {

    }

    FontInfo::FontInfo(const tstring & fontName)
        : height(0)
        , bold(0)
    {
        from(fontName);
    }

    void FontInfo::from(const tstring & fontName)
    {
        tstring name = fontName;

        int i = 0;
        size_t lastPos = 0;
        size_t pos = 0;

        while(true)
        {
            pos = name.find('#', lastPos);
            if (pos == name.npos)
            {
                pos = name.size();
            }


            if (i == 0)
            {
                faceName = name.substr(lastPos, pos);
            }
            else
            {
                tstring temp = name.substr(lastPos, pos-lastPos);
                int data = 0;
                swscanf(temp.c_str(), _T("%d"), &data);
                if (i == 1)
                {
                    height = data;
                }
                else if (i == 2)
                {
                    bold = data ? 1 : 0;
                }
                else
                {
                    break;;
                }
            }

            if (pos >= name.size())
            {
                break;
            }

            ++i;
            lastPos = pos + 1;
        }
    }

    void FontInfo::to(tstring & fontName)
    {
        bold = bold ? 1 : 0;
        formatString(fontName, _T("%s#%d#%d"), fontName.c_str(), height, bold);
    }

    bool FontInfo::valid() const
    {
        if (faceName.empty())
        {
            return false;
        }
        if(height < 5 || height > 100)
        {
            return false;
        }
        return true;
    }

    
    //////////////////////////////////////////////////////////////////////////

    Font::Font(void *pFace, int height)
        : m_pFace(pFace)
        , m_height(height)
    {

        debugMessage(_T("create font: size=%d"), height);

        if (FontConfig::PreloadAscii)
        {
            for (wchar_t i=0; i<128; ++i)
            {
                createGlyph(i);
            }
        }
    }


    Font::~Font(void)
    {
    }

    bool Font::getWord(wchar_t ch, WordInfo & word)
    {
        const CharGlyphData * pGlyph = getGlyph(ch);
        if (!pGlyph->valid) 
            return false;

        word.ax = float(pGlyph->advanceX);
        word.ay = float(pGlyph->advanceY);
        word.x1 = float(pGlyph->texU) / FontConfig::MaxTextureWidth;
        word.y1 = float(pGlyph->texV) / FontConfig::MaxTextureHeight;
        word.x2 = float(pGlyph->texU + pGlyph->bitmapWidth) / FontConfig::MaxTextureWidth;
        word.y2 = float(pGlyph->texV + pGlyph->bitmapHeight) / FontConfig::MaxTextureHeight;
        word.dx = float(pGlyph->leftMargin);
        word.dy = float(pGlyph->topMargin);
        word.dw = float(pGlyph->bitmapWidth);
        word.dh = float(pGlyph->bitmapHeight);
        word.pTexture = pGlyph->pTexture;

        return true;
    }

    CharGlyphData* Font::getGlyph(wchar_t ch)
    {
        GlyphMap::iterator it = m_glyphMap.find(ch);
        if (it != m_glyphMap.end())
            return &(it->second);

        return createGlyph(ch);
    }

    CharGlyphData* Font::createGlyph(wchar_t ch)
    {

#ifdef _DEBUG
        GlyphMap::iterator it = m_glyphMap.find(ch);
        if (it != m_glyphMap.end())
        {
            assert(0 && "the char has been created!");
        }
#endif

        CharGlyphData * pGLData = &(m_glyphMap[ch]);
        ZeroMemory(pGLData, sizeof(CharGlyphData));

        FT_Face pFace = (FT_Face)m_pFace;
        FT_Set_Pixel_Sizes(pFace, m_height, m_height);

        //启用抗锯齿。
        bool bAntiAliased = true;
        FT_Int32 nLoadFlags = FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | (bAntiAliased ? FT_LOAD_TARGET_NORMAL : FT_LOAD_TARGET_MONO);
        FT_Error nResult = FT_Load_Char(pFace, ch, nLoadFlags);
        if (nResult)
        {
            //字体文件中没有这个字！
            //用一个错误字符表示，比如星号。
            //未完待续。
            debugMessage(_T("SoFreeTypeFont::LoadSingleChar : 字体文件中没有这个字"));
            return pGLData;
        }

        
        const FT_GlyphSlot& theGlyph = pFace->glyph;

        //本类只生成横向排版字形，不处理竖向排版。在非等宽字体下，X方向上的步进是各不相同的。
        pGLData->advanceX = (short)(theGlyph->advance.x >> 6);

        //本类只生成横向排版字形，所以Y方向上的步进是固定的；如果是竖向排版，则X方向上的步进是固定的。
        pGLData->advanceY = m_height;
        pGLData->leftMargin = theGlyph->bitmap_left;
        pGLData->topMargin = m_height - theGlyph->bitmap_top;
        pGLData->bitmapWidth = theGlyph->bitmap.width;
        pGLData->bitmapHeight = theGlyph->bitmap.rows;


        const FT_Bitmap& bitmap = theGlyph->bitmap;
        //一个byte表示一个像素，存储bitmap灰度图。
        unsigned char* pPixelBuffer = 0;
        bool bShouldDeletePixelBuffer = false;

        if(bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
        {
            pPixelBuffer = (unsigned char*)bitmap.buffer;
            bShouldDeletePixelBuffer = false;
        }
        else if(bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
        {
            int width = bitmap.width;
            int height = bitmap.rows;
            pPixelBuffer = new unsigned char[width*height];
            bShouldDeletePixelBuffer = true;
            int nIndex = 0;
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    if (bitmap.buffer[y*width + x/8] & (0x80 >> (x & 7)))
                    {
                        pPixelBuffer[nIndex] = 0xFF;
                    }
                    else
                    {
                        pPixelBuffer[nIndex] = 0x00;
                    }
                    ++nIndex;
                }
            }
        }
        else
        {
            debugMessage(_T("Font::createGlyph : 不支持的 bitmap.pixel_mode"));
        }

        if (pPixelBuffer)
        {
            int x, y;
            FontTextureNodePtr node = m_texMgr.getTexNode(m_height);
            if(node && node->lockOne(x, y))
            {
                pGLData->texU = x;
                pGLData->texV = y;
                pGLData->pTexture = node->texture();

                DrawGlyphToTexture(*pGLData, pPixelBuffer);
            }

            if (bShouldDeletePixelBuffer)
            {
                delete [] pPixelBuffer;
            }
        }
        pGLData->valid = true;
        return pGLData;
    }

    bool Font::drawWord(float x, float y, wchar_t ch, DWORD cr, float & ax, float & ay)
    {
        WordInfo word;
        if (!getWord(ch, word))
        {
            ay = ax = (float)m_height;
            return false;
        }
        LPDIRECT3DDEVICE9 pDevice = FontMgr::instance()->device();

        x += word.dx;
        y += word.dy;
        WordVertex vertices[] = {
            { x, y, 1.0f, cr, word.x1, word.y1},
            { x+word.dw, y,1.0f, cr, word.x2, word.y1}, 
            { x, y+word.dh, 1.0f, cr, word.x1, word.y2},
            { x+word.dw, y+word.dh, 1.0f, cr, word.x2, word.y2}
        };

        pDevice->SetTexture(0, word.pTexture);
        pDevice->SetFVF( WordVertex::FVF );
        pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertices, WordVertex::SIZE );

        ax = word.ax;
        ay = word.ay;

        return true;
    }

    void Font::parseTextToLines(
        std::vector<std::wstring> & textLines, 
        std::vector<float> & widthLines,
        const std::wstring & text,
        const CRect & rc,
        DWORD style)
    {
        if (text.empty())
        {
            throw(std::invalid_argument("text must not be null."));
        }

        size_t n = text.size();
        if (style & TextAlign::mutiLine)//多行
        {
            size_t last = 0;
            float width = 0.0f;
            for (size_t i = 0; i!=n; ++i)
            {
                wchar_t ch = text[i];
                if (ch == L'\r')
                {
                    continue;
                }
                else if (ch == L'\n')//换行
                {
                    textLines.push_back(text.substr(last, i+1-last));
                    widthLines.push_back(width);

                    width = 0.0f;
                    last = i + 1;
                }
                else
                {
                    CharGlyphData* pData = getGlyph(ch);
                    if (!pData || !pData->valid)//无效字符
                    {
                        width += m_height;
                    }
                    else
                    {
                        width += pData->advanceX;
                    }

                    if (width >= rc.width())//该行已满
                    {
                        textLines.push_back(text.substr(last, i-last+1));
                        widthLines.push_back(width);
                        width = 0.0f;

                        //跳过紧跟其后的一个换行
                        if (i+1 < n && text[i+1]==L'\n')
                        {
                            ++i;
                        }
                        else if (i+2 < n && text[i+1]==L'\r' && text[i+2]==L'\n')
                        {
                            i += 2;
                        }
                        last = i+1;
                    }
                }
            }
            textLines.push_back(text.substr(last, n-last));
            widthLines.push_back(width);
        }
        else
        {
            int width, height;
            getTextSize(width, height, text);

            textLines.push_back(text);
            widthLines.push_back(float(width));
        }

    }

    bool Font::drawText(const std::wstring & text, const CRect & rc, DWORD style, DWORD cr)
    {
        if (text.empty())
        {
            return false;
        }

        std::vector<std::wstring> textLines;    //每行的文本
        std::vector<float>  widthLines;         //每行文字的宽度
        parseTextToLines(textLines, widthLines, text, rc, style);
        
        float lineHeight = float(m_height);//每行的行高
        float top = float(rc.top);
        if (style & TextAlign::vcenter)
        {
            top += (rc.height() - textLines.size()*lineHeight)*0.5f;
        }
        else if (style & TextAlign::bottom)
        {
            top += rc.height() - textLines.size()*lineHeight;
        }
        for (size_t i=0; i<textLines.size(); ++i)
        {
            float width = widthLines[i];
            float x = float(rc.left);
            float y = top + lineHeight * i;

            if (style & TextAlign::center)//水平居中
            {
                x += (rc.width() - width) * 0.5f;
            }
            else if (style & TextAlign::right)//右对齐
            {
                x = rc.right - width;
            }
            drawText(x, y, textLines[i], cr);
        }

        return true;
    }

    bool Font::drawText(float x, float y, const std::wstring & text, DWORD cr)
    {
        LPDIRECT3DDEVICE9 pDevice = FontMgr::instance()->device();

        DWORD rFlag[] = {
            D3DRS_ZENABLE, 
            D3DRS_LIGHTING, 
            D3DRS_ALPHABLENDENABLE, 
            D3DRS_SRCBLEND, 
            D3DRS_DESTBLEND,
            D3DTSS_COLOROP,
            D3DTSS_COLORARG1,
            D3DTSS_ALPHAOP,
            D3DTSS_ALPHAARG1,
            D3DTSS_ALPHAARG2,
            D3DSAMP_MAGFILTER,
            D3DSAMP_MINFILTER
        };
        DWORD rValue[] = {
            FALSE,
            FALSE,
            TRUE, 
            D3DBLEND_SRCALPHA,
            D3DBLEND_INVSRCALPHA,
            D3DTOP_SELECTARG1,
            D3DTA_DIFFUSE,
            D3DTOP_MODULATE,
            D3DTA_TEXTURE,
            D3DTA_DIFFUSE,
            D3DTEXF_NONE,
            D3DTEXF_NONE
        };

        const int n = 12;
        DWORD oldRValue[n];
        const int i0 = 0;
        const int i1 = 5;
        const int i2 = 10;
        const int i3 = 12;

        for (int i=i0; i<i1; ++i)
        {
            pDevice->GetRenderState((D3DRENDERSTATETYPE)rFlag[i], &oldRValue[i]);
            pDevice->SetRenderState((D3DRENDERSTATETYPE)rFlag[i], rValue[i]);
        }
        for (int i=i1; i<i2; ++i)
        {
            pDevice->GetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE)rFlag[i], &oldRValue[i]);
            pDevice->SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE)rFlag[i], rValue[i]);
        }
        for (int i=i2; i<i3; ++i)
        {
            pDevice->GetSamplerState(0, (D3DSAMPLERSTATETYPE)rFlag[i], &oldRValue[i]);
            pDevice->SetSamplerState(0, (D3DSAMPLERSTATETYPE)rFlag[i], rValue[i]);
        }

        float ax = 0, ay = float(m_height);
        for (std::wstring::const_iterator it = text.begin(); it != text.end(); ++it)
        {
            wchar_t ch = *it;
            if (ch == L'\r')
            {
                continue;
            }
            else if (ch == L'\n')
            {
                ch = L' ';
            }

            drawWord(x, y, *it, cr, ax, ay);
            x += ax;
        }

        for (int i=i0; i<i1; ++i)
            pDevice->SetRenderState((D3DRENDERSTATETYPE)rFlag[i], oldRValue[i]);
        for (int i=i1; i<i2; ++i)
            pDevice->SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE)rFlag[i], oldRValue[i]);
        for (int i=i2; i<i3; ++i)
            pDevice->SetSamplerState(0, (D3DSAMPLERSTATETYPE)rFlag[i], oldRValue[i]);
        return true;
    }

    bool Font::getTextSize(int & width, int & height, const std::wstring & text)
    {
        width = 0;
        height = m_height;
        for (std::wstring::const_iterator it = text.begin(); it != text.end(); ++it)
        {
            wchar_t ch = *it;
            if (ch == L'\r')
            {
                continue;
            }
            else if (ch == '\n')
            {
                ch = L' ';
            }

            CharGlyphData * pData = getGlyph(ch);
            if (pData)
            {
                width += pData->advanceX;
                height = max(pData->advanceY, height);
            }
            else 
            {
                width += m_height;
            }
        }
        height = m_height;

        return true;
    }
    bool Font::getCursorPos(const std::wstring & text, const CRect & rc, DWORD style, 
        DWORD cursor, float & x, float & y, float & h)
    {
        h = float(m_height);
        x = float(rc.left);
        y = float(rc.top);
        if (text.empty() || cursor == 0)
        {
            return false;
        }

        std::vector<std::wstring> textLines;    //每行的文本
        std::vector<float>  widthLines;         //每行文字的宽度


        parseTextToLines(textLines, widthLines, text.substr(0, cursor), rc, style);

        size_t i = textLines.size() - 1;
        x += widthLines[i];
        y += m_height * i;

        return true;
    }

    bool Font::getCursorByPos(const std::wstring & text, const CRect & rc, DWORD style, 
        DWORD & cursor, float x, float y)
    {
        if (text.empty() || !rc.isIn((int)x, (int)y))
        {
            return false;
        }

        std::vector<std::wstring> textLines;    //每行的文本
        std::vector<float>  widthLines;         //每行文字的宽度
        parseTextToLines(textLines, widthLines, text, rc, style);
        int n = textLines.size();

        y -= rc.top;
        int row = int(y / m_height);
        if (row >= n)
        {
            return false;
        }

        //将前n行的文字个数累加
        cursor = 0;
        for (int i=0; i<row; ++i)
        {
            cursor += textLines[i].size();
        }

        //坐标落在了末尾
        std::wstring temp = textLines[row];
        size_t tn = temp.size();
        if (x >= widthLines[row])
        {
            cursor += tn;
            if(tn > 0 && temp[tn-1]==L'\n')//如果光标落在行末，应该位于回车符之前
            {
                --cursor;
            }
            return true;
        }

        //在中间找到一个合理的位置
        x -= rc.left;
        float width = 0.0f;
        for (size_t i = 0; i<tn; ++i)
        {
            wchar_t ch = temp[i];
            if (ch == L'\r')
            {
                continue;
            }
            else if (ch == '\n')
            {
                ch = L' ';
            }

            CharGlyphData * pData = getGlyph(ch);
            if (pData)
            {
                width += pData->advanceX;
            }
            else 
            {
                width += m_height;
            }

            ++cursor;
            if (width >= x)
            {
                break;
            }
        }
        return true;
    }

    bool Font::fillWordToTexture(LPDIRECT3DTEXTURE9 pDestTex, 
        int dx, int dy, wchar_t ch, D3DXCOLOR color,
        int & ax, int & ay)
    {
        CharGlyphData *pGlyph = getGlyph(ch);
        if (!pGlyph || !pGlyph->valid)
        {
            ax = ay = m_height;
            return false;
        }

        ax = pGlyph->advanceX;
        ay = pGlyph->advanceY;

        LPDIRECT3DTEXTURE9 pSrcTex = pGlyph->pTexture;
        if (!pSrcTex)
        {
            return false;
        }

        D3DLOCKED_RECT destLockRC;

        {
            RECT destRC;
            destRC.left = dx + pGlyph->leftMargin;
            destRC.top = dy + pGlyph->topMargin;
            destRC.right = destRC.left + pGlyph->bitmapWidth;
            destRC.bottom = destRC.top + pGlyph->bitmapHeight;

            HRESULT hr = pDestTex->LockRect(0, &destLockRC, &destRC, 0);
            if(FAILED(hr))
            {
                debugMessage(_T("ERROR: fillWordToTexture: lock dest texture faild:%d"), HRESULT_CODE(hr));
                D3DERR_DEVICELOST;
                return false;
            }
        }

        D3DLOCKED_RECT srcLockRC;

        {
            RECT srcRC;
            srcRC.left = pGlyph->texU;
            srcRC.top = pGlyph->texV;
            srcRC.right = srcRC.left + pGlyph->bitmapWidth;
            srcRC.bottom = srcRC.top + pGlyph->bitmapHeight;

            if(FAILED(pSrcTex->LockRect(0, &srcLockRC, &srcRC, 0)))
            {
                pDestTex->UnlockRect(0);
                return false;
            }
        }

        int nWidth = pGlyph->bitmapWidth;
        int nHeight = pGlyph->bitmapHeight;

        unsigned char r = (unsigned char)(255 * color.r);
        unsigned char g = (unsigned char)(255 * color.g);
        unsigned char b = (unsigned char)(255 * color.b);

        for (int y = 0; y < nHeight; ++y)
        {
            for (int x = 0; x < nWidth; ++x)
            {
                unsigned char* src_pixel = ((unsigned char*)srcLockRC.pBits) + srcLockRC.Pitch * y + x;
                unsigned char* dest_pixel = ((unsigned char*)destLockRC.pBits) + destLockRC.Pitch * y + x * 4;
                 
                dest_pixel[0] = b;
                dest_pixel[1] = g;
                dest_pixel[2] = r;
                dest_pixel[3] = BYTE(color.a * (*src_pixel));
            }
        }

        pSrcTex->UnlockRect(0);
        pDestTex->UnlockRect(0);

        return true;
    }
    
    bool Font::fillTextToTexture(LPDIRECT3DTEXTURE9 pDestTex, 
        int dx, int dy, const std::wstring & text, DWORD color)
    {
        int ax = 0, ay = 0;
        for (std::wstring::const_iterator it = text.begin(); it != text.end(); ++it)
        {
            fillWordToTexture(pDestTex, dx, dy, *it, color, ax, ay);
            dx += ax;
        }

        return true;
    }


    //////////////////////////////////////////////////////////////////////////
    static FT_Library g_ft_lib;

    /*static*/ FontMgr *FontMgr::s_pMgr = NULL;

    /*static*/ FontMgr * FontMgr::instance()
    {
        if (!s_pMgr)
        {
            s_pMgr = new FontMgr();
        }
        return s_pMgr;
    }

    FontMgr::FontMgr()
        : m_pd3dDevice(NULL)
    {
    }
    FontMgr::~FontMgr()
    {

    }

    bool FontMgr::init(LPDIRECT3DDEVICE9 pDevice)
    {
        assert(m_pd3dDevice==NULL && "FontMgr has been initialized!");

        m_pd3dDevice = pDevice;
        m_pd3dDevice->AddRef();

        FT_Init_FreeType(&g_ft_lib);

        return true;
    }

    void FontMgr::fini()
    {
        if(m_pd3dDevice)
        {
            FT_Done_FreeType(g_ft_lib);
            m_pd3dDevice->Release();
            m_pd3dDevice = NULL;
        }

        if (s_pMgr)
        {
            delete s_pMgr;
            s_pMgr = NULL;
        }
    }

    bool FontMgr::registerFontFile(const tstring & fontFile, const tstring & key)
    {
        FaceMap::iterator it = m_faceMap.find(key);

        if (it != m_faceMap.end())
            return true;

        void* pFace = createFace(fontFile);
        m_faceMap[key] = pFace;
        return pFace != NULL;
    }

    void* FontMgr::getFace(const tstring & faceName)
    {
        FaceMap::iterator it = m_faceMap.find(faceName);
        if (it != m_faceMap.end())
            return it->second;

        throw(std::runtime_error("the font face doesn't registered!please register it first."));
    }

    FontPtr FontMgr::getFont(const tstring & fontName)
    {
        FontMap::iterator it = m_fontMap.find(fontName);
        if (it != m_fontMap.end())
        {
            return it->second;
        }

        FontInfo info(fontName);
        if (!info.valid())
        {
            return NULL;
        }

        void* pFace = getFace(info.faceName);
        if(!pFace)
            return NULL;

        FontPtr ptr(new Font(pFace, info.height));
        m_fontMap[fontName] = ptr;

        return ptr;
    }

    void* FontMgr::createFace(const tstring & faceName)
    {
        //FT模块的返回值。为0表示成功。
        FT_Error nFTResult = 0;
        FT_Face pFace = NULL ;

#ifdef _UNICODE
        std::string faceNameA = wcharToChar(faceName);
        nFTResult = FT_New_Face(g_ft_lib, faceNameA.c_str(), 0, &pFace);
#else
        //加载一个字体。
        nFTResult = FT_New_Face(g_ft_lib, faceName.c_str(), 0, &pFace);
#endif
        if (nFTResult)
        {
            debugMessage(_T("加载字库文件%s失败"), faceName.c_str());
            return NULL;
        }

        //FontFace加载完毕后，默认是Unicode charmap。
        //这里再主动设置一下，使用Unicode charmap。
        nFTResult = FT_Select_Charmap(pFace, FT_ENCODING_UNICODE);
        if (nFTResult)
        {
            debugMessage(_T("选择Unicode编码失败"));
            return NULL;
        }

        debugMessage(_T("create face:%s"), faceName.c_str());

        return pFace;
    }

}//namespace Lazy