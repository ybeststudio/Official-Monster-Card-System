// Add to includes:
#if defined(ENABLE_MONSTER_CARD)
#	include "MonstercardSystem.h"
#endif

// Find this line:
m_stName = "";

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	m_isMonsterCardTarget = false;
	m_pMonsterCardSystem.reset();
#endif

// In `void CHARACTER::Destroy()`, find this block:
{
	CloseMyShop();

#if defined(ENABLE_MONSTER_CARD)
	if (m_pMonsterCardSystem)
		m_pMonsterCardSystem->CycleSerialize();
#endif

	if (m_pkRegen)
	{
		if (m_pkDungeon)
		{
			// Dungeon regen may not be valid at this point

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	if (m_pMonsterCardSystem)
		m_pMonsterCardSystem->CycleSerialize();
#endif

// Find this line:
SetRace(t->job);

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	m_isMonsterCardTarget = false;
	m_pMonsterCardSystem.reset(new kalisto::MonstercardSystem(this));
#endif

// In `void CHARACTER::SetPlayerProto(const TPlayerTable* t)`, find this block:
ComputePoints();

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	if (m_pMonsterCardSystem)
		m_pMonsterCardSystem->ApplyLoadedAchievBonuses();
#endif

// Before
	if (m_pkMobInst)
		M2_DELETE(m_pkMobInst);

	m_pkMobData = pkMob;
	m_pkMobInst = M2_NEW CMobInstance;


	m_bPKMode = PK_MODE_FREE;

	const TMobTable* t = &m_pkMobData->m_table;

	m_bCharType = t->bType;

	SetLevel(t->bLevel);
	SetEmpire(t->bEmpire);

	SetExp(t->dwExp);
	SetRealPoint(POINT_ST, t->bStr);
	SetRealPoint(POINT_DX, t->bDex);
	SetRealPoint(POINT_HT, t->bCon);
	SetRealPoint(POINT_IQ, t->bInt);

#if defined(__CONQUEROR_LEVEL__)
	SetConquerorExp(t->dwSungMaExp);
	SetRealPoint(POINT_SUNGMA_STR, t->bSungMaStr);
	SetRealPoint(POINT_SUNGMA_HP, t->bSungMaDex);
	SetRealPoint(POINT_SUNGMA_MOVE, t->bSungMaCon);
	SetRealPoint(POINT_SUNGMA_IMMUNE, t->bSungMaInt);
#endif

	ComputePoints();

	SetHP(GetMaxHP());
	SetSP(GetMaxSP());

	////////////////////
	m_pointsInstant.dwAIFlag = t->dwAIFlag;
	SetImmuneFlag(t->dwImmuneFlag);

	AssignTriggers(t);

	ApplyMobAttribute(t);

	if (IsStone())
	{
		DetermineDropMetinStone();
	}

// After
	if (m_pkMobInst)
		M2_DELETE(m_pkMobInst);

	m_pkMobData = pkMob;
	m_pkMobInst = M2_NEW CMobInstance;

#if defined(ENABLE_MONSTER_CARD)
	m_isMonsterCardTarget = kalisto::MonstercardSystem::IsMonsterCardRace(static_cast<std::size_t>(pkMob->m_table.dwVnum));
#endif

	m_bPKMode = PK_MODE_FREE;

	const TMobTable* t = &m_pkMobData->m_table;

	m_bCharType = t->bType;

	SetLevel(t->bLevel);
	SetEmpire(t->bEmpire);

	SetExp(t->dwExp);
	SetRealPoint(POINT_ST, t->bStr);
	SetRealPoint(POINT_DX, t->bDex);
	SetRealPoint(POINT_HT, t->bCon);
	SetRealPoint(POINT_IQ, t->bInt);

#if defined(__CONQUEROR_LEVEL__)
	SetConquerorExp(t->dwSungMaExp);
	SetRealPoint(POINT_SUNGMA_STR, t->bSungMaStr);
	SetRealPoint(POINT_SUNGMA_HP, t->bSungMaDex);
	SetRealPoint(POINT_SUNGMA_MOVE, t->bSungMaCon);
	SetRealPoint(POINT_SUNGMA_IMMUNE, t->bSungMaInt);
#endif

	ComputePoints();

	SetHP(GetMaxHP());
	SetSP(GetMaxSP());

	////////////////////
	m_pointsInstant.dwAIFlag = t->dwAIFlag;
	SetImmuneFlag(t->dwImmuneFlag);

	AssignTriggers(t);

	ApplyMobAttribute(t);

	if (IsStone())
	{
		DetermineDropMetinStone();
	}
