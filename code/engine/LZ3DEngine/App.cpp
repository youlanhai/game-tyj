//D3DApp.cpp

#include "App.h"
#include "Device.h"
#include "../Font/Font.h"
#include "TerrinMap.h"
#include "Sound.h"
#include "RenderState.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")


#define CLASS_NAME _T("LZ3DEnginex")


CApp * g_pInnerApp_ = NULL;

/** 获得游戏类指针。*/
LZDLL_API CApp*  getApp(void)
{
    return g_pInnerApp_ ;
}

//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK app_defWndProc_(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0 ;
    }
    if (g_pInnerApp_ != NULL)
    {
        if(g_pInnerApp_->wndProc(hWnd,msg,wParam,lParam))
        {
            return 0;
        }
    }
    return DefWindowProc(hWnd,msg,wParam,lParam);
}

///////////////////class CApp start/////////////////////////
CApp::CApp()
    : m_isLoading(false)
    , m_isLoadingOK(false)
    , m_loadingElapse(0.0f)
    , m_loadingType(LoadingType::None)
    , m_timeScale(1.0f)
{
    Lazy::MemoryPool::init();

    g_pInnerApp_ = this;

    m_hWnd=NULL;
    m_hInstance=NULL;
    m_bFullScreen=false;
    m_nWidth=0;
    m_nHeight=0;
    m_pD3d = NULL;
    m_pd3dDevice = NULL;
    m_bMsgHooked = false;
    m_fElapse = 0.0f;
}
CApp::~CApp()
{
    releaseSingleton();

    g_pInnerApp_ = NULL;
    SafeRelease(m_pd3dDevice);
	SafeRelease(m_pD3d);
	UnregisterClass(CLASS_NAME, m_hInstance);
}

void CApp::releaseSingleton()
{
    cTerrainRes::instance()->release();
    cSoundMgr::instance()->release();
    //////////////////////////////////////////////////////////////////////////
    Lazy::MemoryPool::fini();
    Lazy::MemoryCheck::instance()->release();
}

/** 游戏类初始化
* hInstance: 应用程序实例对象
* caption: 窗口标题
* width: 窗口宽度
* height: 窗口高度
* bFullScreen: 是否全屏
*/
bool CApp::create(HINSTANCE hInstance,
                LPCSTR szCaption,
                int nWidth,
                int nHeight,
                bool bFullScr)
{
    m_hInstance=hInstance;
    m_nWidth = nWidth;
    m_nHeight = nHeight;
    m_bFullScreen=bFullScr;

    if(szCaption != NULL)
    {
        m_caption = szCaption;
    }

    WRITE_LOG(_T("register window class..."));
    registerClass();//注册窗口类

    WRITE_LOG(_T("create window..."));
    if(!createWindow())//创建窗口
    {
        WRITE_LOG(_T("ERROR: create window failed!"));
        return false;
    }

    WRITE_LOG(_T("create d3d device..."));
    if(!createDevice())//创建设备
    {
        WRITE_LOG(_T("ERROR: create d3d device failed!"));
        return false;
    }

    ShowWindow(m_hWnd,SW_SHOW);
    UpdateWindow(m_hWnd);

    WRITE_LOG(_T("initialize game..."));
    if(!init())//初始化游戏
    {
        WRITE_LOG(_T("ERROR: initialize failed!"));
        return false;
    }


    return true;
}


void CApp::mainLoop(void)
{
    WRITE_LOG(_T("start game loop."));
    static MSG msg = {0};
    while(msg.message != WM_QUIT)
    {
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            if(GetActiveWindow() != getHWnd())
            {
                Sleep(100);
            }
            run();
        }
    }
    WRITE_LOG(_T("game loop end."));

    clear();//释放资源
}

///处理消息。返回false，表示程序退出。
bool CApp::processMessage()
{
    static MSG msg = {0};
    while(msg.message != WM_QUIT)
    {
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else break;
    }

    return msg.message != WM_QUIT;
}

/**运行*/
void CApp::run(void) 
{
    DWORD curTime = timeGetTime();
    static DWORD oldUpdateTime = curTime;
    m_fElapse = (curTime - oldUpdateTime) * 0.001f;
    m_fElapse *= m_timeScale;
    if (m_fElapse < 0.00001f)
    {
        return;
    }
    //m_fElapse *= 0.5f;
    oldUpdateTime = curTime;

    update();//更新逻辑
    render();//渲染画面

    //Sleep(100);
}

/** 退出游戏*/
void CApp::quitGame(void)
{
    PostQuitMessage(0);
}


//初始化Direct3D对象及设备对象
bool CApp::createDevice()
{
    m_pD3d = Direct3DCreate9(D3D_SDK_VERSION);
    if(NULL == m_pD3d)
    {
        return false;
    }
    D3DCAPS9 caps;
    if(FAILED(m_pD3d->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps)))
    {
        m_pD3d->Release();
        return false;
    }
    int vp = 0;
    if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
    else
    {
        vp = D3DCREATE_MIXED_VERTEXPROCESSING;
    
    }

    vp |= D3DCREATE_MULTITHREADED;

    //获得显卡当前显示模式
    D3DDISPLAYMODE d3dmm;
    m_pD3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3dmm);
    //设置显示参数
    D3DPRESENT_PARAMETERS d3dpp;
    memset(&d3dpp,0,sizeof(d3dpp));
    d3dpp.Windowed = !m_bFullScreen;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3dmm.Format;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    
    if(m_bFullScreen)
    {//全屏显示
        m_nWidth = d3dmm.Width;
        m_nHeight = d3dmm.Height;
        d3dpp.BackBufferHeight = d3dmm.Height;
        d3dpp.BackBufferWidth = d3dmm.Width;
        d3dpp.FullScreen_RefreshRateInHz = d3dmm.RefreshRate;
        d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    }

    //d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    onCreateDevice(&d3dpp);

    //创建Direct3D设备对象
    if(FAILED(m_pD3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        m_hWnd,
        vp,//D3DCREATE_MIXED_VERTEXPROCESSING
        &d3dpp,
        &m_pd3dDevice)))
    {
        return false;
    }

    //设置默认的渲染状态。如果程序修改了某个值，用完之后需要改过来。

    //设置部分环境光
    m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, DefaultRS::LightingEnable);
    m_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, DefaultRS::SpecularEnable);
    m_pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(100,100,100));

    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, DefaultRS::ZEnable);
    m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, DefaultRS::CullMode);

    m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, DefaultRS::AlphaTestEnable);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, DefaultRS::AlphaRef);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, DefaultRS::AlphaFunc);

    m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, DefaultRS::AlphaBlendEnable);
    m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, DefaultRS::SrcBlend);
    m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, DefaultRS::DstBlend);

    m_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, DefaultRS::ShadeMode);


    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, DefaultRS::TextureMinFilter);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, DefaultRS::TextureMagFilter);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, DefaultRS::TextureAdressU);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, DefaultRS::TextureAdressV);

    m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    
    return true;
}


//创建窗口
bool CApp::createWindow()
{
    DWORD dwStyle ;
    int x, y;
    int width, height;
    if (m_bFullScreen)
    {
        dwStyle = WS_VISIBLE | WS_POPUP;
        x = 0;
        y = 0;
        width = m_nWidth = GetSystemMetrics(SM_CXSCREEN);
        height = m_nHeight = GetSystemMetrics(SM_CYSCREEN);
    }
    else
    {
        width = m_nWidth + ::GetSystemMetrics(SM_CXBORDER)*2;
        height = m_nHeight + ::GetSystemMetrics(SM_CYBORDER) + ::GetSystemMetrics(SM_CYCAPTION);
        
        x = (GetSystemMetrics(SM_CXSCREEN)-width)/2;
        y = (GetSystemMetrics(SM_CYSCREEN)-height - 30)/2;
        dwStyle = WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;
    }
    m_hWnd = CreateWindowW(
        CLASS_NAME,
        Lazy::charToWChar(m_caption).c_str(),
        dwStyle,
        x,
        y,
        width,
        height,
        NULL,
        NULL,
        m_hInstance,
        NULL);
    return (m_hWnd != NULL);
}

//注册窗口类
void CApp::registerClass()
{
    WNDCLASSEX wc;
    memset(&wc,0,sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = (WNDPROC)app_defWndProc_;
    wc.hInstance = m_hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL,IDC_ARROW);
    wc.style = CS_CLASSDC;

    onRegisterClass(&wc);

    RegisterClassEx(&wc);
}


/**游戏初始化*/
bool CApp::init(void)
{
    getDevStack()->setDevice(m_pd3dDevice);

    Lazy::FontMgr::instance()->init(m_pd3dDevice);
    cMap::instance()->init(m_pd3dDevice);

    m_pResMgr = new cResMgr(m_pd3dDevice);
    m_pResMgr->setFactory(new cResFactory());

    m_pRenderTaskMgr = new CRenderTask();
    m_pUpdateTaskMgr = new CRenderTask();
    m_pBillboardMgr = new CRenderTask();

    m_pKeyboard=new CKeyboard();
    addUpdater(m_pKeyboard.get());

    m_pCallBackPool = new cCallBackPool();

    m_pTextTextureFactory = new cTextTextureFactory(m_pd3dDevice);

    m_pSkyBox = new cSkyBox(m_pd3dDevice);
    //addRender(m_pSkyBox.get());
	return true;
}

/**游戏更新*/
void CApp::update(void)
{
    
}

/**渲染*/
void CApp::render(void)
{
	
}

/**释放资源*/
void CApp::clear(void)
{
    m_pSkyBox = NULL;
    m_pTextTextureFactory = NULL;
    m_pCallBackPool = NULL;
    m_pRenderTaskMgr = NULL;
    m_pUpdateTaskMgr = NULL;
    m_pBillboardMgr = NULL;
    m_pResMgr = NULL;
    m_pKeyboard = NULL;

    cMap::instance()->fini();
    Lazy::FontMgr::instance()->fini();
}

/**注册窗口类消息。请实现该消息来修改窗口风格。*/
void CApp::onRegisterClass(WNDCLASSEX *pwc)
{

}

/**创建设备消息。实现该方法来修改设备。*/
void CApp::onCreateDevice(D3DPRESENT_PARAMETERS * pParam)
{

}

/**窗口过程。如果未处理消息请返回0，否则请返回非0。*/
LRESULT  CApp::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch(msg)
    {
    case WM_KEYUP:
        switch(wParam)
        {
        case VK_ESCAPE:
            DestroyWindow(hWnd);
            break;
        }
        break;
    default:
        return 0;
    }
    return 1;
}


void CApp::setCaption(std::string caption)
{
    m_caption = caption;
    if (m_hWnd != NULL)
    {
        SetWindowText(m_hWnd, Lazy::charToWChar(m_caption).c_str());
    }
}

std::string CApp::getCaption(void)
{
    return m_caption;
}

void CApp::screenToClient(POINT* pt)
{
    ScreenToClient(m_hWnd, pt);
}

void CApp::clientToScreen(POINT* pt)
{
    ClientToScreen(m_hWnd, pt);
}

CPoint CApp::getCursorPos(void)
{
    CPoint pt;
    GetCursorPos(&pt);
    screenToClient(&pt);
    return pt;
}

void CApp::setCursorPos(CPoint pt)
{
    clientToScreen(&pt);
    SetCursorPos(pt.x, pt.y);
}

CRect CApp::getClientRect(void)
{
    CRect rc;
    GetClientRect(m_hWnd, &rc);
    return rc;
}

void CApp::addRender(IRenderObj* rend)
{
    m_pRenderTaskMgr->add(rend);
}
void CApp::addUpdater(IRenderObj* up)
{
    m_pUpdateTaskMgr->add(up);
}

void CApp::addUpdateRender(IRenderObj *pObj)
{
    addUpdater(pObj);
    addRender(pObj);
}

void CApp::addBillboard(IRenderObj* pObj)
{
    m_pBillboardMgr->add(pObj);
}

void CApp::removeRender(IRenderObj* rend)
{
    m_pRenderTaskMgr->remove(rend);
}

void CApp::removeUpdater(IRenderObj* up)
{
    m_pUpdateTaskMgr->remove(up);
}

void CApp::removeUpdateRender(IRenderObj* pObj)
{
    removeUpdater(pObj);
    removeRender(pObj);
}

void CApp::removeBillboard(IRenderObj* pObj)
{
    m_pBillboardMgr->remove(pObj);
}
///////////////////class CApp end/////////////////////////
