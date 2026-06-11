#pragma once

#if defined(ENABLE_MONSTER_CARD)

#include <array>
#include <chrono>
#include <map>
#include <set>
#include <random>
#include <string>
#include <vector>

class CHARACTER;
class CItem;

namespace kalisto {

class MonstercardSystem final
{
public:
	static constexpr std::size_t s_MONSTERCARD_VNUM = 50283;
	static constexpr std::size_t s_MONSTERCARD_TRADEABLE_VNUM = 50284;
	static constexpr std::size_t s_MONSTERCARD_USE_ITEM_VNUM_A = 50283;
	static constexpr std::size_t s_MONSTERCARD_USE_ITEM_VNUM_B = 50284;
	static constexpr std::size_t s_MISSION_RESET_CONSUMABLE_VNUM = 72322;
	static constexpr std::size_t s_MISSION_SHUFFLE_CONSUMABLE_VNUM = 72323;
	static constexpr std::size_t s_MAIN_MONSTER_CARD_COUNT = 3;
	static constexpr std::size_t s_MONSTER_CARD_MAX_COUNT = 16;
	static constexpr std::size_t s_DIFFERENT_MONSTERS_PER_STAGE = 20;
	static constexpr std::size_t s_COMBINED_DATA_COUNT = 22;
	static constexpr std::size_t s_NEEDED_CARDCOUNT_TRADE = 5;
	static constexpr std::size_t s_MAX_LEVEL = 18;
	// Official (wiki): 15 mission levels, then wraps back to 1 automatically.
	static constexpr std::size_t s_WIKI_MISSION_LEVEL_MAX = 15;
	static constexpr std::size_t s_NORMAL_MONSTERCARD_DROPP_PERCENT = 5;
	static constexpr std::size_t s_STAGE_MAX = 6;
	// Illustration star classes: 0..5 (client uses STAR_COUNT=5).
	static constexpr int s_STAR_COUNT = 5;
	static constexpr std::size_t s_MONSTERWARP_INFO_COUNT = 172;
	static constexpr std::size_t s_ITEM_MONSTERCARD_SOCKET_INDEX = 0;
	// Polymorph marble socket(1): from monster card UI -> ItemProcess_Polymorph skips normal marble POINT_ATT_BONUS.
	static constexpr long s_POLY_FROM_MONSTER_CARD_MARKER_SOCKET1 = static_cast<long>(0x4D432021u);

	struct AchievDef
	{
		std::size_t vnum;
		int type;
		int rankCount;
		std::array<std::size_t, 8> monsters;
		std::array<int, 5> rankBonus;
		std::array<int, 2> applyType;
	};

private:
	struct MonsterCardInfo;

	using CardTableT = std::array<std::array<std::size_t, s_DIFFERENT_MONSTERS_PER_STAGE>, s_MAX_LEVEL>;
	using TimePointT = std::chrono::high_resolution_clock::time_point;
	using ClockT = std::chrono::high_resolution_clock;
	using InfoIteratorT = std::map<std::size_t, MonsterCardInfo>::iterator;
	using ConstInfoIteratorT = std::map<std::size_t, MonsterCardInfo>::const_iterator;
	using DurationTypeT = decltype(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::seconds(0)).count());

	// Cumulative card totals for star tiers (see UseMonstercard).
	enum class NeededCardStage : int
	{
		eSTAGE1 = 9,
		eSTAGE2 = 30,
		eSTAGE3 = 60,
		eSTAGE4 = 120,
		eSTAGE5 = 210
	};

	enum class ClientCommand : int
	{
		eSEND_STATE = 2,
		eSEND_CURRENT_MISSION,
		eSEND_NEW_MISSION,
		eSEND_MONSTERCARD,
		eSEND_NEW_MONSTERCARD_ORDER,
		eSEND_RESET_MISSION,
		eSEND_MONSTERCARD_FEATURE,
		eSEND_WARPT_REKUTED_MONSTER,
		eSEND_ACHIEV_APPLY = 10,
		eSEND_ACHIEV_REGIST = 11,
		eSEND_UNKNOWN = -1
	};

	enum class MonsterFeature : int
	{
		eFEATURE_WARP,
		eFEATURE_POLY,
		eFEATURE_DROPPGUI,
		eFEATURE_SPAWN,
		eFEATURE_REKRUTE,
		eFEATURE_TRADE,
		eFEATURE_PROMOTION = 6,
		eFEATURE_UNKNOWN = -1
	};

	enum class WaitTime : long long
	{
		eWAIT_RESET_MISSION = 60 * 60 * 24,
		eWAIT_RESET_ORDER = 60 * 60 * 24,
		eWAIT_FEATURE_WARP = 30 * 60,
		eWAIT_FEATURE_POLY = 60 * 60 * 3,
		eWAIT_FEATURE_SPAWN = 60 * 60 * 12,
		eWAIT_FEATURE_REKRUTE = 60 * 60 * 24
	};

	enum class WaitEvent : int
	{
		eEVENT_FEATURE_WARP = static_cast<int>(MonsterFeature::eFEATURE_WARP),
		eEVENT_FEATURE_POLY = static_cast<int>(MonsterFeature::eFEATURE_POLY),
		eEVENT_FEATURE_SPAWN = static_cast<int>(MonsterFeature::eFEATURE_SPAWN),
		eEVENT_FEATURE_REKRUTE = static_cast<int>(MonsterFeature::eFEATURE_REKRUTE),
		eEVENT_RESET_MISSION,
		eEVENT_RESET_ORDER
	};

	struct MonsterWarpInfo
	{
		std::size_t vnum;
		long mapIndex;
		long x;
		long y;

		inline bool operator<(const MonsterWarpInfo& right)
		{
			return vnum < right.vnum;
		}
	};

	struct WaitResult
	{
		bool hasToWait;
		long long timeToWait;
	};

	struct MonsterCardInfo
	{
		MonsterCardInfo();
		MonsterCardInfo(int collectedCards, int killCount, int needCards, int stage, TimePointT lastTeleport, TimePointT lastPoly, TimePointT lastSpawn, TimePointT lastFight);

		TimePointT m_lastTeleport;
		TimePointT m_lastPoly;
		TimePointT m_lastSpawn;
		TimePointT m_lastFight;
		int m_collectedCards;
		int m_killCount;
		int m_needCards;
		int m_stage;
	};

public:
	explicit MonstercardSystem(CHARACTER* associatedCharacter);
	~MonstercardSystem() noexcept = default;

	MonstercardSystem(const MonstercardSystem&) = delete;
	MonstercardSystem(MonstercardSystem&&) = delete;
	MonstercardSystem& operator=(const MonstercardSystem&) = delete;
	MonstercardSystem& operator=(MonstercardSystem&&) = delete;

	void DispatchClientCommand(const char* arguments);

	// GM debug helpers (for rapid testing)
	void DebugCompleteCurrentMission();
	void DebugIncreaseAllCollectionsStage();
	void DebugResetAll();
	void OnKillMonsterCardMob(std::size_t race);
	void CycleSerialize();
	bool UseMonstercard(CItem* monstercard);
	float GetAdditonalMonsterDamagePercent(std::size_t vnum) const;
	// Achiev bonuses must be applied AFTER character points are computed,
	// otherwise ComputePoints() can overwrite them (and some applies early-exit when max HP/SP is 0).
	void ApplyLoadedAchievBonuses();

	static bool IsMonsterCardRace(std::size_t race);
	static const CardTableT& GetMonsterCardList();
	static std::size_t RollMonsterCardUseItemRace();

private:
	void LoadMonstercardMissionTable();
	void LoadMonstercardInfoTable();
	void LoadMonstercardAchievTable();
	static bool LoadAchievDefsOnce();
	static const AchievDef* GetAchievDef(std::size_t achievVnum);
	int GetAchievBonusValue(const AchievDef& def, int registRank) const;
	bool IsAchievUnlocked(const AchievDef& def) const;
	void ApplyAchievBonus(std::size_t achievVnum, bool add);
	void DroppMonstercard(std::size_t mobVnum) const;
	bool ValidateFeatureWaitTime(MonsterFeature feature, std::size_t mobVnum) const;
	void DispatchFeature(MonsterFeature feature, std::size_t mobVnum, const InfoIteratorT& mobIter, const std::string& victimName);
	void SendStateToClient();
	void ReceiveMonsterCardMission();
	void GiveMonsterCardMission();
	void CreateRandomMonsterCard();
	void ResetMonsterCardMission();
	void ResetMonsterCardOrder();
	void InsertMission();
	void OnWarpToRekutedMonster() const;
	void InsertMonsterStatus(std::size_t race, const MonsterCardInfo& info);
	std::size_t PickRandomMonsterCardRaceFromTable();
	void OnUseMonsterCardFeature(const std::vector<std::string>& clientArguments);
	void UseFeaturePoly(std::size_t mobVnum, const InfoIteratorT& mobIter);
	void UseFeatureWarp(std::size_t mobVnum, const InfoIteratorT& mobIter);
	void UseFeatureRekrute(std::size_t mobVnum, const InfoIteratorT& mobIter, const std::string& victimName);
	void UseFeatureTrade(std::size_t mobVnum, const InfoIteratorT& mobIter);
	void UseFeatureDroppGui(std::size_t mobVnum, const InfoIteratorT& mobIter);
	void UseFeatureSpawn(std::size_t mobVnum, const InfoIteratorT& mobIter);
	void UseFeaturePromotion(std::size_t mobVnum, const InfoIteratorT& mobIter);
	WaitResult WaitTimeIsOver(WaitEvent event, std::size_t mobVnum = 0) const;
	InfoIteratorT GetOrAddMonstercardInfo(std::size_t race);
	void OnAchievApply(const std::vector<std::string>& clientArguments);
	void OnAchievRegist(const std::vector<std::string>& clientArguments);
	void RefreshMissionResetWindow();
	static int GetMissionResetItemCostForNext(int nextResetIndexInWindow);

private:
	static const std::array<MonsterWarpInfo, s_MONSTERWARP_INFO_COUNT> s_cMonsterWarpInfos;
	static const CardTableT s_cMonsterVnums;

	const std::array<std::size_t, s_DIFFERENT_MONSTERS_PER_STAGE>& SetNextMonsterCardLevel();

	union
	{
		struct
		{
			std::array<std::size_t, s_MAIN_MONSTER_CARD_COUNT> m_mainMonstercards;
			std::array<std::size_t, s_MONSTER_CARD_MAX_COUNT> m_actualCards;
			std::array<std::size_t, s_MAIN_MONSTER_CARD_COUNT> m_killedMonsters;
		};
		std::array<std::size_t, s_COMBINED_DATA_COUNT> m_cardData;
	};

	std::random_device m_rDevice;
	std::mt19937 m_twister;
	std::map<std::size_t, MonsterCardInfo> m_monsterInfos;
	std::vector<char> m_szQueryBuffer;
	CHARACTER* m_associatedCharacter = nullptr;
	TimePointT m_lastOrderReset;
	TimePointT m_lastMissionReset;
	TimePointT m_lastDroppGui;
	long long m_missionResetWindowStartSec = 0;
	int m_missionResetsInWindow = 0;
	std::size_t m_attackerMob = 0;
	std::size_t m_accountID = 0;
	bool m_needUpdate = false;
	int m_killCount = 0;
	int m_level = 0;

	std::set<std::size_t> m_achievApplied;
	std::map<std::size_t, int> m_achievRegistRank;
	bool m_achievBonusesAppliedOnce = false;
};

} // namespace kalisto

#endif // ENABLE_MONSTER_CARD

