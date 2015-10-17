
#include "stdafx.h"
#include "Header.h"
#include "..\LZ3DEngine\LZ3DEngine.h"
#include "..\Physics\LazyConllision.h"
#include "WrapperMisc.h"



class TaskWithPyFun : public Lazy::TaskInterface
{
public:
    TaskWithPyFun(object pWork, object pFinish, int workID)
        : m_pWork(pWork), m_pFinish(pFinish), m_workID(workID)
    {
    }

    virtual void doTask(void)
    {
        if(!m_pWork.is_none())
        {
            m_pWork();
        }
    }
    virtual void onTaskFinish(void)
    {
        if(!m_pFinish.is_none())
        {
            m_pFinish();
        }
    }

    virtual bool isReady(void)
    {
        if(Lazy::LoadingMgr::instance()->getWorkStep() > m_workID)
        {
            debugMessage(_T("--------loading task is ready.."));
            return true;
        }
        else
        {
            debugMessage(_T("--------loading task not ready.."));
            return false;
        }
    }

private:
    object  m_pWork;
    object  m_pFinish;
    int     m_workID;
};

void addLoadingTask(object work, object finish)
{
    bool immediate = false;

    int workID = 0;
    if (!immediate)
    {
        //延迟一帧再执行。防止后台线程立即执行时，造成资源访问冲突。
        workID = Lazy::LoadingMgr::instance()->getWorkStep();
    }
    Lazy::LoadingMgr::instance()->addTask(new TaskWithPyFun(work, finish, workID));
}

void testPyObjectArg(object work, object finish)
{
    int a = extract<int>(work);
    int b = extract<int>(finish);

    debugMessage(_T("----test addLoadingTask2: %d %d"), a, b);
}


//////////////////////////////////////////////////////////////////////////

struct WrapperIRes : IRenderRes, wrapper<IRenderRes> 
{
    virtual void update(float fElapse)
    {
        get_override("update")(fElapse);
    }

    virtual void render(IDirect3DDevice9 * pDevice)
    {
        get_override("render")(pDevice);
    }

    /** 加载资源。*/
    virtual bool load(std::string filename)
    {
        return get_override("load")(filename);
    }

    /** 获得资源类型*/
    virtual LPCSTR getType(void)
    {
        return get_override("getType")();
    }
};

///得到模型。资源必须由外部调用delete释放。
ModelPtr loadModel(LPCSTR name, LPCSTR type)
{
    return cModelFactory::createObj(name, type);
    
#if 0
    if (pModel)
    {
        pModel->setManaged(false);
    }
    return pModel;
#endif
}

SoundPtr loadSound(LPCSTR szName)
{
    return cSoundMgr::instance()->loadSound(szName);
}

SoundPtr playSound(LPCSTR szName)
{
    SoundPtr pSound = loadSound(szName);
    if (pSound)
    {
        pSound->play();
    }
    return pSound;
}

cTerrainRes* getTerrainRes()
{
    return cTerrainRes::instance();
}

void debugMsg(LPCSTR msg)
{
    OutputDebugStringA(msg);
}

void writeLog(const std::string & msg)
{
    WRITE_LOGA(msg);
}

int getWheelDelta(WPARAM wParam)
{
    return GET_WHEEL_DELTA_WPARAM(wParam);
}

D3DXVECTOR3 vector3Nomalize(D3DXVECTOR3 vec)
{
    D3DXVec3Normalize(&vec, &vec);
    return vec;
}

void setAmbientLight(DWORD color)
{
    getApp()->getDevice()->SetRenderState(D3DRS_AMBIENT, color);
}

void setDirectionLight(const D3DXVECTOR3 & dir, float a, float r, float g, float b)
{
    //设置灯光
    LPDIRECT3DDEVICE9 device = getApp()->getDevice();
    CLight light;
    light.SetDirectionLight1(device, dir, D3DXCOLOR(r, g, b, a));
}

const D3DFILLMODE FillMap[] =  {D3DFILL_POINT, D3DFILL_WIREFRAME, D3DFILL_SOLID};
const int MaxFillMode = 3;

int getFillMode()
{
    DWORD mode;
    getApp()->getDevice()->GetRenderState(D3DRS_FILLMODE, &mode);
    for (int i=0; i<MaxFillMode; ++i)
    {
        if(mode == FillMap[i]) return i;
    }
    return  0;
}

void setFillMode(int mode)
{
    if(mode < 0 || mode >= MaxFillMode) return;
    getApp()->getDevice()->SetRenderState(D3DRS_FILLMODE, FillMap[mode]);
}

void wrapperMisc()
{
    def("addLoadingTask", addLoadingTask);
    def("testPyObjectArg", testPyObjectArg);

    def("setAmbientLight", setAmbientLight);
    def("setDirectionLight", setDirectionLight);
    def("getOCTreeCacheBytes", Physics::getOCTreeCacheBytes);
    def("getFillMode", getFillMode);
    def("setFillMode", setFillMode);

    class_<CApp>("CApp", no_init)
        .def_readonly("width", &CApp::getWidth)
        .def_readonly("height", &CApp::getHeight)
        .add_property("caption", &CApp::getCaption,&CApp::setCaption)
        .def("getResMgr", &CApp::getResMgr, return_value_policy<reference_existing_object>())
        .def("getSkyBox", &CApp::getSkyBox, return_value_policy<reference_existing_object>())
        .def("getKeyboard", &CApp::getKeyboard, return_value_policy<reference_existing_object>())
        .def("addUpdater", &CApp::addUpdater)
        .def("quitGame", &CApp::quitGame)
        .def("getCursorPos", &CApp::getCursorPos)
        .def("startGameLoading", &CApp::startGameLoading)
        .def("stopGameLoading", &CApp::stopGameLoading)
        .def("getTimeScale", &CApp::getTimeScale)
        .def("setTimeScale", &CApp::setTimeScale)
        ;

    //常用API
    def("WRITE_LOG", writeLog);

    def("getApp", getApp, return_value_policy<reference_existing_object>());
    def("getResMgr", getResMgr, return_value_policy<reference_existing_object>());
    def("getCamera", getCamera, return_value_policy<reference_existing_object>());
    def("getMap", getMap, return_value_policy<reference_existing_object>());
    def("getPick", getPick, return_value_policy<reference_existing_object>());
    def("debug", debugMsg);
    def("loadSound", loadSound);
    def("playSound", playSound);
    def("getWheelDelta", getWheelDelta);
    def("vector3Distance", D3DXVec3Length);
    def("vector3Nomalize", vector3Nomalize);
    def("isEntitySphereEnable", isEntitySphereEnable);
    def("setEntitySphereEnable", setEntitySphereEnable);
    def("getMaxDistToNpc", getMaxDistToNpc);
    def("setMaxDistToNpc", setMaxDistToNpc);
    def("enableDrawCollision", enableDrawCollision);
    def("isDrawCollisionEnable", isDrawCollisionEnable);


    //常用类
    class_<WrapperIRes, boost::noncopyable>("iRenderRes")
        .def("load", pure_virtual(&IRenderRes::load))
        .def("getType", pure_virtual(&IRenderRes::getType))
        ;

    class_<cResMgr>("cResMgr", no_init)
        .def("init", &cResMgr::init)
        .def("get", &cResMgr::get, return_value_policy<reference_existing_object>())
        .def("remove", &cResMgr::remove)
        ;

    class_<cPick>("cPick", init<>())
        .def("handleMouseEvent", &cPick::handleMouseEvent)
        .def("isIntersectWithTerrain", &cPick::isIntersectWithTerrain)
        .def("getPosOnTerrain", &cPick::getPosOnTerrain, return_value_policy<copy_const_reference>())
        .def("isTerrainObjEnable", &cPick::isTerrainObjEnable)
        .def("setTerrainObjEnable", &cPick::setTerrainObjEnable)
        ;


    class_<cTerrainObj, TerrainObjPtr, bases<I3DObject>, boost::noncopyable>("cTerrainObj", no_init)
        .def("getID", &cTerrainObj::getID)
        .def("getModelID", &cTerrainObj::getModelID)
        .def("setModelID", &cTerrainObj::setModelID)
        .def("getAngle", &cTerrainObj::getAngle, return_value_policy<copy_const_reference>())
        .def("addLookAngle", &cTerrainObj::addLookAngle)
        .def("addUpAngle", &cTerrainObj::addUpAngle)
        .def("addRightAngle", &cTerrainObj::addRightAngle)
        ;

    //公告板

    class_<cBillboard, bases<IBase>>("cBillboard", init<>())
        .def("setSize", &cBillboard::setSize)
        .def("setImage", &cBillboard::setImage)
        .def("show", &cBillboard::show)
        .def("visible", &cBillboard::visible)
        .def("setMaxShowDistance", &cBillboard::setMaxShowDistance)
        .def("getShowMaxDistance", &cBillboard::getShowMaxDistance)
        ;

    class_<cTopBoard, bases<cBillboard>>("cTopBoard", init<I3DObject*>())
        .def("create", &cTopBoard::create)
        .def("setText", &cTopBoard::setText)
        .def("getText", &cTopBoard::getText)
        .def("setColor", &cTopBoard::setColor)
        .add_property("text", &cTopBoard::getText, &cTopBoard::setText)
        .def("setBiasHeight", &cTopBoard::setBiasHeight)
        ;

    //地形
    class_<cMap>("cMap", no_init)
        .def("loadMap", &cMap::loadMap)
        .def("setSource", &cMap::setSource)
        .def("isInBound", &cMap::isInBound)
        .def("getHeight", &cMap::getHeight)
        .def("setShowLevel", &cMap::setShowLevel)
        .def("getShowLevel", &cMap::getShowLevel)
        .def("xMin", &cMap::xMin)
        .def("xMax", &cMap::xMax)
        .def("zMin", &cMap::zMin)
        .def("zMax", &cMap::zMax)
        .def("width", &cMap::width)
        .def("height", &cMap::height)
        .def("loadObj", &cMap::loadObj)
        .def("saveObj", &cMap::saveObj)
        .def("handeMouseEvent", &cMap::handeMouseEvent)
        .def("getActiveObj", &cMap::getActiveObj)//, return_value_policy<reference_existing_object>())
        .def("getSelectObj", &cMap::getSelectObj)//, return_value_policy<reference_existing_object>())
        .def("terrainObj", &cMap::terrainObj)//, return_value_policy<reference_existing_object>())
        .def("createObj", &cMap::createObj)//, return_value_policy<reference_existing_object>())
        .def("addObj", &cMap::addObj)
        .def("removeObj", &cMap::removeObj)
        .def("refreshObj", &cMap::refreshObj)
        ;

    class_<TerrainResNode>("TerrainResNode", init<>())
        .def_readwrite("id", &TerrainResNode::id)
        .def_readwrite("type", &TerrainResNode::type)
        .def_readwrite("path", &TerrainResNode::path)
        ;

    typedef std::vector<TerrainResNode> TRNVector;
    class_<TRNVector>("TRNVector")
        .def(vector_indexing_suite< TRNVector > ())
        ;

    class_<cTerrainRes, boost::noncopyable>("cTerrainRes", no_init)
        .def("getResPool", &cTerrainRes::getResPool, return_value_policy<reference_existing_object>())
        .def("getType", &cTerrainRes::getType)
        .def("getPath", &cTerrainRes::getPath)
        ;

    def("getTerrainRes", getTerrainRes, return_value_policy<reference_existing_object>());

    //粒子系统
    class_<cParticleSystem>("cParticleSystem", init<int, LPCSTR>())
        .def("isDead", &cParticleSystem::isDead)
        .def("reset", &cParticleSystem::reset)
        ;

    class_<cSnow, bases<cParticleSystem>>("cSnow", init<int, LPCSTR, I3DObject*>())
        .def("setSource", &cSnow::setSource)
        .def("setRange", &cSnow::setRange)
        .def("show", &cSnow::show)
        .def("visible", &cSnow::visible)
        ;

}
