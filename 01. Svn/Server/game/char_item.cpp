// Add to includes:
#if defined(ENABLE_MONSTER_CARD)
#	include "MonstercardSystem.h"
#endif

// Add the following `item->GetVnum` function anywhere in this file:
#if defined(ENABLE_MONSTER_CARD)
	if (item->GetVnum() == kalisto::MonstercardSystem::s_MONSTERCARD_VNUM || item->GetVnum() == kalisto::MonstercardSystem::s_MONSTERCARD_TRADEABLE_VNUM ||
		item->GetVnum() == kalisto::MonstercardSystem::s_MONSTERCARD_USE_ITEM_VNUM_A || item->GetVnum() == kalisto::MonstercardSystem::s_MONSTERCARD_USE_ITEM_VNUM_B)
	{
		const auto& sys = GetMonsterCardSystem();
		if (sys != nullptr)
		{
			if (sys->UseMonstercard(item))
			{
				item->SetCount(item->GetCount() - 1);
				return true;
			}
			return false;
		}
	}
#endif

// Add the following `CHARACTER::CountSpecifyItemBySocket` function anywhere in this file:
#if defined(ENABLE_MONSTER_CARD)
int CHARACTER::CountSpecifyItemBySocket(DWORD vnum, int socketIndex, int socketValue, int iExceptionCell) const
{
	int iCount = 0;
	LPITEM pItem = nullptr;

#if defined(__EXTEND_INVEN_SYSTEM__)
	for (WORD wCell = 0; wCell < GetExtendInvenMax(); ++wCell)
#else
	for (WORD wCell = 0; wCell < INVENTORY_MAX_NUM; ++wCell)
#endif
	{
		if (wCell == iExceptionCell)
			continue;

		pItem = GetInventoryItem(wCell);
		if (pItem && pItem->GetVnum() == vnum)
		{
			if (socketIndex >= 0 && pItem->GetSocket(socketIndex) != socketValue)
				continue;

			if (m_pkMyShop && m_pkMyShop->IsSellingItem(pItem->GetID()))
				continue;

			iCount += pItem->GetCount();
		}
	}

	return iCount;
}

void CHARACTER::RemoveSpecifyItemBySocket(DWORD vnum, DWORD count, int socketIndex, int socketValue, int iExceptionCell)
{
	if (0 == count)
		return;

#if defined(__EXTEND_INVEN_SYSTEM__)
	for (UINT i = 0; i < GetExtendInvenMax(); ++i)
#else
	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
		if (static_cast<int>(i) == iExceptionCell)
			continue;

		LPITEM curItem = GetInventoryItem(i);
		if (curItem == nullptr)
			continue;

		if (curItem->GetVnum() != vnum)
			continue;

		if (socketIndex >= 0 && curItem->GetSocket(socketIndex) != socketValue)
			continue;

		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(curItem->GetID());
			if (isItemSelling)
				continue;
		}

		if (count >= curItem->GetCount())
		{
			count -= curItem->GetCount();
			curItem->SetCount(0);
			if (0 == count)
				return;
		}
		else
		{
			curItem->SetCount(curItem->GetCount() - count);
			return;
		}
	}

	if (count)
		sys_log(0, "CHARACTER::RemoveSpecifyItemBySocket cannot remove enough item vnum %u, still remain %u", vnum, count);
}
#endif

// Before
		case 71093: // Polymorph Marble
		{
			sys_log(0, "USE_POLYMORPH_BALL PID(%d) vnum(%d)", GetPlayerID(), dwVnum);

			int iPolymorphLevelLimit = MAX(0, 20 - GetLevel() * 3 / 10);
			if (pMob->m_table.bLevel >= GetLevel() + iPolymorphLevelLimit)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot transform into a monster who has a higher level than you."));
				return false;
			}

			int iDuration = GetSkillLevel(POLYMORPH_SKILL_ID) == 0 ? 5 : (5 + (5 + GetSkillLevel(POLYMORPH_SKILL_ID) / 40 * 25));
			iDuration *= 60;

			DWORD dwBonus = 0;

			if (true == LC_IsYMIR() || true == LC_IsKorea())
			{
				dwBonus = GetSkillLevel(POLYMORPH_SKILL_ID) + 60;
			}
			else
			{
				dwBonus = (2 + GetSkillLevel(POLYMORPH_SKILL_ID) / 40) * 100;
			}

			AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
			{
				AddAffect(AFFECT_POLYMORPH, POINT_ATT_BONUS, dwBonus, AFF_POLYMORPH, iDuration, 0, false);
			}

			item->SetCount(item->GetCount() - 1);
		}

// After
		case 71093: // Polymorph Marble
		{
			sys_log(0, "USE_POLYMORPH_BALL PID(%d) vnum(%d)", GetPlayerID(), dwVnum);

			int iPolymorphLevelLimit = MAX(0, 20 - GetLevel() * 3 / 10);
			if (pMob->m_table.bLevel >= GetLevel() + iPolymorphLevelLimit)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_STRING("You cannot transform into a monster who has a higher level than you."));
				return false;
			}

			int iDuration = GetSkillLevel(POLYMORPH_SKILL_ID) == 0 ? 5 : (5 + (5 + GetSkillLevel(POLYMORPH_SKILL_ID) / 40 * 25));
			iDuration *= 60;

			DWORD dwBonus = 0;

			if (true == LC_IsYMIR() || true == LC_IsKorea())
			{
				dwBonus = GetSkillLevel(POLYMORPH_SKILL_ID) + 60;
			}
			else
			{
				dwBonus = (2 + GetSkillLevel(POLYMORPH_SKILL_ID) / 40) * 100;
			}

			AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
#if defined(ENABLE_MONSTER_CARD)
			if (item->GetSocket(1) != kalisto::MonstercardSystem::s_POLY_FROM_MONSTER_CARD_MARKER_SOCKET1)
#endif
			{
				AddAffect(AFFECT_POLYMORPH, POINT_ATT_BONUS, dwBonus, AFF_POLYMORPH, iDuration, 0, false);
			}

			item->SetCount(item->GetCount() - 1);
		}
