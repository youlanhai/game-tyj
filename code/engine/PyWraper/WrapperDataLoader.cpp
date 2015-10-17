
#include "stdafx.h"
#include "Header.h"
#include "..\LZData\lzdheader.h"
#include "WrapperDataLoader.h"

const std::string & testG(const std::string & t)
{
    return t;
}

void wrapperDataLoader()
{
    def("testG", testG, return_internal_reference<>());

    class_<LZData, LZDataPtr, boost::noncopyable>("LZDataPtr", no_init)
        .def("setTag", &LZData::setTag)
        .def("tag", &LZData::tag, return_internal_reference<>())
        .def("read", &LZData::read)
        .def("write", &LZData::write)
        .def("asInt", &LZData::asInt)
        .def("asBool", &LZData::asBool)
        .def("asFloat", &LZData::asDouble)
        .def("asString", &LZData::asString, return_internal_reference<>())
        .def("setInt", &LZData::setInt)
        .def("setBool", &LZData::setBool)
        .def("setFloat", &LZData::setDouble)
        .def("setString", &LZData::setString)
        .def("readBool", &LZData::readBool)
        .def("readFloat", &LZData::readDouble)
        .def("readString", &LZData::readString)
        .def("writeInt", &LZData::writeInt)
        .def("writeBool", &LZData::writeBool)
        .def("writeFloat", &LZData::writeDouble)
        .def("writeString", &LZData::writeString)
        .def("countChildren", &LZData::countChildren)
        .def("addChild", &LZData::addChild)
        .def("getChild", &LZData::getChild)
        .def("findChild", &LZData::findChild)
        .def("findNextChild", &LZData::findNextChild)
        ;

    def("createLzdFromFile", createLzdFromFile);
    def("createLzdFromBuffer", createLzdFromBuffer);
}
