// Add to includes:
#if defined(ENABLE_MONSTER_CARD)
#	include "MonstercardSystem.h"
#endif

// Before
			else
				pkAttacker->UpdateAlignment(2);
		}

		pkAttacker->SetQuestNPCID(GetVID());
		quest::CQuestManager::instance().Kill(pkAttacker->GetPlayerID(), GetRaceNum());
		CHARACTER_MANAGER::instance().KillLog(GetRaceNum());


#if defined(__PARTY_KILL_RENEWAL__)
		if (pkAttacker->GetParty())
		{

// After
			else
				pkAttacker->UpdateAlignment(2);
		}

		pkAttacker->SetQuestNPCID(GetVID());
		quest::CQuestManager::instance().Kill(pkAttacker->GetPlayerID(), GetRaceNum());
		CHARACTER_MANAGER::instance().KillLog(GetRaceNum());

#if defined(ENABLE_MONSTER_CARD)
		if (pkAttacker->GetMonsterCardSystem() != nullptr && IsMonstercardTarget())
			pkAttacker->GetMonsterCardSystem()->OnKillMonsterCardMob(GetRaceNum());
#endif

#if defined(__PARTY_KILL_RENEWAL__)
		if (pkAttacker->GetParty())
		{

// Before
				if (g1->UnderWar(g2->GetID()))
					isUnderGuildWar = true;

			pkKiller->SetQuestNPCID(GetVID());
			quest::CQuestManager::instance().Kill(pkKiller->GetPlayerID(), quest::QUEST_NO_NPC);
			CGuildManager::instance().Kill(pkKiller, this);

		}
	}

	if (IsPC())
	{

// After
				if (g1->UnderWar(g2->GetID()))
					isUnderGuildWar = true;

			pkKiller->SetQuestNPCID(GetVID());
			quest::CQuestManager::instance().Kill(pkKiller->GetPlayerID(), quest::QUEST_NO_NPC);
			CGuildManager::instance().Kill(pkKiller, this);

#if defined(ENABLE_MONSTER_CARD)
			if (pkKiller->GetMonsterCardSystem() != nullptr && IsMonstercardTarget())
				pkKiller->GetMonsterCardSystem()->OnKillMonsterCardMob(GetRaceNum());
#endif
		}
	}

	if (IsPC())
	{

// In `bool CHARACTER::Damage(LPCHARACTER pAttacker, int dam, EDamageType type /*= DAMAGE_TYPE_NORMAL*/)`, find this block:
else if (CGuildDragonLairManager::Instance().IsKing(GetRaceNum()))
		{
			SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
			return false;
		}

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	if (IsMonstercardTarget() && pAttacker != nullptr && pAttacker->GetMonsterCardSystem() != nullptr)
		dam = static_cast<int>(dam * pAttacker->GetMonsterCardSystem()->GetAdditonalMonsterDamagePercent(GetRaceNum()));
#endif
