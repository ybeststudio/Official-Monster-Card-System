// Find this line:
const CSpecialAttrGroup* GetSpecialAttrGroup(DWORD dwVnum);

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	CDropItemGroup* FindDropItemGroupByMobVnum(DWORD dwMobVnum);
#endif
