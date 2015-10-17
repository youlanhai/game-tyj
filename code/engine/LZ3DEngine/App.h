//App.h
#pragma once

#include "Commen.h"

#include "CallBack.h"

#include "Keyboard.h"
#include "RenderObj.h"

#include "Res.h"
#include "RenderTask.h"

#include "SkyBox.h"

#include "Texture.h"
#include "TextTexture.h"

namespace LoadingType
{
    const int None = 0;
    const int Initialize = 1;
    const int SpaceChange = 2;
}


/**应用程序类*/
class LZDLL_API CApp : public IBase
{
public:
    CApp(void);

    virtual ~CApp(void);

    /** 游戏类初始化
    * hInstance: 应用程序实例对象
    * szCaption: 窗口标题
    * nWidth: 窗口宽度
    * nHeight: 窗口高度
    * bFullScreen: 是否全屏*/
    bool create(HINSTANCE hInstance,
        LPCSTR szCaption,
        int nWidth,
        int nHeight,
        bool bFullScreen);

    /**消息循环*/
    virtual void mainLoop();

    /**窗口过程。不需要调用默认窗口处理过程，如果未处理消息请返回0，否则请返回非0。*/
    virtual LRESULT wndProc(HWND ,UINT ,WPARAM ,LPARAM );

    /** 退出游戏*/
    void quitGame(void);

    ///处理消息。返回false，表示程序退出。
    bool processMessage();

protected:

    /**游戏初始化*/
    virtual bool init(void) ;

    /**运行*/
    virtual void run(void) ;

    /**更新*/
    virtual void update(void);

    /**渲染*/
    virtual void render(void) ;

    /**释放资源*/
    virtual void clear(void) ;

    /**注册窗口类消息。请实现该消息来修改窗口风格。*/
    virtual void onRegisterClass(WNDCLASSEX *pwc) ;

    /**创建设备消息。实现该方法来修改设备。*/
    virtual void onCreateDevice(D3DPRESENT_PARAMETERS * pParam) ;

    /** 释放单例资源。*/
    virtual void releaseSingleton();
private:

    /**注册窗口类*/
    void registerClass();

    /**创建窗口*/
    bool createWindow();

    /**初始化渲染设备*/
    bool createDevice();
public://属性

    ///获得D3D对象
    IDirect3D9* getD3d(void){ return m_pD3d; }

    ///获得绘图设备
    IDirect3DDevice9* getDevice(void){ return m_pd3dDevice; }

    ///获得窗口宽度
    int getWidth(void){ return m_nWidth; }

    ///获得窗口高度
    int getHeight(void){ return m_nHeight; }

    ///是否是全屏
    bool isFullScr(void){ return m_bFullScreen; }

    ///获得窗口句柄
    HWND getHWnd(void){ return m_hWnd; }

    ///获得窗口实例句柄
    HINSTANCE getInstance(void){ return m_hInstance; }

    ///设置窗口标题
    void setCaption(std::string caption);

    ///获得窗口标题
    std::string getCaption(void);

    float elapse(){ return m_fElapse; }

    float getTimeScale() const { return m_timeScale; }
    void setTimeScale(float scale){ m_timeScale = scale; }

public:
    CRenderTask* getRenderTaskMgr(void){ return m_pRenderTaskMgr.get(); }

    CRenderTask* getUpdateTaskMgr(void){ return m_pUpdateTaskMgr.get(); }

    CRenderTask* getBillboardMgr(void){ return m_pBillboardMgr.get(); }

    cResMgr* getResMgr(void){ return m_pResMgr.get();}

    CKeyboard* getKeyboard(void){ return m_pKeyboard.get(); }

    cCallBackPool* getCallBackPool(void){ return m_pCallBackPool.get(); }

    cSkyBox* getSkyBox(void){ return m_pSkyBox.get(); }

    void addRender(IRenderObj* rend);

    void addUpdater(IRenderObj* up);

    void addUpdateRender(IRenderObj *pObj);

    void addBillboard(IRenderObj* pObj);

    void removeRender(IRenderObj* rend);

    void removeUpdater(IRenderObj* up);

    void removeUpdateRender(IRenderObj* pObj);

    void removeBillboard(IRenderObj* pObj);

    bool isMsgHooked(void){ return m_bMsgHooked; }
public:
    void screenToClient(POINT* pt);

    void clientToScreen(POINT* pt);

    CPoint getCursorPos(void);

    void setCursorPos(CPoint pt);

    CRect getClientRect(void);

public:

    //多线程Loading

    bool isGameLoading() const { return m_isLoading; }
    virtual void startGameLoading(int type)
    {
        m_isLoading = true;
        m_loadingType = type;
        m_loadingElapse = 0.0f;
    }
    virtual void stopGameLoading() { m_isLoading = false; }

    bool isGameLoadingOK() const { return m_isLoadingOK; }
    bool setGameLoadingStatus(bool ok){ m_isLoadingOK = ok; }

protected:
    IDirect3D9          *m_pD3d;
    IDirect3DDevice9    *m_pd3dDevice;      //Direct3D设备对象
    HINSTANCE			m_hInstance;		//应用程序实例对象
    HWND				m_hWnd;				//窗口句柄
    int		            m_nWidth;			//窗口宽度
    int		            m_nHeight;			//窗口高度
    std::string	        m_caption;	        //窗口标题
    bool	            m_bFullScreen;		//是否全屏
    float               m_fElapse;
    float               m_timeScale;

    bool    m_isLoading;
    bool    m_isLoadingOK;
    int     m_loadingType;
    float   m_loadingElapse;

protected:
    //公共资源
    RefPtr<cResMgr>         m_pResMgr;
    RenderTaskPtr           m_pRenderTaskMgr;   //< 渲染队列
    RenderTaskPtr           m_pUpdateTaskMgr;   //< 更新队列
    RenderTaskPtr           m_pBillboardMgr;
    RefPtr<CKeyboard>	    m_pKeyboard;
    RefPtr<cCallBackPool>   m_pCallBackPool;
    bool                    m_bMsgHooked;
    RefPtr<cTextTextureFactory> m_pTextTextureFactory;
    RefPtr<cSkyBox>         m_pSkyBox;
};

