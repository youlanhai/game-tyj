
#include "stdafx.h"
#include "Header.h"
#include "..\LZ3DEngine\LZ3DEngine.h"
#include "WrapperTool.h"

#include <ostream>

struct WrapperUpdate : IRenderObj , wrapper<IRenderObj>
{
    void update(float fElapse)
    {
        get_override("update")();
    }
    void render(IDirect3DDevice9 * pDevice)
    {
    }
};

struct WrapperCallBack : callObj , wrapper<callObj>
{
    virtual void onDead(void)
    {
        get_override("onDead")();
    }
};

std::ostream & operator<<(std::ostream & out, const D3DXVECTOR3 & pos)
{
    out<<"("<<pos.x <<"," <<pos.y <<", "<< pos.z <<")";
    return out;
}

class TestA
{
public:
    int a;
};

class TestB
{
public:
    int fun(TestA a)
    {
        return a.a;
    }
};

void wrapperTool()
{
    class_<TestB>("TestB")
        .def("fun", &TestB::fun);

    class_<IBase>("Base", init<>())
        .add_property("engineManaged", &IBase::getManaged, &IBase::setManaged)
        ;

    //基本数据结构
    class_<D3DXVECTOR3>("VECTOR3", init<>())
        .def(init<float, float, float>())
        .def(init<D3DXVECTOR3>())
        .def_readwrite("x", &D3DXVECTOR3::x)
        .def_readwrite("y", &D3DXVECTOR3::y)
        .def_readwrite("z", &D3DXVECTOR3::z)
        .def(self += self)
        .def(self -= self)
        .def(self *= float())
        .def(self /= float())
        .def(self + self)
        .def(self - self)
        .def(self * float())
        .def(self / float())
        .def(self == self)
        .def(self != self)
        .def(self_ns::str(self))
        ;

    class_<Physics::Vector3, bases<D3DXVECTOR3>>("Vector3", init<>())
        .def(init<float, float, float>())
        .def(init<D3DXVECTOR3>())
        .def(init<Physics::Vector3>())
        .def("set", &Physics::Vector3::set)
        .def("distTo", &Physics::Vector3::distTo)
        .def("distToSq", &Physics::Vector3::distToSq)
        .def(self += self)
        .def(self -= self)
        .def(self *= float())
        .def(self /= float())
        .def(self + self)
        .def(self - self)
        .def(self * float())
        .def(self / float())
        .def(self == self)
        .def(self != self)
        .def(self_ns::str(self))
        ;

    class_<D3DXMATRIX>("MATRIX", init<>())
        .def(init<D3DXMATRIX>())
        .def(self += self)
        .def(self -= self)
        .def(self *= self)
        .def(self * float())
        .def(self / float())
        .def(self + self)
        .def(self - self)
        .def(self * self)
        .def(self *= float())
        .def(self /= float())
        .def(self == self)
        .def(self != self)
        ;

    class_<CPoint>("cPoint", init<>())
        .def(init<CPoint>())
        .def(init<LPARAM>())
        .def(init<int, int>())
        .def_readwrite("x", &CPoint::x)
        .def_readwrite("y", &CPoint::y)
        .def("set", &CPoint::set)
        .def("formatLParam", &CPoint::formatLParam)
        .def("toLParam", &CPoint::toLParam)
        .def(self == self)
        .def(self + self)
        .def(self += self)
        .def(self - self)
        .def(self -= self)
        ;

    class_<CSize>("cSize", init<>())
        .def(init<int, int>())
        .def(init<CSize>())
        .def_readwrite("w", &CSize::cx)
        .def_readwrite("h", &CSize::cy)
        .def("set", &CSize::set)
        .def(self == self)
        ;

    bool(CRect::*PFisIn)(const CPoint &) = &CRect::isIn;
    bool(CRect::*PFisIn2)(int, int) = &CRect::isIn;

    class_<CRect>("cRect", init<>())
        .def(init<int, int, int, int>())
        .def(init<CRect>())
        .def(init<CPoint, CPoint>())
        .def(init<CPoint, CSize>())
        .def("set", &CRect::set)
        .def_readwrite("left", &CRect::left)
        .def_readwrite("top", &CRect::top)
        .def_readwrite("right", &CRect::right)
        .def_readwrite("bottom", &CRect::bottom)
        .def_readonly("width", &CRect::width)
        .def_readonly("height", &CRect::height)
        .def("offset", &CRect::offset)
        .def("expand",&CRect::expand)
        .def("isIntersect", &CRect::isIntersect)
        .def("isIn", PFisIn)
        .def("isIn2", PFisIn2)
        ;


    class_<CKeyboard, boost::noncopyable>("CKeyboard", no_init)
        .def("isKeyDown", &CKeyboard::isKeyDown)
        .def("isKeyPress", &CKeyboard::isKeyPress)
        .def("isKeyUp", &CKeyboard::isKeyUp)
        ;


    //callback
    class_<WrapperUpdate, boost::noncopyable>("IRenderObj")
        .def("update", pure_virtual(&IRenderObj::update))
        ;

    class_<WrapperCallBack, boost::noncopyable>("iCallObj")
        .def("init", &callObj::init)
        .def("onDead", pure_virtual(&callObj::onDead))
        ;

    class_<CSceneFog>("cFog", init<>())
        .def("setFogType", &CSceneFog::setFogType)
        .def("setStartEnd", &CSceneFog::setStartEnd)
        .def("setUseRange", &CSceneFog::setUseRange)
        .def("setColor", &CSceneFog::setColor)
        .def("setDensity", &CSceneFog::setDensity)
        .def("show", &CSceneFog::show)
        .def("visible", &CSceneFog::visible)
        .def("toggle", &CSceneFog::toggle)
        ;

    class_<cSkyBox>("cSkyBox", init<>())
        .def("show", &cSkyBox::show)
        .def("visible", &cSkyBox::visible)
        .def("toggle", &cSkyBox::toggle)
        .def("setSkyImage", &cSkyBox::setSkyImage)
        .def("setSkyRange", &cSkyBox::setSkyRange)
        .def("setSource", &cSkyBox::setSource)
        .def("getSource", &cSkyBox::getSource, return_value_policy<reference_existing_object>())
        ;

}