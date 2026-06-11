// Find this line:
initwndMgr();

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	kalisto::initInterfaceModleModule();
#endif
