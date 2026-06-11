// In `void CPythonPlayer::OnKeyDown(int iKey)`, extend the switch statement with:
#if defined(ENABLE_MONSTER_CARD)
			case KEY_MONSTER_CARD:
				PyCallClassMemberFunc(m_ppyGameWindow, "OpenWindow", Py_BuildValue("(is)", KEY_MONSTER_CARD, ""));
				break;
#endif
