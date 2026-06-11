#include "StdAfx.h"

#if defined(ENABLE_MONSTER_CARD)

#include "PythonInterfaceModelModule.h"

static PyObject* interfaceModelModuleCreate(PyObject* /*poSelf*/, PyObject* /*poArgs*/)
{
	return Py_BuildValue("i", 0);
}

static PyObject* interfaceModelModuleDestroy(PyObject* /*poSelf*/, PyObject* /*poArgs*/)
{
	return Py_BuildNone();
}

static PyObject* interfaceModelModuleNoop(PyObject* /*poSelf*/, PyObject* /*poArgs*/)
{
	return Py_BuildNone();
}

void kalisto::initInterfaceModleModule()
{
	static PyMethodDef s_methods[] = {
		{ "Create", interfaceModelModuleCreate, METH_VARARGS },
		{ "Destroy", interfaceModelModuleDestroy, METH_VARARGS },
		{ "EnableAutoRotation", interfaceModelModuleNoop, METH_VARARGS },
		{ "DisableAutoRotation", interfaceModelModuleNoop, METH_VARARGS },
		{ "Show", interfaceModelModuleNoop, METH_VARARGS },
		{ "Hide", interfaceModelModuleNoop, METH_VARARGS },
		{ "ShowModel", interfaceModelModuleNoop, METH_VARARGS },
		{ "HideModel", interfaceModelModuleNoop, METH_VARARGS },
		{ "ShowBackground", interfaceModelModuleNoop, METH_VARARGS },
		{ "HideBackground", interfaceModelModuleNoop, METH_VARARGS },
		{ "SetSize", interfaceModelModuleNoop, METH_VARARGS },
		{ "SetPosition", interfaceModelModuleNoop, METH_VARARGS },
		{ "GetSize", interfaceModelModuleNoop, METH_VARARGS },
		{ "GetPosition", interfaceModelModuleNoop, METH_VARARGS },
		{ "SetBackgroundFile", interfaceModelModuleNoop, METH_VARARGS },
		{ "SetModelRace", interfaceModelModuleNoop, METH_VARARGS },
		{ "RotateLeft", interfaceModelModuleNoop, METH_VARARGS },
		{ "RotateRight", interfaceModelModuleNoop, METH_VARARGS },
		{ "RotateUp", interfaceModelModuleNoop, METH_VARARGS },
		{ "RotateDown", interfaceModelModuleNoop, METH_VARARGS },
		{ "ZoomIn", interfaceModelModuleNoop, METH_VARARGS },
		{ "ZoomOut", interfaceModelModuleNoop, METH_VARARGS },
		{ "SetParentWindow", interfaceModelModuleNoop, METH_VARARGS },
		{ "RestoreStartPosition", interfaceModelModuleNoop, METH_VARARGS },
		{ "GetVirtualID", interfaceModelModuleCreate, METH_VARARGS },
		{ nullptr, nullptr }
	};

	Py_InitModule("interfaceModelModule", s_methods);
}

#endif // ENABLE_MONSTER_CARD

