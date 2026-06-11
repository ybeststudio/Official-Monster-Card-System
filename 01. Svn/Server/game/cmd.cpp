// Find this line:
ACMD(do_auto_restart);

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
ACMD(do_monstercard);
ACMD(do_cardmonster_1_full);
ACMD(do_cardmonster_2_full);
ACMD(do_cardmonster_3_full);
ACMD(do_cardmonster_reset);
#endif

// Find this line:
{ "war", do_war, 0, POS_DEAD, GM_PLAYER },

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	// Avoid prefix collisions with GM spawn commands (e.g. "/mon", "/mob", "/monster").
	{ "cardmonster", do_monstercard, 0, POS_DEAD, GM_PLAYER },
	{ "cardmonster_1_full", do_cardmonster_1_full, 0, POS_DEAD, GM_HIGH_WIZARD },
	{ "cardmonster_2_full", do_cardmonster_2_full, 0, POS_DEAD, GM_HIGH_WIZARD },
	{ "cardmonster_3_full", do_cardmonster_3_full, 0, POS_DEAD, GM_HIGH_WIZARD },
	{ "cardmonster_reset", do_cardmonster_reset, 0, POS_DEAD, GM_HIGH_WIZARD },
#endif
