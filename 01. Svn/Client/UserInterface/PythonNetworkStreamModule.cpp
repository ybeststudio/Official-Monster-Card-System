// In `netSendPartyMatchCancel(PyObject* poSelf, PyObject* poArgs)`, find this block:
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	CPythonNetworkStream::Instance().PartyMatch(index, CPythonPlayer::PARTY_MATCH_CANCEL);
	return Py_BuildNone();
}

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
PyObject* netSendMissionMessage(PyObject* /*poSelf*/, PyObject* poArgs)
{
	int cmd;
	if (!PyTuple_GetInteger(poArgs, 0, &cmd))
		return Py_BuildException();

	char buf[64];
	_snprintf(buf, sizeof(buf), "/cardmonster %d", cmd);
	CPythonNetworkStream::Instance().SendChatPacket(buf);
	return Py_BuildNone();
}

PyObject* netSendIllustrationMessage(PyObject* /*poSelf*/, PyObject* poArgs)
{
	int feature;
	if (!PyTuple_GetInteger(poArgs, 0, &feature))
		return Py_BuildException();

	int mobVnum = 0;
	PyTuple_GetInteger(poArgs, 1, &mobVnum);

	char buf[128];
	_snprintf(buf, sizeof(buf), "/cardmonster 8 %d %d", feature, mobVnum);
	CPythonNetworkStream::Instance().SendChatPacket(buf);
	return Py_BuildNone();
}
#endif

// Find this line:
{ "SendPartyMatchCancel",				netSendPartyMatchCancel,				METH_VARARGS },

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
		{ "SendMissionMessage",					netSendMissionMessage,					METH_VARARGS },
		{ "SendIllustrationMessage",			netSendIllustrationMessage,				METH_VARARGS },
#endif

// Find this line:
PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_SUNGMA_IMMUNE", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_SUNGMA_IMMUNE);

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	// Mission (server-side /cardmonster command ids)
	PyModule_AddIntConstant(poModule, "REQUEST_MISSION", 2);
	PyModule_AddIntConstant(poModule, "RECIVE_MISSION", 3);
	PyModule_AddIntConstant(poModule, "SHUFFLE_MISSION", 6);
	PyModule_AddIntConstant(poModule, "REWARD_MISSION", 5);
	PyModule_AddIntConstant(poModule, "INIT_MISSION", 7);

	PyModule_AddIntConstant(poModule, "REQUEST_ILLUSTRATION", 2);

	// Illustration features (match server MonsterFeature enum indices)
	PyModule_AddIntConstant(poModule, "MC_PROMOTION", 6);
	PyModule_AddIntConstant(poModule, "MC_TRADE", 5);
	PyModule_AddIntConstant(poModule, "MC_POLY", 1);
	PyModule_AddIntConstant(poModule, "MC_WARP", 0);
	PyModule_AddIntConstant(poModule, "MC_SPAWN", 3);
	PyModule_AddIntConstant(poModule, "MC_REKRUTE", 4);
#endif
