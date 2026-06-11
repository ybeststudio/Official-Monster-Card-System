// Add to includes:
#if defined(ENABLE_MONSTER_CARD)
#	include "MonstercardSystem.h"
#endif

// In `LPITEM ITEM_MANAGER::CreateItem(DWORD vnum, DWORD count, DWORD id, bool bTryMagic, int iRarePct, bool bSkipSave, bool bSkilAddon)`, extend the related check like this:
#if defined(ENABLE_MONSTER_CARD)
	if (0 == id && (item->GetVnum() == kalisto::MonstercardSystem::s_MONSTERCARD_USE_ITEM_VNUM_A || item->GetVnum() == kalisto::MonstercardSystem::s_MONSTERCARD_USE_ITEM_VNUM_B))
	{
		if (item->GetSocket(0) == 0 && item->GetSocket(1) == 0)
		{
			const std::size_t race = kalisto::MonstercardSystem::RollMonsterCardUseItemRace();
			if (race != 0)
				item->SetSocket(0, static_cast<long>(race));
		}
	}
#endif

// Add the following `ITEM_MANAGER::FindDropItemGroupByMobVnum` function anywhere in this file:
#if defined(ENABLE_MONSTER_CARD)
CDropItemGroup* ITEM_MANAGER::FindDropItemGroupByMobVnum(DWORD dwMobVnum)
{
	const auto it = m_map_pkDropItemGroup.find(dwMobVnum);
	if (it == m_map_pkDropItemGroup.end())
		return nullptr;
	return it->second;
}
#endif
