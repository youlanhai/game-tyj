//可绘制资源

#pragma once

#include "RenderObj.h"

#include <map>

/** 可绘制资源接口*/
class IRenderRes : public IBase, public IRenderObj
{
public:
    /** 创建自己。*/
    static IRenderRes* createObj(void){ return NULL; }

    /** 加载资源。*/
    virtual bool load(std::string filename) = 0;

    /** 获得资源类型*/
    virtual LPCSTR getType(void) = 0;

    virtual ~IRenderRes(){}
protected:
    ///构造函数
    IRenderRes(){}
};

/** 资源工厂*/
class IResFactory : public IBase
{
public:
    /** 创建资源对象。*/
    virtual IRenderRes* createObj(std::string type) = 0;
};

/** 资源管理器*/
class LZDLL_API cRenderResMgr : public IBase
{
public:
    typedef std::map<std::string, IRenderRes*>      ResContainer;
    typedef std::pair<std::string, IRenderRes*>     ResPair;
    typedef ResContainer::iterator                  ResIterator;
public:

    ///构造函数
    cRenderResMgr(IDirect3DDevice9 *pDevice);

    ///析构函数
    ~cRenderResMgr(void);

    /** 初始化。可以传入配置文件，预先加载资源。
        @param configFile : 配置文件。
        配置文件类型：
        count[int,资源数目]
        name[string, 资源名称], type[string，资源类型]
        */
    bool init(std::string configFile="");

    /** 按名称获得资源。如果现存列表中没有，则会从文件中加载，如果没有找到活加载失败返回NULL。
        @param fileName : 资源文件名。
        @param type : 资源类型。
        */
    IRenderRes* get(std::string fileName, std::string type);

    /** 移除指定资源。
        @param fileName : 资源文件名。*/
    void remove(std::string fileName);

    /** 清空资源。*/
    void clear(void);

    /** 加载资源。
        @param fileName：资源文件名。
        @param type : 资源类型。
        */
    IRenderRes* load(std::string fileName, std::string type);

    /** 设置工厂方法。*/
    void setFactory(IResFactory* factory){ m_factory = factory; }

    /** 获得工厂方法。*/
    IResFactory* getFactory(void){ return m_factory.get(); }

    IDirect3DDevice9* getDevice(void){ return m_pd3dDevice; }
private:
    IDirect3DDevice9*   m_pd3dDevice;
    ResContainer        m_resourses;    //< 资源容器
    RefPtr<IResFactory> m_factory;      //< 工厂对象
};