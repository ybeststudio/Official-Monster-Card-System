// Add to includes:
#if defined(ENABLE_MONSTER_CARD)
#	include "MonstercardSystem.h"
#endif

// Find this line:
ch->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You need %s.", LC_ITEM(ITEM_MANAGER::instance().GetTable(dwFood)->dwVnum)));

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
ACMD(do_monstercard)
{
	const auto& sys = ch->GetMonsterCardSystem();
	if (sys == nullptr)
	{
		sys_err("FATAL! MonstercardSystem from %s was nullptr", ch->GetName());
		return;
	}
	sys->DispatchClientCommand(argument);
}

ACMD(do_cardmonster_1_full)
{
	const auto& sys = ch->GetMonsterCardSystem();
	if (sys == nullptr)
		return;
	sys->DebugCompleteCurrentMission();
}

ACMD(do_cardmonster_2_full)
{
	const auto& sys = ch->GetMonsterCardSystem();
	if (sys == nullptr)
		return;
	// NOTE: Server-side does not distinguish solo vs party collections; this boosts all known monster cards.
	sys->DebugIncreaseAllCollectionsStage();
}

ACMD(do_cardmonster_3_full)
{
	const auto& sys = ch->GetMonsterCardSystem();
	if (sys == nullptr)
		return;
	// NOTE: Server-side does not distinguish solo vs party collections; this boosts all known monster cards.
	sys->DebugIncreaseAllCollectionsStage();
}

ACMD(do_cardmonster_reset)
{
	const auto& sys = ch->GetMonsterCardSystem();
	if (sys == nullptr)
		return;
	sys->DebugResetAll();
}
#endif
