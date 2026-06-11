// Add to includes:
#if defined(ENABLE_MONSTER_CARD)
#include "PythonMonsterCardManager.h"
PyObject* playerIllustrationSelectModel(PyObject* poSelf, PyObject* poArgs)
{
	int iVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVnum))
		return Py_BadArgument();

	return Py_BuildValue("b", CPythonIllustratedManager::Instance().SelectModel(iVnum));
}

PyObject* playerIllustrationShow(PyObject* poSelf, PyObject* poArgs)
{
	bool isShow;
	if (!PyTuple_GetBoolean(poArgs, 0, &isShow))
		return Py_BadArgument();

	CPythonIllustratedManager::Instance().SetShow(isShow);
	return Py_BuildNone();
}

PyObject* playerIllustrationChangeMotion(PyObject* poSelf, PyObject* poArgs)
{
	int iVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVnum))
		return Py_BadArgument();

	CPythonIllustratedManager::Instance().ChangeMotion(iVnum);
	return Py_BuildNone();
}

PyObject* playerIllustrationModelRotation(PyObject* poSelf, PyObject* poArgs)
{
	float fRot;
	if (!PyTuple_GetFloat(poArgs, 0, &fRot))
		return Py_BadArgument();

	CPythonIllustratedManager::Instance().ModelRotation(fRot);
	return Py_BuildNone();
}

PyObject* playerIllustrationModelUpDown(PyObject* poSelf, PyObject* poArgs)
{
	bool bUp;
	if (!PyTuple_GetBoolean(poArgs, 0, &bUp))
		return Py_BadArgument();

	CPythonIllustratedManager::Instance().ModelUpDown(bUp);
	return Py_BuildNone();
}

PyObject* playerIllustrationModelZoom(PyObject* poSelf, PyObject* poArgs)
{
	bool bZoom;
	if (!PyTuple_GetBoolean(poArgs, 0, &bZoom))
		return Py_BadArgument();

	CPythonIllustratedManager::Instance().ModelZoom(bZoom);
	return Py_BuildNone();
}

PyObject* playerIllustrationModelViewReset(PyObject* poSelf, PyObject* poArgs)
{
	CPythonIllustratedManager::Instance().ModelViewReset();
	return Py_BuildNone();
}

PyObject* playerIsMissionDataLoad(PyObject* /*poSelf*/, PyObject* /*poArgs*/)
{
	return Py_BuildValue("b", CPythonMonsterCardManager::Instance().IsMissionLoaded());
}

PyObject* playerGetMonsterCardMissionInfo(PyObject* /*poSelf*/, PyObject* /*poArgs*/)
{
	CPythonMonsterCardManager& mgr = CPythonMonsterCardManager::Instance();
	mgr.LoadTable();

	int stage = 0;
	std::vector<DWORD> mainCards;
	std::vector<int> clears;
	long long resetTime = 0;
	int resetCount = 0;
	int shuffleCount = 0;
	if (!mgr.GetMissionInfo(stage, mainCards, clears, resetTime, resetCount, shuffleCount))
		return Py_BuildValue("i", 0);

	PyObject* pyMobVnums = PyTuple_New(3);
	PyObject* pyMobClears = PyTuple_New(3);
	for (int i = 0; i < 3; ++i)
	{
		const DWORD v = (i < static_cast<int>(mainCards.size())) ? mainCards[i] : 0;
		const int c = (i < static_cast<int>(clears.size())) ? clears[i] : 0;
		PyTuple_SetItem(pyMobVnums, i, Py_BuildValue("i", v));
		PyTuple_SetItem(pyMobClears, i, Py_BuildValue("i", c));
	}

	// returns: (stage, (mob_vnum0..2), (mob_clear0..2), reset_time, reset_count, shuffle_count)
	return Py_BuildValue("iOOiii", stage, pyMobVnums, pyMobClears, static_cast<int>(resetTime), resetCount, shuffleCount);
}

PyObject* playerGetMissionVec(PyObject* /*poSelf*/, PyObject* poArgs)
{
	int group;
	if (!PyTuple_GetInteger(poArgs, 0, &group))
		return Py_BuildException();

	CPythonMonsterCardManager& mgr = CPythonMonsterCardManager::Instance();
	mgr.LoadTable();

	std::vector<CPythonMonsterCardManager::SMissionEntry> vec;
	mgr.GetMissionVec(group, vec);

	PyObject* pyList = PyList_New(static_cast<int>(vec.size()));
	for (int i = 0; i < static_cast<int>(vec.size()); ++i)
	{
		const auto& e = vec[i];
		PyObject* t = Py_BuildValue("iii iii",
			static_cast<int>(e.vnum),
			static_cast<int>(e.level),
			static_cast<int>(e.type),
			static_cast<int>(e.mapIndex0),
			static_cast<int>(e.mapIndex1),
			static_cast<int>(e.mapIndex2)
		);
		PyList_SetItem(pyList, i, t);
	}
	return pyList;
}

PyObject* playerGetMobEmergenceAreaIndex(PyObject* /*poSelf*/, PyObject* poArgs)
{
	int vnum;
	if (!PyTuple_GetInteger(poArgs, 0, &vnum))
		return Py_BuildException();

	std::vector<int> maps;
	if (!CPythonMonsterCardManager::Instance().GetMobEmergenceAreaIndex(static_cast<DWORD>(vnum), maps))
		return Py_BuildValue("i", 0);

	PyObject* pyTuple = PyTuple_New(static_cast<int>(maps.size()));
	for (int i = 0; i < static_cast<int>(maps.size()); ++i)
		PyTuple_SetItem(pyTuple, i, Py_BuildValue("i", maps[i]));
	return pyTuple;
}

PyObject* playerGetIllustrationFileLoad(PyObject* /*poSelf*/, PyObject* /*poArgs*/)
{
	return Py_BuildValue("b", CPythonMonsterCardManager::Instance().LoadTable());
}

PyObject* playerIsIllustrationDataLoad(PyObject* /*poSelf*/, PyObject* /*poArgs*/)
{
	return Py_BuildValue("b", CPythonMonsterCardManager::Instance().IsIllustrationLoaded());
}

PyObject* playerGetIllustrationSoloPageMax(PyObject* /*poSelf*/, PyObject* /*poArgs*/)
{
	CPythonMonsterCardManager& mgr = CPythonMonsterCardManager::Instance();
	mgr.LoadTable();
	return Py_BuildValue("i", mgr.GetIllustrationSoloPageMax());
}

PyObject* playerGetIllustrationPartyPageMax(PyObject* /*poSelf*/, PyObject* /*poArgs*/)
{
	CPythonMonsterCardManager& mgr = CPythonMonsterCardManager::Instance();
	mgr.LoadTable();
	return Py_BuildValue("i", mgr.GetIllustrationPartyPageMax());
}

static PyObject* BuildIllustrationPageTuple(const std::vector<CPythonMonsterCardManager::SMissionEntry>& vec)
{
	PyObject* pyList = PyList_New(static_cast<int>(vec.size()));
	for (int i = 0; i < static_cast<int>(vec.size()); ++i)
	{
		const auto& e = vec[i];
		PyObject* t = Py_BuildValue("iii iii",
			static_cast<int>(e.vnum),
			static_cast<int>(e.level),
			static_cast<int>(e.type),
			static_cast<int>(e.mapIndex0),
			static_cast<int>(e.mapIndex1),
			static_cast<int>(e.mapIndex2)
		);
		PyList_SetItem(pyList, i, t);
	}
	return pyList;
}

PyObject* playerGetIllustrationSoloPageData(PyObject* /*poSelf*/, PyObject* poArgs)
{
	int page;
	if (!PyTuple_GetInteger(poArgs, 0, &page))
		return Py_BuildException();
	CPythonMonsterCardManager& mgr = CPythonMonsterCardManager::Instance();
	mgr.LoadTable();
	std::vector<CPythonMonsterCardManager::SMissionEntry> out;
	mgr.GetIllustrationSoloPageData(page, out);
	return BuildIllustrationPageTuple(out);
}

PyObject* playerGetIllustrationPartyPageData(PyObject* /*poSelf*/, PyObject* poArgs)
{
	int page;
	if (!PyTuple_GetInteger(poArgs, 0, &page))
		return Py_BuildException();
	CPythonMonsterCardManager& mgr = CPythonMonsterCardManager::Instance();
	mgr.LoadTable();
	std::vector<CPythonMonsterCardManager::SMissionEntry> out;
	mgr.GetIllustrationPartyPageData(page, out);
	return BuildIllustrationPageTuple(out);
}

PyObject* playerGetIllustrationData(PyObject* /*poSelf*/, PyObject* poArgs)
{
	int vnum;
	if (!PyTuple_GetInteger(poArgs, 0, &vnum))
		return Py_BuildException();

	CPythonMonsterCardManager::SMobInfo info {};
	int accumulation = 0;
	int curCount = 0;
	int curClass = 0;
	int cool0 = 0;
	int cool1 = 0;
	if (CPythonMonsterCardManager::Instance().GetMobInfo(static_cast<DWORD>(vnum), info))
	{
		accumulation = info.collectedCards;
		// Official-like behavior: star class does NOT auto-increment when reaching the card threshold.
		// The client shows a flash effect at curCount==count_max, and promotion ("Geli?tir") moves to next star.
		// So use server-provided stage as curClass (0..5) and collectedCards as progress in current class.
		curClass = info.stage;
		curCount = info.collectedCards;
		if (curClass < 0)
			curClass = 0;
		if (curClass > 5)
			curClass = 5;
		if (curCount < 0)
			curCount = 0;

		// Cooldown timestamps are provided as epoch seconds from server.
		// UI expects "cooltime_end_timestamp".
		const long long polyWait = 3LL * 60LL * 60LL;   // 3 hours (official wiki)
		const long long warpWait = 30LL * 60LL;         // 30 minutes
		if (info.lastPoly > 0)
			cool0 = static_cast<int>(info.lastPoly + polyWait);
		if (info.lastTeleport > 0)
			cool1 = static_cast<int>(info.lastTeleport + warpWait);
	}
	return Py_BuildValue("iiiii", accumulation, curCount, curClass, cool0, cool1);
}

PyObject* playerIsMonsterCardAchievApplied(PyObject* /*poSelf*/, PyObject* poArgs)
{
	int vnum;
	if (!PyTuple_GetInteger(poArgs, 0, &vnum))
		return Py_BuildException();
	return Py_BuildValue("b", CPythonMonsterCardManager::Instance().IsAchievApplied(static_cast<DWORD>(vnum)));
}

PyObject* playerGetMonsterCardAchievRegistRank(PyObject* /*poSelf*/, PyObject* poArgs)
{
	int vnum;
	if (!PyTuple_GetInteger(poArgs, 0, &vnum))
		return Py_BuildException();
	return Py_BuildValue("i", CPythonMonsterCardManager::Instance().GetAchievRegistRank(static_cast<DWORD>(vnum)));
}
#endif

// Find this line:
{ "SetItemPetAttrChangeWindowActivedItemSlot", playerSetItemPetAttrChangeWindowActivedItemSlot, METH_VARARGS },

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
		// Monster Card
		{ "IllustrationSelectModel", playerIllustrationSelectModel, METH_VARARGS },
		{ "IllustrationShow", playerIllustrationShow, METH_VARARGS },
		{ "IllustrationChangeMotion", playerIllustrationChangeMotion, METH_VARARGS },
		{ "IllustrationModelRotation", playerIllustrationModelRotation, METH_VARARGS },
		{ "IllustrationModelUpDown", playerIllustrationModelUpDown, METH_VARARGS },
		{ "IllustrationModelZoom", playerIllustrationModelZoom, METH_VARARGS },
		{ "IllustrationModelViewReset", playerIllustrationModelViewReset, METH_VARARGS },

		{ "IsMissionDataLoad", playerIsMissionDataLoad, METH_VARARGS },
		{ "GetMonsterCardMissionInfo", playerGetMonsterCardMissionInfo, METH_VARARGS },
		{ "GetMissionVec", playerGetMissionVec, METH_VARARGS },
		{ "GetMobEmergenceAreaIndex", playerGetMobEmergenceAreaIndex, METH_VARARGS },

		{ "GetIllustrationFileLoad", playerGetIllustrationFileLoad, METH_VARARGS },
		{ "IsIllustrationDataLoad", playerIsIllustrationDataLoad, METH_VARARGS },
		{ "GetIllustrationSoloPageMax", playerGetIllustrationSoloPageMax, METH_VARARGS },
		{ "GetIllustrationPartyPageMax", playerGetIllustrationPartyPageMax, METH_VARARGS },
		{ "GetIllustrationSoloPageData", playerGetIllustrationSoloPageData, METH_VARARGS },
		{ "GetIllustrationPartyPageData", playerGetIllustrationPartyPageData, METH_VARARGS },
		{ "GetIllustrationData", playerGetIllustrationData, METH_VARARGS },

		{ "IsMonsterCardAchievApplied", playerIsMonsterCardAchievApplied, METH_VARARGS },
		{ "GetMonsterCardAchievRegistRank", playerGetMonsterCardAchievRegistRank, METH_VARARGS },
#endif
