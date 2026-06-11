// Add to includes:
#if defined(ENABLE_MONSTER_CARD)
#	include "PythonMonsterCardManager.h"
#	include <cstdlib>
#endif

// Add the following `static_cast<DWORD>` function anywhere in this file:
#if defined(ENABLE_MONSTER_CARD)
	// MonsterCard server-side uses CHAT_TYPE_COMMAND text stream.
	// We intercept and translate it into data queried by official uimonstercard.py.
	if (0 == strncmp(szCmd, "monstercardsystem", 17) || 0 == strncmp(c_szCommand, "MONSTERCARDSYSTEM", 17))
	{
		// Expect "MONSTERCARDSYSTEM <payload>" or "MONSTERCARDSYSTEM <token>" etc.
		std::string cmdLine(c_szCommand);
		const std::string prefix("MONSTERCARDSYSTEM ");
		std::string payload = cmdLine;
		if (payload.find(prefix) == 0)
			payload = payload.substr(prefix.size());
		else
		{
			// lower-case path: TokenVector already lower-cased in OneArgument, but SplitToken doesn't.
			const std::string prefix2("monstercardsystem ");
			if (payload.find(prefix2) == 0)
				payload = payload.substr(prefix2.size());
		}

		CPythonMonsterCardManager& mgr = CPythonMonsterCardManager::Instance();
		mgr.LoadTable();

		// payload formats from server code:
		// ADD_DATA/Level/<n>
		// ADD_DATA/Cards/<22 ints>
		// NEW_MISSION/<16 ints>
		// REC_MAINCARDS/<c0>/<c1>/<c2>/... (indices ignored)
		// ADD_MOB_INFO/<vnum>/<collected>/<kills>/<need>/<stage>
		std::vector<std::string> parts;
		parts.reserve(32);
		std::string cur;
		for (std::size_t i = 0; i < payload.size(); ++i)
		{
			const char ch = payload[i];
			if (ch == '/')
			{
				parts.push_back(cur);
				cur.clear();
			}
			else
			{
				cur.push_back(ch);
			}
		}
		if (!cur.empty())
			parts.push_back(cur);

		if (!parts.empty())
		{
			if (parts[0] == "ADD_DATA" && parts.size() >= 3)
			{
				if (parts[1] == "Level")
					mgr.SetMissionStage(static_cast<int>(std::strtol(parts[2].c_str(), nullptr, 10)));
				// Payload: ADD_DATA/Cards/<22 ints> -> parts[0..1] + 22 numbers => parts.size() == 24
				else if (parts[1] == "Cards" && parts.size() >= 24)
				{
					// Cards payload is 22 values: 3 main + 16 deck + 3 killed
					DWORD main0 = static_cast<DWORD>(std::strtoul(parts[2 + 0].c_str(), nullptr, 10));
					DWORD main1 = static_cast<DWORD>(std::strtoul(parts[2 + 1].c_str(), nullptr, 10));
					DWORD main2 = static_cast<DWORD>(std::strtoul(parts[2 + 2].c_str(), nullptr, 10));
					mgr.SetMissionMainCards(main0, main1, main2);

					std::vector<DWORD> deck;
					deck.reserve(16);
					for (int d = 0; d < 16; ++d)
						deck.push_back(static_cast<DWORD>(std::strtoul(parts[2 + 3 + d].c_str(), nullptr, 10)));
					mgr.SetMissionDeck(deck);

					const int k0 = static_cast<int>(std::strtol(parts[2 + 3 + 16 + 0].c_str(), nullptr, 10));
					const int k1 = static_cast<int>(std::strtol(parts[2 + 3 + 16 + 1].c_str(), nullptr, 10));
					const int k2 = static_cast<int>(std::strtol(parts[2 + 3 + 16 + 2].c_str(), nullptr, 10));
					mgr.SetMissionKilledFlags(k0, k1, k2);
				}
				return;
			}
			if (parts[0] == "NEW_MISSION" && parts.size() >= 1 + 16)
			{
				std::vector<DWORD> deck;
				deck.reserve(16);
				for (int i = 0; i < 16; ++i)
					deck.push_back(static_cast<DWORD>(std::strtoul(parts[1 + i].c_str(), nullptr, 10)));
				mgr.SetMissionDeck(deck);
				mgr.SetMissionLoaded(true);
				return;
			}
			if (parts[0] == "REC_MAINCARDS" && parts.size() >= 4)
			{
				mgr.SetMissionMainCards(
					static_cast<DWORD>(std::strtoul(parts[1].c_str(), nullptr, 10)),
					static_cast<DWORD>(std::strtoul(parts[2].c_str(), nullptr, 10)),
					static_cast<DWORD>(std::strtoul(parts[3].c_str(), nullptr, 10))
				);
				mgr.SetMissionLoaded(true);
				if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshMissionPage", Py_BuildValue("()"));
				return;
			}
			if (parts[0] == "SUCCES_KILL" && parts.size() >= 2)
			{
				// kill flags already come via ADD_DATA/Cards; ignore.
				mgr.SetMissionLoaded(true);
				return;
			}
			if (parts[0] == "ADD_MOB_INFO" && parts.size() >= 6)
			{
				CPythonMonsterCardManager::SMobInfo info {};
				const DWORD vnum = static_cast<DWORD>(std::strtoul(parts[1].c_str(), nullptr, 10));
				info.collectedCards = static_cast<int>(std::strtol(parts[2].c_str(), nullptr, 10));
				info.killCount = static_cast<int>(std::strtol(parts[3].c_str(), nullptr, 10));
				info.needCards = static_cast<int>(std::strtol(parts[4].c_str(), nullptr, 10));
				info.stage = static_cast<int>(std::strtol(parts[5].c_str(), nullptr, 10));
				info.lastTeleport = 0;
				info.lastPoly = 0;
				info.lastSpawn = 0;
				// Backward compatible: newer server sends timestamps too.
				if (parts.size() >= 9)
				{
					info.lastTeleport = static_cast<long long>(std::strtoll(parts[6].c_str(), nullptr, 10));
					info.lastPoly = static_cast<long long>(std::strtoll(parts[7].c_str(), nullptr, 10));
					info.lastSpawn = static_cast<long long>(std::strtoll(parts[8].c_str(), nullptr, 10));
				}
				mgr.UpsertMobInfo(vnum, info);
				mgr.SetIllustrationLoaded(true);
				// Refresh UI immediately so star count updates without reopening.
				if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MonsterCardIllustrationRefresh", Py_BuildValue("()"));
				return;
			}
			if (parts[0] == "NO_NEW_ORDER" && parts.size() >= 2)
			{
				const long long seconds = static_cast<long long>(std::strtoll(parts[1].c_str(), nullptr, 10));
				if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MonsterCardShowCooldown", Py_BuildValue("(L)", seconds));
				return;
			}
			if (parts[0] == "NO_NEED_STAGE" && parts.size() >= 2)
			{
				const int needStage = static_cast<int>(std::strtol(parts[1].c_str(), nullptr, 10));
				if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MonsterCardShowNeedStage", Py_BuildValue("(i)", needStage));
				return;
			}
			if (parts[0] == "ACHIEV_APPLY" && parts.size() >= 3)
			{
				const DWORD vnum = static_cast<DWORD>(std::strtoul(parts[1].c_str(), nullptr, 10));
				const int applied = static_cast<int>(std::strtol(parts[2].c_str(), nullptr, 10));
				mgr.SetAchievApplied(vnum, applied != 0);
				if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MonsterCardAchievRefresh", Py_BuildValue("()"));
				return;
			}
			if (parts[0] == "ACHIEV_REGIST" && parts.size() >= 3)
			{
				const DWORD vnum = static_cast<DWORD>(std::strtoul(parts[1].c_str(), nullptr, 10));
				const int rank = static_cast<int>(std::strtol(parts[2].c_str(), nullptr, 10));
				mgr.SetAchievRegistRank(vnum, rank);
				if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MonsterCardAchievRefresh", Py_BuildValue("()"));
				return;
			}
			if (parts[0] == "MISSION_FAIL" && parts.size() >= 3)
			{
				const int type = static_cast<int>(std::strtol(parts[1].c_str(), nullptr, 10));
				const int data = static_cast<int>(std::strtol(parts[2].c_str(), nullptr, 10));
				if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "MonsterCardMissionFail", Py_BuildValue("(ii)", type, data));
				return;
			}
			if (parts[0] == "OPEN")
			{
				if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
					PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshMissionPage", Py_BuildValue("()"));
				return;
			}
		}
		return;
	}
#endif
