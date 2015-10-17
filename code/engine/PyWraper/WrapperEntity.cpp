
#include "stdafx.h"
#include "Header.h"
#include "..\LZ3DEngine\LZ3DEngine.h"
#include "WrapperEntity.h"


template<typename T>
struct WrapperPhysics : public T, public wrapper<T>
{
    VT1(RT_VOID, void, onMoveToFinished, bool);
    VT1(RT_VOID, void, onMoveToEntityFinish, bool);
    VT1(RT_VOID, void, onStateChange, DWORD);
    VT(return, bool, isPlayers);
    VT(RT_VOID, void, onAITrigger);
};
typedef WrapperPhysics<iPhysics> WraperIPhysics;

//////////////////////////////////////////////////////////////////////////

struct WrapperIEntity : public iEntity, public wrapper<iEntity>
{
    typedef iEntity T;
    VT(return, bool, canSelect);
    VT1(RT_VOID, void, onFocusCursor, UINT);

    void py_setModel(iModel* p)
    {
        setModel(p);
    }
    iModel* py_getModel()
    {
        return getModel().get();
    }

    void py_setPhysics(iPhysics *p)
    {
        setPhysics(p);
    }

    iPhysics * py_getPhysics()
    {
        return getPhysics().get();
    }
};

//////////////////////////////////////////////////////////////////////////

ModelPtr getNullModel(){ return ModelPtr(NULL); }

//////////////////////////////////////////////////////////////////////////

void wrapperEntity()
{

    void (I3DObject::*SET_SCALE)(float) = &I3DObject::setScale;
    void (I3DObject::*SET_SPEED)(float) = &I3DObject::setSpeed;

    class_<I3DObject>("I3DObject")
        .def_readwrite("position", &I3DObject::m_vPos)
        .def_readwrite("look", &I3DObject::m_vLook)
        .def_readwrite("up", &I3DObject::m_vUp)
        .def_readwrite("right", &I3DObject::m_vRight)
        .def_readwrite("speed", &I3DObject::m_vSpeed)
        .def_readwrite("rotSpeed", &I3DObject::m_vRotSpeed)
        .def_readwrite("scale", &I3DObject::m_vScale)
        .def("setScale", SET_SCALE)
        .def("setSpeed", SET_SPEED)
        .def("moveLook", &I3DObject::moveLook)
        .def("moveUp", &I3DObject::moveUp)
        .def("moveRight", &I3DObject::moveRight)
        .def("rotationUp", &I3DObject::rotationUp)
        .def("rotationLook", &I3DObject::rotationLook)
        .def("rotationRight", &I3DObject::rotationRight)
        .def("isPlayer", &I3DObject::isPlayer)
        .def("show", &I3DObject::show)
        .def("visible", &I3DObject::visible)
        .def("enableLockHeight", &I3DObject::enableLockHeight)
        .def("setLockHeight", &I3DObject::setLockHeight)
        ;

    class_<WraperIPhysics, bases<IBase>, boost::noncopyable>("iPhysics")
        .def("enable", &iPhysics::enable)
        .def("isEnable", &iPhysics::isEnabled)
        .def("setSource", &iPhysics::setSource)
        .def("getSource", &iPhysics::getSource, return_value_policy<reference_existing_object>())
        .def("getState", &iPhysics::getState)
        .def("setState", &iPhysics::setState)
        .def("searchPath", &iPhysics::searchPath)
        .def("faceTo", &iPhysics::faceTo)
        .def("faceToDir", &iPhysics::faceToDir)
        .def("moveTo", &iPhysics::moveTo)
        .def("moveToEntity", &iPhysics::moveToEntity)
        .def("breakAutoMove", &iPhysics::breakAutoMove)
        .def("onStateChange", &iPhysics::onStateChange, &WraperIPhysics::default_onStateChange)
        .def("onMoveToFinished", &iPhysics::onMoveToFinished, &WraperIPhysics::default_onMoveToFinished)
        .def("onMoveToEntityFinish", &iPhysics::onMoveToEntityFinish, &WraperIPhysics::default_onMoveToEntityFinish)
        .def("isPlayers", &iPhysics::isPlayers, &WraperIPhysics::default_isPlayers)
        .def("onAITrigger", &iPhysics::onAITrigger, &WraperIPhysics::default_onAITrigger)
        .def("enableAI", &iPhysics::enableAI)
        .def("isAIEnable", &iPhysics::isAIEnable)
        .def("setAIInterval", &iPhysics::setAIInterval)
        .def("getAIInterval", &iPhysics::getAIInterval)
        ;

    class_<iModel, ModelPtr, boost::noncopyable> ("iModel")
        .add_property("scale", &iModel::getScale, &iModel::setScale)
        .add_property("yaw", &iModel::getYaw, &iModel::setYaw)
        .add_property("pitch", &iModel::getPitch, &iModel::setPicth)
        .add_property("roll", &iModel::getRoll, &iModel::setRoll)
        .def("getWorldMatrix", &iModel::getWorldMatrix, return_value_policy<copy_const_reference>())
        .def("setWorldMatrix", &iModel::setWorldMatrix)
        .def("isSkeleton", &iModel::isSkeleton)
        .def("showBound", &iModel::showBound)
        .def("getBBRadius", &iModel::getBBRadius)
        .def("getBBCenter", &iModel::getBBCenter)
        .def("playAction", &iModel::playAction)
        .def("stopAction", &iModel::stopAction)
        .def("getActionCount", &iModel::getActionCount)
        .def("getActionName", &iModel::getActionName)
        .def("setAnimSpeed", &iModel::setAnimSpeed)
        ;

    def("getNullModel", getNullModel);
    def("loadModel", cModelFactory::createObj); //, return_value_policy<manage_new_object>()

    class_<WrapperIEntity, bases<I3DObject>, boost::noncopyable>("iEntity")
        .add_property("id", &iEntity::getID)
        .add_property("showDistance", &iEntity::getShowDistance, &iEntity::setShowDistance)
        .add_property("model", &iEntity::getModel, &WrapperIEntity::py_setModel)
        .def("setPhysics", &WrapperIEntity::py_setPhysics)
        .def("getPhysics", &WrapperIEntity::py_getPhysics, return_value_policy<reference_existing_object>())
        .def("setModel", &WrapperIEntity::py_setModel)
        .def("getModel", &WrapperIEntity::py_getModel, return_value_policy<reference_existing_object>())
        .def("setTopboard", &iEntity::setTopboard)
        .def("enableSphereShow", &iEntity::enableSphereShow)
        .def("show", &iEntity::show)
        .def("isSphereShowEnable", &iEntity::isSphereShowEnable)
        .def("isActive", &iEntity::isActive)
        .def("distToCamera", &iEntity::distToCamera)
        .def("distToPlayer", &iEntity::distToPlayer)
        .def("lookAtPosition", &iEntity::lookAtPosition)
        .def("canSelect", &iEntity::canSelect, &WrapperIEntity::default_canSelect)
        .def("onFocusCursor", &iEntity::onFocusCursor, &WrapperIEntity::default_onFocusCursor)
        ;

    class_<cEntityMgr>("cEntityMgr", init<>())
        .def("handleMouseEvent", &cEntityMgr::handleMouseEvent)
        .def("entity", &cEntityMgr::entity, return_value_policy<reference_existing_object>())
        .def("getActiveEntity", &cEntityMgr::getActiveEntity, return_value_policy<reference_existing_object>())
        .def("player", &cEntityMgr::player, return_value_policy<reference_existing_object>())
        .def("setPlayer", &cEntityMgr::setPlayer)
        .def("add", &cEntityMgr::add)
        .def("remove", &cEntityMgr::remove)
        .def("clear", &cEntityMgr::clear)
        ;

    class_<CCamera>("cCamera", no_init)
        .add_property("speed", &CCamera::speed, &CCamera::setSpeed)
        .add_property("distToPlayer", &CCamera::getDistance, &CCamera::setDistance)
        .add_property("yaw", &CCamera::yaw)
        .add_property("pitch", &CCamera::pitch)
        .add_property("roll", &CCamera::roll)
        .def("getPosition", &CCamera::position, return_value_policy<copy_const_reference>())
        .def("setPosition", &CCamera::setPosition)
        .def("getLook", &CCamera::look, return_value_policy<copy_const_reference>())
        .def("getRight", &CCamera::right, return_value_policy<copy_const_reference>())
        .def("getUp", &CCamera::up, return_value_policy<copy_const_reference>())
        .def("rotYaw", &CCamera::rotYaw)
        .def("rotPitch", &CCamera::rotPitch)
        .def("rotRoll", &CCamera::rotRoll)
        .def("moveLook", &CCamera::moveLook)
        .def("moveUp", &CCamera::moveUp)
        .def("moveRight", &CCamera::moveRight)
        .def("setNearFar", &CCamera::setNearFar)
        .def("setDistRange", &CCamera::setDistRange)
        ;

    class_<CCursorCamera, bases<CCamera>>("cCursorCamera", no_init)
        .add_property("rotSpeed", &CCursorCamera::getRotateSpeed, &CCursorCamera::setCurRoSpeed)
        .def("setSource", &CCursorCamera::setSource)
        .def("handleMessage", &CCursorCamera::handleMessage)
        .def("isDraged", &CCursorCamera::isDraged)
        .def("setHeight", &CCursorCamera::setHeight)
        ;
}
