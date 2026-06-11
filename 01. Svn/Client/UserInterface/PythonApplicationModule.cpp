// In `appUpdateEffects(PyObject* poSelf, PyObject* poArgs)`, find this block:
{
	CPythonApplication::Instance().UpdateEffects();
	return Py_BuildNone();
}

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
PyObject* appIllustratedCreate(PyObject* poSelf, PyObject* poArgs)
{
	CPythonApplication::Instance().IllustratedCreate();
	return Py_BuildNone();
}
#endif

// Find this line:
{ "GetTextWidth", appGetTextWidth, METH_VARARGS },

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
		{ "IllustratedCreate", appIllustratedCreate, METH_VARARGS },
#endif

// Find this line:
PyModule_AddIntConstant(poModule, "ENABLE_EXTEND_MALLBOX", 0);

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	PyModule_AddIntConstant(poModule, "ENABLE_MONSTER_CARD", 1);
	#if defined(ENABLE_MONSTER_CARD_ACHIEV)
	PyModule_AddIntConstant(poModule, "ENABLE_MONSTER_CARD_ACHIEV", 1);
	#else
	PyModule_AddIntConstant(poModule, "ENABLE_MONSTER_CARD_ACHIEV", 0);
	#endif
#else
	PyModule_AddIntConstant(poModule, "ENABLE_MONSTER_CARD", 0);
#endif

// Find this line:
PyModule_AddIntConstant(poModule, "ENABLE_MONSTER_CARD", 0);

// Add after it:
	#if defined(ENABLE_MONSTER_CARD)
	PyModule_AddIntConstant(poModule, "RENDER_TARGET_INDEX_ILLUSTRATED", CRenderTargetManager::RENDER_TARGET_INDEX_ILLUSTRATED);
	#endif
