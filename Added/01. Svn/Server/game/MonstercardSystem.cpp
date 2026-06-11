#include "stdafx.h"

#if defined(ENABLE_MONSTER_CARD)

#if defined(_DEBUG) && defined(_MSC_VER)
#	pragma strict_gs_check(on)
#endif

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>
#include <numeric>
#include <random>
#include <set>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/join.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"

#include "MonstercardSystem.h"

#include "char.h"
#include "char_manager.h"
#include "config.h"
#include "db.h"
#include "item.h"
#include "item_manager.h"
#include "locale_service.h"
#include "mob_manager.h"
#include "sectree_manager.h"
#include "typedef.h"
#include "utils.h"

#include "../common/FileUtils.h"
#include "../common/MacroUtils.h"

const std::array<kalisto::MonstercardSystem::MonsterWarpInfo, kalisto::MonstercardSystem::s_MONSTERWARP_INFO_COUNT> kalisto::MonstercardSystem::s_cMonsterWarpInfos = {{
#	include "MonstercardCoordinates.txt"
}};

const kalisto::MonstercardSystem::CardTableT kalisto::MonstercardSystem::s_cMonsterVnums = {
#	include "MonstercardVnums.txt"
};

static bool g_mcAchievLoaded = false;
static std::map<std::size_t, kalisto::MonstercardSystem::AchievDef> g_mcAchievDefs;

namespace {

const std::vector<std::size_t>& GetMonsterCardUseItemRacePool()
{
	static std::vector<std::size_t> pool;
	static bool ready = false;
	if (!ready)
	{
		std::set<std::size_t> uniq;
		const auto& t = kalisto::MonstercardSystem::GetMonsterCardList();
		for (const auto& row : t)
		{
			for (const auto v : row)
			{
				if (v != 0)
					uniq.insert(v);
			}
		}
		pool.assign(uniq.begin(), uniq.end());
		ready = true;
	}
	return pool;
}

} // namespace

kalisto::MonstercardSystem::MonsterCardInfo::MonsterCardInfo()
	: m_lastTeleport(std::chrono::seconds { -static_cast<DurationTypeT>(WaitTime::eWAIT_FEATURE_WARP) })
	, m_lastPoly(std::chrono::seconds { -static_cast<DurationTypeT>(WaitTime::eWAIT_FEATURE_POLY) })
	, m_lastSpawn(std::chrono::seconds { -static_cast<DurationTypeT>(WaitTime::eWAIT_FEATURE_SPAWN) })
	, m_lastFight(std::chrono::seconds { -static_cast<DurationTypeT>(WaitTime::eWAIT_FEATURE_REKRUTE) })
	, m_collectedCards(0)
	, m_killCount(0)
	, m_needCards(0)
	// Internal stage is stored as 1..6 (legacy DB format). Client-visible star class is (stage-1) = 0..5.
	, m_stage(1)
{
}

kalisto::MonstercardSystem::MonsterCardInfo::MonsterCardInfo(int collectedCards, int killCount, int needCards, int stage, TimePointT lastTeleport, TimePointT lastPoly, TimePointT lastSpawn, TimePointT lastFight)
	: m_lastTeleport(lastTeleport)
	, m_lastPoly(lastPoly)
	, m_lastSpawn(lastSpawn)
	, m_lastFight(lastFight)
	, m_collectedCards(collectedCards)
	, m_killCount(killCount)
	, m_needCards(needCards)
	, m_stage(stage)
{
}

kalisto::MonstercardSystem::MonstercardSystem(CHARACTER* associatedCharacter)
	: m_twister(m_rDevice())
	, m_szQueryBuffer(QUERY_MAX_LEN)
	, m_associatedCharacter(associatedCharacter)
	, m_lastOrderReset(-std::chrono::seconds { static_cast<DurationTypeT>(WaitTime::eWAIT_RESET_ORDER) })
	, m_lastMissionReset(-std::chrono::seconds { static_cast<DurationTypeT>(WaitTime::eWAIT_RESET_MISSION) })
	, m_lastDroppGui(ClockT::now())
	, m_needUpdate(true)
{
	if (m_associatedCharacter == nullptr)
	{
		sys_err("MonsterCardSystem associatedCharacter was nullptr!!!!!!");
		return;
	}

	m_accountID = associatedCharacter->GetAID();
	std::memset(m_mainMonstercards.data(), 0, sizeof(std::size_t) * m_mainMonstercards.size());
	std::memset(m_killedMonsters.data(), 0, sizeof(std::size_t) * m_killedMonsters.size());
	std::memset(m_actualCards.data(), 0, sizeof(std::size_t) * m_actualCards.size());
	LoadMonstercardMissionTable();
	RefreshMissionResetWindow();
	LoadMonstercardInfoTable();
	LoadAchievDefsOnce();
	LoadMonstercardAchievTable();

	// Enforce "max 1 field bonus applied" rule on load (keep smallest vnum deterministically)
	{
		std::size_t keptField = 0;
		for (const auto& v : m_achievApplied)
		{
			const auto def = GetAchievDef(v);
			if (def == nullptr)
				continue;
			if (def->type != 0)
				continue;
			if (keptField == 0 || v < keptField)
				keptField = v;
		}
		if (keptField != 0)
		{
			for (auto it = m_achievApplied.begin(); it != m_achievApplied.end();)
			{
				const auto def = GetAchievDef(*it);
				if (def != nullptr && def->type == 0 && *it != keptField)
					it = m_achievApplied.erase(it);
				else
					++it;
			}
		}
	}

	// Apply previously applied achievements to character points
	// NOTE: Do NOT apply here. CHARACTER::ComputePoints() runs after this constructor during login/load
	// and will overwrite most points. Call ApplyLoadedAchievBonuses() after ComputePoints().
}

void kalisto::MonstercardSystem::ApplyLoadedAchievBonuses()
{
	// ComputePoints() can overwrite points at login / on some recompute paths.
	// We apply after ComputePoints; on the first call, just add. On subsequent calls, reapply.
	if (m_achievBonusesAppliedOnce)
	{
		for (const auto& v : m_achievApplied)
			ApplyAchievBonus(v, false);
	}
	for (const auto& v : m_achievApplied)
		ApplyAchievBonus(v, true);
	m_achievBonusesAppliedOnce = true;
}

static inline std::string TrimCopy(const std::string& s)
{
	std::string out(s);
	boost::algorithm::trim(out);
	return out;
}

bool kalisto::MonstercardSystem::LoadAchievDefsOnce()
{
	if (g_mcAchievLoaded)
		return !g_mcAchievDefs.empty();

	auto parentDir = [](const std::string& p) -> std::string {
		if (p.empty())
			return std::string();
		std::size_t slash = p.find_last_of("/\\");
		if (slash == std::string::npos)
			return std::string();
		return p.substr(0, slash);
	};

	// Try common candidate paths; some deployments use "locale/common" layout.
	std::vector<std::string> candidates;
	// Official server-side layout: <LocaleService_GetBasePath()>/monster_card_achiev.txt
	// Example: /home/files/main/srv1/share/locale/uk/monster_card_achiev.txt
	{
		const std::string base = LocaleService_GetBasePath();
		if (!base.empty())
		{
			candidates.push_back(base + "/monster_card_achiev.txt");
			// Some shards keep the file under ".../locale/common/" while base points to ".../locale/<lang>".
			candidates.push_back(base + "/common/monster_card_achiev.txt");
			const std::string parent = parentDir(base);
			if (!parent.empty())
				candidates.push_back(parent + "/common/monster_card_achiev.txt");
		}
	}
	// Fallbacks for older layouts
	candidates.push_back("locale/locale/common/monster_card_achiev.txt");
	candidates.push_back("locale/common/monster_card_achiev.txt");
	candidates.push_back("locale/locale/tr/monster_card_achiev.txt");
	candidates.push_back("locale/tr/monster_card_achiev.txt");
	candidates.push_back("locale/monster_card_achiev.txt");
	candidates.push_back("monster_card_achiev.txt");

	std::vector<char> raw;
	for (const auto& p : candidates)
	{
		raw = kalisto::utils::FileReadAllLines(p.c_str());
		if (!raw.empty())
			break;
	}
	if (raw.empty())
	{
		std::string tried;
		for (const auto& p : candidates)
		{
			if (!tried.empty())
				tried += " | ";
			tried += p;
		}
		sys_err("MonsterCard: cannot read achiev defs file (base=%s) (tried: %s)",
			LocaleService_GetBasePath().c_str(), tried.c_str());
		return false;
	}

	std::string text(raw.begin(), raw.end());
	std::vector<std::string> lines;
	boost::split(lines, text, boost::is_any_of("\n"));

	AchievDef cur {};
	bool inInfo = false;
	bool inSub = false;
	std::string curGroup;
	int monIdx = 0;
	int bonusIdx = 0;
	int applyIdx = 0;

	auto flush = [&]() {
		if (cur.vnum != 0)
			g_mcAchievDefs.emplace(cur.vnum, cur);
		cur = AchievDef {};
		cur.vnum = 0;
		cur.type = 0;
		cur.rankCount = 0;
		cur.monsters.fill(0);
		cur.rankBonus.fill(0);
		cur.applyType.fill(0);
		monIdx = bonusIdx = applyIdx = 0;
		curGroup.clear();
		inInfo = false;
		inSub = false;
	};

	for (std::size_t li = 0; li < lines.size(); ++li)
	{
		std::string line = TrimCopy(lines[li]);
		if (line.empty())
			continue;
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		if (line.empty())
			continue;
		if (line[0] == '#')
			continue;

		if (!inInfo)
		{
			if (line.find("Group") == 0 && line.find("info") != std::string::npos)
			{
				cur = AchievDef {};
				cur.vnum = 0;
				cur.type = 0;
				cur.rankCount = 0;
				cur.monsters.fill(0);
				cur.rankBonus.fill(0);
				cur.applyType.fill(0);
				monIdx = bonusIdx = applyIdx = 0;
				curGroup.clear();
				inInfo = true;
				inSub = false;
			}
			continue;
		}

		// Enter subgroup block. Must be checked before skipping "{" lines globally.
		if (!curGroup.empty() && !inSub && line == "{")
		{
			inSub = true;
			continue;
		}

		// Ignore braces that don't start a subgroup block.
		if (line == "{")
			continue;
		if (line == "}")
		{
			if (inSub)
			{
				inSub = false;
				curGroup.clear();
				continue;
			}
			flush();
			continue;
		}

		if (line.find("Group") == 0)
		{
			// "Group monster" / "Group rank_bonus" / "Group apply_type"
			std::vector<std::string> toks;
			boost::split(toks, line, boost::is_any_of("\t "));
			if (toks.size() >= 2)
				curGroup = toks[1];
			continue;
		}

		if (inSub && !curGroup.empty())
		{
			std::vector<std::string> toks;
			boost::split(toks, line, boost::is_any_of("\t "));
			std::vector<std::string> vt;
			for (const auto& t : toks)
				if (!t.empty())
					vt.push_back(t);
			if (vt.size() >= 2)
			{
				if (curGroup == "monster" && monIdx < static_cast<int>(cur.monsters.size()))
					cur.monsters[monIdx++] = static_cast<std::size_t>(std::atoi(vt[1].c_str()));
				else if (curGroup == "rank_bonus" && bonusIdx < static_cast<int>(cur.rankBonus.size()))
					cur.rankBonus[bonusIdx++] = std::atoi(vt[1].c_str());
				else if (curGroup == "apply_type" && applyIdx < static_cast<int>(cur.applyType.size()))
					cur.applyType[applyIdx++] = std::atoi(vt[1].c_str());
			}
			continue;
		}

		// info tokens: "achiev_vnum 1"
		std::vector<std::string> toks;
		boost::split(toks, line, boost::is_any_of("\t "));
		std::vector<std::string> vt;
		for (const auto& t : toks)
			if (!t.empty())
				vt.push_back(t);
		if (vt.size() >= 2)
		{
			if (vt[0] == "achiev_vnum")
				cur.vnum = static_cast<std::size_t>(std::atoi(vt[1].c_str()));
			else if (vt[0] == "achiev_type")
				cur.type = std::atoi(vt[1].c_str());
			else if (vt[0] == "achiev_rank_count")
				cur.rankCount = std::atoi(vt[1].c_str());
		}
	}

	// If file didn't end with a flush, flush now.
	if (inInfo)
		flush();

	g_mcAchievLoaded = true;
	return !g_mcAchievDefs.empty();
}

const kalisto::MonstercardSystem::AchievDef* kalisto::MonstercardSystem::GetAchievDef(std::size_t achievVnum)
{
	// Lazy-load defs on first use. Some shards may not have loaded them yet.
	if (!g_mcAchievLoaded || g_mcAchievDefs.empty())
		LoadAchievDefsOnce();

	const auto it = g_mcAchievDefs.find(achievVnum);
	if (it == g_mcAchievDefs.end())
		return nullptr;
	return &it->second;
}

int kalisto::MonstercardSystem::GetAchievBonusValue(const AchievDef& def, int registRank) const
{
	if (registRank <= 0)
		return 0;
	int startRank = 1;
	if (def.rankCount > 0 && def.rankCount < 5)
		startRank = 6 - def.rankCount;
	if (registRank < startRank || registRank > 5)
		return 0;
	// File format keeps 5 slots (1..5). rank_count only controls which ranks are valid (from 5 backwards),
	// so index by the actual rank (rank-1), not by (rank-startRank).
	const int idx = registRank - 1;
	if (idx < 0 || idx >= static_cast<int>(def.rankBonus.size()))
		return 0;
	return def.rankBonus[idx];
}

bool kalisto::MonstercardSystem::IsAchievUnlocked(const AchievDef& def) const
{
	// Official behavior: set bonuses are claimed by consuming stars (stage) on required monsters.
	// So require at least stage 1 (one star) for every listed monster.
	for (std::size_t mv : def.monsters)
	{
		if (mv == 0)
			continue;
		const auto it = m_monsterInfos.find(mv);
		if (it == m_monsterInfos.end())
			return false;
		// Client-visible stars = (stage-1). So require internal stage >= 2 (at least 1 star).
		if (it->second.m_stage < 2)
			return false;
	}
	return true;
}

void kalisto::MonstercardSystem::ApplyAchievBonus(std::size_t achievVnum, bool add)
{
	const AchievDef* def = GetAchievDef(achievVnum);
	if (def == nullptr)
		return;

	const auto rankIt = m_achievRegistRank.find(achievVnum);
	if (rankIt == m_achievRegistRank.end() || rankIt->second <= 0)
		return;

	const int bonus = GetAchievBonusValue(*def, rankIt->second);
	if (bonus == 0)
		return;

	for (int ap : def->applyType)
	{
		if (ap <= 0)
			continue;
		// Apply types are APPLY_* ids; ApplyPoint internally calls PointChange and updates packets.
		m_associatedCharacter->ApplyPoint(static_cast<POINT_TYPE>(ap), add ? bonus : -bonus);
	}
}

int kalisto::MonstercardSystem::GetMissionResetItemCostForNext(int nextResetIndexInWindow)
{
	// Official (wiki): 1 free reset per 24h window, then 1 "Yeni Başlangıç Kartı" per reset.
	if (nextResetIndexInWindow <= 1)
		return 0;
	return 1;
}

void kalisto::MonstercardSystem::RefreshMissionResetWindow()
{
	if (m_missionResetWindowStartSec == 0LL)
		return;

	const auto nowSec = std::chrono::duration_cast<std::chrono::seconds>(ClockT::now().time_since_epoch()).count();
	if (nowSec - m_missionResetWindowStartSec >= static_cast<decltype(nowSec)>(86400))
	{
		m_missionResetWindowStartSec = 0LL;
		m_missionResetsInWindow = 0;
		m_needUpdate = true;
	}
}

void kalisto::MonstercardSystem::LoadMonstercardAchievTable()
{
	static constexpr const char* ACHIEV_QUERY = "SELECT achiev_vnum, applied, regist_rank FROM player.monstercard_achiev WHERE account_id = %u";
	::snprintf(m_szQueryBuffer.data(), m_szQueryBuffer.size(), ACHIEV_QUERY, m_accountID);

	std::unique_ptr<SQLMsg> sql(DBManager::instance().DirectQuery(m_szQueryBuffer.data(), m_accountID));
	if (sql == nullptr)
		return;
	if (sql->Get()->uiNumRows == 0)
		return;

	while (MYSQL_ROW row = mysql_fetch_row(sql->Get()->pSQLResult))
	{
		if (row[0] == nullptr)
			continue;

		std::size_t vnum = 0;
		int applied = 0;
		int rank = 0;
		try
		{
			vnum = boost::lexical_cast<std::size_t>(row[0]);
			if (row[1] != nullptr)
				applied = boost::lexical_cast<int>(row[1]);
			if (row[2] != nullptr)
				rank = boost::lexical_cast<int>(row[2]);
		}
		catch (const boost::bad_lexical_cast&)
		{
			continue;
		}

		if (applied != 0)
			m_achievApplied.insert(vnum);
		if (rank > 0)
			m_achievRegistRank[vnum] = rank;
	}
}

void kalisto::MonstercardSystem::LoadMonstercardMissionTable()
{
	static constexpr const char* CARDQUERY = "SELECT * FROM player.monstercard_mission WHERE account_id = %u";
	enum
	{
		MAIN_CARD_INDEX_FIRST = 1,
		MAIN_CARD_INDEX_SECOND = 2,
		MAIN_CARD_INDEX_THIRD = 3,
		KILLED_INDEX_FIRST = 20,
		KILLED_INDEX_SECOND = 21,
		KILLED_INDEX_THIRD = 22,
		LAST_ORDER_RESET = 23,
		LAST_MISSION_RESET = 24,
		LEVEL = 25,
		ACTUCAL_CARD_START = 4,
		ACTUAL_CARD_END = 20
	};

	::snprintf(m_szQueryBuffer.data(), m_szQueryBuffer.size(), CARDQUERY, m_accountID);
	std::unique_ptr<SQLMsg> cardSqlResult(DBManager::instance().DirectQuery(m_szQueryBuffer.data(), m_accountID));
	if (cardSqlResult == nullptr)
		return;

	if (cardSqlResult->Get()->uiNumRows != 0)
	{
		MYSQL_RES* const sqlRes = cardSqlResult->Get()->pSQLResult;
		MYSQL_ROW row = mysql_fetch_row(sqlRes);
		m_mainMonstercards[0] = boost::lexical_cast<std::size_t>(row[MAIN_CARD_INDEX_FIRST]);
		m_mainMonstercards[1] = boost::lexical_cast<std::size_t>(row[MAIN_CARD_INDEX_SECOND]);
		m_mainMonstercards[2] = boost::lexical_cast<std::size_t>(row[MAIN_CARD_INDEX_THIRD]);
		for (std::size_t card = ACTUCAL_CARD_START; card < ACTUAL_CARD_END; ++card)
			m_actualCards[card - ACTUCAL_CARD_START] = boost::lexical_cast<std::size_t>(row[card]);

		m_killedMonsters[0] = boost::lexical_cast<std::size_t>(row[KILLED_INDEX_FIRST]);
		m_killedMonsters[1] = boost::lexical_cast<std::size_t>(row[KILLED_INDEX_SECOND]);
		m_killedMonsters[2] = boost::lexical_cast<std::size_t>(row[KILLED_INDEX_THIRD]);
		m_lastOrderReset = TimePointT { std::chrono::seconds { boost::lexical_cast<DurationTypeT>(row[LAST_ORDER_RESET]) } };
		m_lastMissionReset = TimePointT { std::chrono::seconds { boost::lexical_cast<DurationTypeT>(row[LAST_MISSION_RESET]) } };
		m_level = boost::lexical_cast<int>(row[LEVEL]);
		m_killCount = std::accumulate(m_killedMonsters.cbegin(), m_killedMonsters.cend(), 0);

		const unsigned int fieldCount = mysql_num_fields(sqlRes);
		if (fieldCount >= 28U && row[26] != nullptr && row[27] != nullptr)
		{
			m_missionResetWindowStartSec = std::strtoll(row[26], nullptr, 10);
			m_missionResetsInWindow = static_cast<int>(std::strtol(row[27], nullptr, 10));
		}
		else
		{
			m_missionResetWindowStartSec = 0LL;
			m_missionResetsInWindow = 0;
		}
	}
	else
	{
		InsertMission();
	}
}

void kalisto::MonstercardSystem::LoadMonstercardInfoTable()
{
	static constexpr const char* INFO_QUERY = "SELECT * FROM player.monstercard_status WHERE account_id = %d";
	enum
	{
		VNUM = 1,
		COLLECTED_CARDS,
		KILLCOUNT,
		NEED_CARDS,
		STAGE,
		LAST_TELEPORT,
		LAST_POLY,
		LAST_SPAWN,
		LAST_FIGHT
	};

	std::unique_ptr<SQLMsg> infoSqlResult(DBManager::instance().DirectQuery(INFO_QUERY, m_accountID));
	if (infoSqlResult == nullptr)
		return;

	if (infoSqlResult->Get()->uiNumRows != 0)
	{
		auto* sqlResult = infoSqlResult->Get()->pSQLResult;
		for (MYSQL_ROW row = mysql_fetch_row(sqlResult); row != nullptr; row = mysql_fetch_row(sqlResult))
		{
			m_monsterInfos.emplace(std::make_pair(
				boost::lexical_cast<std::size_t>(row[VNUM]),
				MonsterCardInfo { boost::lexical_cast<int>(row[COLLECTED_CARDS]),
					boost::lexical_cast<int>(row[KILLCOUNT]),
					boost::lexical_cast<int>(row[NEED_CARDS]),
					boost::lexical_cast<int>(row[STAGE]),
					TimePointT { std::chrono::seconds { boost::lexical_cast<DurationTypeT>(row[LAST_TELEPORT]) } },
					TimePointT { std::chrono::seconds { boost::lexical_cast<DurationTypeT>(row[LAST_POLY]) } },
					TimePointT { std::chrono::seconds { boost::lexical_cast<DurationTypeT>(row[LAST_SPAWN]) } },
					TimePointT { std::chrono::seconds { boost::lexical_cast<DurationTypeT>(row[LAST_FIGHT]) } } }));
		}
	}
}

void kalisto::MonstercardSystem::DispatchClientCommand(const char* arguments)
{
	std::string argstr(arguments);
	std::vector<std::string> splittedCommands;
	boost::trim_if(argstr, boost::is_any_of("\t "));
	boost::split(splittedCommands, argstr, boost::is_any_of(" "));
	static constexpr std::size_t COMMANDINDEX = 0;
	ClientCommand cmd = ClientCommand::eSEND_UNKNOWN;
	try
	{
		cmd = static_cast<ClientCommand>(boost::lexical_cast<int>(splittedCommands[COMMANDINDEX]));
	}
	catch (const boost::bad_lexical_cast&)
	{
		sys_err("invalid client command from user %s command was : %s", m_associatedCharacter->GetName(), arguments);
		KALISTO_ASSERT(false, "invalid client command!");
		return;
	}

	switch (cmd)
	{
	case ClientCommand::eSEND_STATE:
		SendStateToClient();
		break;
	case ClientCommand::eSEND_CURRENT_MISSION:
		ReceiveMonsterCardMission();
		break;
	case ClientCommand::eSEND_NEW_MISSION:
		GiveMonsterCardMission();
		break;
	case ClientCommand::eSEND_MONSTERCARD:
		CreateRandomMonsterCard();
		break;
	case ClientCommand::eSEND_NEW_MONSTERCARD_ORDER:
		ResetMonsterCardOrder();
		break;
	case ClientCommand::eSEND_RESET_MISSION:
		ResetMonsterCardMission();
		break;
	case ClientCommand::eSEND_MONSTERCARD_FEATURE:
		OnUseMonsterCardFeature(splittedCommands);
		break;
	case ClientCommand::eSEND_WARPT_REKUTED_MONSTER:
		OnWarpToRekutedMonster();
		break;
	case ClientCommand::eSEND_ACHIEV_APPLY:
		OnAchievApply(splittedCommands);
		break;
	case ClientCommand::eSEND_ACHIEV_REGIST:
		OnAchievRegist(splittedCommands);
		break;
	default:
		sys_err("MonstercardSystem invalid Command Type : %d (%s)", static_cast<int>(cmd), m_associatedCharacter->GetName());
		break;
	}
}

void kalisto::MonstercardSystem::OnAchievApply(const std::vector<std::string>& clientArguments)
{
	// /monstercard 10 <achiev_vnum>
	if (clientArguments.size() < 2)
		return;

	std::size_t vnum = 0;
	try
	{
		vnum = boost::lexical_cast<std::size_t>(clientArguments[1]);
	}
	catch (const boost::bad_lexical_cast&)
	{
		return;
	}

	// Apply requires a registered rank.
	// NOTE: Claiming (regist) consumes stars/cards, so "unlocked" must NOT depend on current stage here.
	const auto def = GetAchievDef(vnum);
	if (def == nullptr)
		return;
	const auto rankIt = m_achievRegistRank.find(vnum);
	if (rankIt == m_achievRegistRank.end() || rankIt->second <= 0)
		return;

	const bool isApplied = (m_achievApplied.find(vnum) != m_achievApplied.end());
	if (isApplied)
	{
		ApplyAchievBonus(vnum, false);
		m_achievApplied.erase(vnum);
	}
	else
	{
		// type==0: field bonus (one active); other types stack (e.g. dungeon).
		if (def->type == 0)
		{
			for (auto it = m_achievApplied.begin(); it != m_achievApplied.end();)
			{
				const auto otherDef = GetAchievDef(*it);
				if (otherDef != nullptr && otherDef->type == 0)
				{
					const std::size_t otherVnum = *it;
					ApplyAchievBonus(otherVnum, false);
					it = m_achievApplied.erase(it);
					m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ACHIEV_APPLY/%d/0", static_cast<int>(otherVnum));
					continue;
				}
				++it;
			}
		}

		m_achievApplied.insert(vnum);
		ApplyAchievBonus(vnum, true);
		m_achievBonusesAppliedOnce = true;
	}

	// Ensure derived battle points and client-visible points are refreshed immediately.
	m_associatedCharacter->ComputeBattlePoints();
	m_associatedCharacter->PointsPacket();

	if (test_server)
	{
		const auto def2 = GetAchievDef(vnum);
		int rank2 = 0;
		try { rank2 = m_achievRegistRank[vnum]; } catch (...) { rank2 = 0; }
		int bonus2 = (def2 != nullptr) ? GetAchievBonusValue(*def2, rank2) : 0;
		m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, "MC AchievApply v=%d now=%d rank=%d bonus=%d",
			static_cast<int>(vnum), (isApplied ? 0 : 1), rank2, bonus2);
		if (def2 == nullptr)
			m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, "MC AchievApply def=null (achiev defs not loaded / path issue)");
		if (def2 != nullptr)
		{
			for (int ap : def2->applyType)
			{
				if (ap <= 0 || ap >= MAX_APPLY_NUM)
					continue;
				const POINT_TYPE pt = aApplyInfo[ap].wPointType;
				m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, " - apply=%d pt=%d val=%d", ap, static_cast<int>(pt), static_cast<int>(m_associatedCharacter->GetPoint(pt)));
			}
		}
	}

	m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ACHIEV_APPLY/%d/%d", static_cast<int>(vnum), isApplied ? 0 : 1);
	m_needUpdate = true;
}

void kalisto::MonstercardSystem::OnAchievRegist(const std::vector<std::string>& clientArguments)
{
	// /monstercard 11 <achiev_vnum> <rank>
	if (clientArguments.size() < 3)
		return;

	std::size_t vnum = 0;
	int rank = 0;
	try
	{
		vnum = boost::lexical_cast<std::size_t>(clientArguments[1]);
		rank = boost::lexical_cast<int>(clientArguments[2]);
	}
	catch (const boost::bad_lexical_cast&)
	{
		return;
	}

	const auto def = GetAchievDef(vnum);
	if (def == nullptr)
		return;
	if (!IsAchievUnlocked(*def))
		return;

	int startRank = 1;
	if (def->rankCount > 0 && def->rankCount < 5)
		startRank = 6 - def->rankCount;
	if (rank < startRank || rank > 5)
		return;

	// If currently applied, remove old bonus then apply new one.
	const bool wasApplied = (m_achievApplied.find(vnum) != m_achievApplied.end());
	if (wasApplied)
		ApplyAchievBonus(vnum, false);

	m_achievRegistRank[vnum] = rank;

	// Official feel: requesting an achievement should immediately activate it.
	// For field-type (type==0): ensure only one is active at a time.
	if (def->type == 0)
	{
		for (auto it = m_achievApplied.begin(); it != m_achievApplied.end();)
		{
			const auto otherDef = GetAchievDef(*it);
			if (otherDef != nullptr && otherDef->type == 0 && *it != vnum)
			{
				const std::size_t otherVnum = *it;
				ApplyAchievBonus(otherVnum, false);
				it = m_achievApplied.erase(it);
				m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ACHIEV_APPLY/%d/0", static_cast<int>(otherVnum));
				continue;
			}
			++it;
		}
	}

	m_achievApplied.insert(vnum);
	ApplyAchievBonus(vnum, true);
	m_achievBonusesAppliedOnce = true;
	m_associatedCharacter->ComputeBattlePoints();
	m_associatedCharacter->PointsPacket();
	m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ACHIEV_APPLY/%d/1", static_cast<int>(vnum));

	if (test_server)
	{
		const int dbgBonus = GetAchievBonusValue(*def, rank);
		m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, "MC AchievRegist v=%d rank=%d bonus=%d", static_cast<int>(vnum), rank, dbgBonus);
		for (int ap : def->applyType)
		{
			if (ap <= 0 || ap >= MAX_APPLY_NUM)
				continue;
			const POINT_TYPE pt = aApplyInfo[ap].wPointType;
			m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, " - apply=%d pt=%d val=%d", ap, static_cast<int>(pt), static_cast<int>(m_associatedCharacter->GetPoint(pt)));
		}
	}

	// Official behavior: claiming (regist) consumes illustration stars of the required monsters.
	// Cards are not refunded; they are consumed and disappear together with the stars.
	for (std::size_t mv : def->monsters)
	{
		if (mv == 0)
			continue;
		auto it = m_monsterInfos.find(mv);
		if (it == m_monsterInfos.end())
			continue;
		if (it->second.m_stage != 1 || it->second.m_collectedCards != 0)
		{
			it->second.m_collectedCards = 0;
			it->second.m_needCards = 0;
			it->second.m_stage = 1;
			const int stageOut = 0;
			m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ADD_MOB_INFO/%d/%d/%d/%d/%d/%lld/%lld/%lld",
				static_cast<int>(mv), it->second.m_collectedCards,
				it->second.m_killCount, it->second.m_needCards, stageOut,
				static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(it->second.m_lastTeleport).time_since_epoch().count()),
				static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(it->second.m_lastPoly).time_since_epoch().count()),
				static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(it->second.m_lastSpawn).time_since_epoch().count()));
		}
	}

	m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ACHIEV_REGIST/%d/%d", static_cast<int>(vnum), rank);
	m_needUpdate = true;
}

void kalisto::MonstercardSystem::ReceiveMonsterCardMission()
{
	if (m_mainMonstercards[0] != 0 && m_mainMonstercards[1] != 0 && m_mainMonstercards[2] != 0)
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM NO_NEW_MISSION");
		return;
	}
	if (m_killCount >= static_cast<int>(s_MAIN_MONSTER_CARD_COUNT))
		return;

	if (std::accumulate(m_actualCards.cbegin(), m_actualCards.cend(), 0) != 0)
	{
		struct ElemenT
		{
			std::size_t vnum, index;
		};
		std::array<ElemenT, s_MONSTER_CARD_MAX_COUNT> possibleCards;
		std::size_t indexCounter = 0;
		std::generate(possibleCards.begin(), possibleCards.end(), [&indexCounter, this] {
			std::size_t card = m_actualCards[indexCounter];
			return ElemenT { card, indexCounter++ };
		});
		std::shuffle(possibleCards.begin(), possibleCards.end(), m_twister);

		std::array<ElemenT, s_MAIN_MONSTER_CARD_COUNT> newMonstercards {};
		indexCounter = 0;
		for (const auto& iter : possibleCards)
		{
			if (iter.vnum != 0)
				newMonstercards[indexCounter++] = iter;

			if (indexCounter == newMonstercards.size())
				break;
		}
		if (indexCounter != newMonstercards.size())
		{
			// Not enough valid candidates; keep state unchanged (avoid UB / crash).
			return;
		}

		indexCounter = 0;
		std::transform(m_mainMonstercards.begin(), m_mainMonstercards.end(), m_mainMonstercards.begin(), [&indexCounter, &newMonstercards](auto&&) { return newMonstercards[indexCounter++].vnum; });

		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM REC_MAINCARDS/%d/%d/%d/%d/%d/%d", m_mainMonstercards[0], m_mainMonstercards[1],
			m_mainMonstercards[2], newMonstercards[0].index, newMonstercards[1].index, newMonstercards[2].index);

		m_actualCards[newMonstercards[0].index] = m_actualCards[newMonstercards[1].index] = m_actualCards[newMonstercards[2].index] = 0;
	}
	else
	{
		GiveMonsterCardMission();
		ReceiveMonsterCardMission();
	}
	m_needUpdate = true;
}

void kalisto::MonstercardSystem::DroppMonstercard(std::size_t mobVnum) const
{
	static constexpr std::size_t MONSTERCARD_MOBVNUM_SOCKET_INDEX = 1;
	static constexpr int OwnerShipTime = 120;
	LPITEM item = ITEM_MANAGER::instance().CreateItem(s_MONSTERCARD_VNUM);
	if (item == nullptr)
		return;

	item->SetSocket(MONSTERCARD_MOBVNUM_SOCKET_INDEX, mobVnum);

	PIXEL_POSITION pos;
	item->SetOwnership(m_associatedCharacter, OwnerShipTime);
	pos.x = m_associatedCharacter->GetX() + number(-200, 200);
	pos.y = m_associatedCharacter->GetY() + number(-200, 200);
	item->AddToGround(m_associatedCharacter->GetMapIndex(), pos);
	item->StartDestroyEvent();
}

void kalisto::MonstercardSystem::OnKillMonsterCardMob(std::size_t race)
{
	if (number(1, 100) <= s_NORMAL_MONSTERCARD_DROPP_PERCENT)
		DroppMonstercard(race);

	auto iter = std::find(m_mainMonstercards.begin(), m_mainMonstercards.end(), race);
	if (iter == m_mainMonstercards.end())
		return;

	auto index = iter - m_mainMonstercards.begin();
	m_killedMonsters[index] = 1;
	m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM SUCCES_KILL/%d", static_cast<int>(index));

	auto infoIter = GetOrAddMonstercardInfo(race);
	++infoIter->second.m_killCount;

	m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ADD_MOB_INFO/%d/%d/%d/%d/%d/%lld/%lld/%lld",
		static_cast<int>(race), infoIter->second.m_collectedCards,
		infoIter->second.m_killCount, infoIter->second.m_needCards, (infoIter->second.m_stage > 0 ? (infoIter->second.m_stage - 1) : 0),
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(infoIter->second.m_lastTeleport).time_since_epoch().count()),
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(infoIter->second.m_lastPoly).time_since_epoch().count()),
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(infoIter->second.m_lastSpawn).time_since_epoch().count()));

	const CMob* pkMob = CMobManager::instance().Get(static_cast<DWORD>(race));
	const char* mobName = (pkMob != nullptr) ? pkMob->m_table.szLocaleName : "?";
	// Some locale strings may not contain the same format placeholders; preformat into a single string to avoid UB.
	char msgBuf[256];
	// Do not use LC_STRING here: on some shards, locale tables can override raw English keys unexpectedly.
	::snprintf(msgBuf, sizeof(msgBuf), "You have defeated the monster %s at mission level %d.", mobName, m_level);
	m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, "%s", msgBuf);

	if (std::accumulate(m_killedMonsters.cbegin(), m_killedMonsters.cend(), 0) == static_cast<int>(s_MAIN_MONSTER_CARD_COUNT))
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM SUCCES_MISSION");
		::snprintf(msgBuf, sizeof(msgBuf), "You have completed mission level %d. Go claim your Monster Card.", m_level);
		m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, "%s", msgBuf);
	}
	m_needUpdate = true;
}

void kalisto::MonstercardSystem::CycleSerialize()
{
	if (!m_needUpdate)
		return;

	static constexpr const char* MISSION_UPDATE_QUERY =
		"UPDATE player.monstercard_mission SET "
		"main_card0=%d, main_card1=%d, main_card2=%d, card0=%d, card1=%d, card2=%d, card3=%d, card4=%d, card5=%d, card6=%d, card7=%d, card8=%d, card9=%d, "
		"card10=%d, card11=%d, card12=%d, card13=%d, card14=%d, card15=%d, main_card0_killed=%d, main_card1_killed=%d, main_card2_killed=%d, "
		"last_mission_back = %lld, last_order_back = %lld, monstercardsystem_level = %d, mission_reset_window_start = %lld, mission_resets_in_window = %d WHERE account_id = %d";

	auto lastOrderReset = std::chrono::time_point_cast<std::chrono::seconds>(m_lastOrderReset).time_since_epoch().count();
	auto lastMissionReset = std::chrono::time_point_cast<std::chrono::seconds>(m_lastMissionReset).time_since_epoch().count();

	::snprintf(m_szQueryBuffer.data(), m_szQueryBuffer.size(), MISSION_UPDATE_QUERY, static_cast<int>(m_mainMonstercards[0]), static_cast<int>(m_mainMonstercards[1]),
		static_cast<int>(m_mainMonstercards[2]), static_cast<int>(m_actualCards[0]), static_cast<int>(m_actualCards[1]), static_cast<int>(m_actualCards[2]),
		static_cast<int>(m_actualCards[3]), static_cast<int>(m_actualCards[4]), static_cast<int>(m_actualCards[5]), static_cast<int>(m_actualCards[6]),
		static_cast<int>(m_actualCards[7]), static_cast<int>(m_actualCards[8]), static_cast<int>(m_actualCards[9]), static_cast<int>(m_actualCards[10]),
		static_cast<int>(m_actualCards[11]), static_cast<int>(m_actualCards[12]), static_cast<int>(m_actualCards[13]), static_cast<int>(m_actualCards[14]),
		static_cast<int>(m_actualCards[15]), static_cast<int>(m_killedMonsters[0]), static_cast<int>(m_killedMonsters[1]), static_cast<int>(m_killedMonsters[2]),
		static_cast<long long>(lastMissionReset), static_cast<long long>(lastOrderReset), m_level, static_cast<long long>(m_missionResetWindowStartSec),
		m_missionResetsInWindow, static_cast<int>(m_accountID));

	static constexpr const char* INFO_UPDATE_QUERY =
		"UPDATE player.monstercard_status SET vnum = %d, collected_monstercards = %d, "
		"killcount = %d, needcards = %d, stage = %d, last_teleport = %lld, last_poly = %lld, last_spawn = %lld, last_fight = %lld WHERE account_id = %d AND vnum =%d; ";

	static auto& dbmgr = DBManager::instance();
	dbmgr.DirectQuery(m_szQueryBuffer.data());

	for (auto& info : m_monsterInfos)
	{
		::snprintf(m_szQueryBuffer.data(), m_szQueryBuffer.size(), INFO_UPDATE_QUERY, static_cast<int>(info.first), info.second.m_collectedCards, info.second.m_killCount,
			info.second.m_needCards, info.second.m_stage,
			static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(info.second.m_lastTeleport).time_since_epoch().count()),
			static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(info.second.m_lastPoly).time_since_epoch().count()),
			static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(info.second.m_lastSpawn).time_since_epoch().count()),
			static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(info.second.m_lastFight).time_since_epoch().count()),
			static_cast<int>(m_accountID), static_cast<int>(info.first));
		dbmgr.DirectQuery(m_szQueryBuffer.data());
	}

	// Achievements (best-effort; requires player.monstercard_achiev)
	static constexpr const char* ACHIEV_UPSERT =
		"REPLACE INTO player.monstercard_achiev(account_id, achiev_vnum, applied, regist_rank) VALUES(%d, %d, %d, %d)";

	for (const auto& it : m_achievRegistRank)
	{
		const int vnum = static_cast<int>(it.first);
		const int rank = it.second;
		const int applied = (m_achievApplied.find(it.first) != m_achievApplied.end()) ? 1 : 0;
		::snprintf(m_szQueryBuffer.data(), m_szQueryBuffer.size(), ACHIEV_UPSERT, static_cast<int>(m_accountID), vnum, applied, rank);
		dbmgr.DirectQuery(m_szQueryBuffer.data());
	}
	for (const auto& v : m_achievApplied)
	{
		if (m_achievRegistRank.find(v) != m_achievRegistRank.end())
			continue;
		const int vnum = static_cast<int>(v);
		::snprintf(m_szQueryBuffer.data(), m_szQueryBuffer.size(), ACHIEV_UPSERT, static_cast<int>(m_accountID), vnum, 1, 0);
		dbmgr.DirectQuery(m_szQueryBuffer.data());
	}

	m_needUpdate = false;
}

bool kalisto::MonstercardSystem::UseMonstercard(CItem* monstercard)
{
	static constexpr std::size_t MONSTERCARD_MOBVNUM_SOCKET_INDEX = 1;

	const DWORD itemVnum = monstercard->GetVnum();
	std::size_t mobVnum = 0;
	// Tradeable: mob on socket 0 first; mission card 50283: mob on socket 1 first. Same vnums as USE items: check card layout before generic use-item socket order.
	if (itemVnum == static_cast<DWORD>(s_MONSTERCARD_TRADEABLE_VNUM))
	{
		mobVnum = static_cast<std::size_t>(monstercard->GetSocket(static_cast<int>(s_ITEM_MONSTERCARD_SOCKET_INDEX)));
		if (mobVnum == 0)
			mobVnum = static_cast<std::size_t>(monstercard->GetSocket(1));
		if (mobVnum == 0)
			mobVnum = PickRandomMonsterCardRaceFromTable();
	}
	else if (itemVnum == static_cast<DWORD>(s_MONSTERCARD_VNUM))
	{
		mobVnum = static_cast<std::size_t>(monstercard->GetSocket(1));
		if (mobVnum == 0)
			mobVnum = static_cast<std::size_t>(monstercard->GetSocket(0));
		if (mobVnum == 0)
			mobVnum = PickRandomMonsterCardRaceFromTable();
	}
	else if (itemVnum == static_cast<DWORD>(s_MONSTERCARD_USE_ITEM_VNUM_A) || itemVnum == static_cast<DWORD>(s_MONSTERCARD_USE_ITEM_VNUM_B))
	{
		mobVnum = static_cast<std::size_t>(monstercard->GetSocket(0));
		if (mobVnum == 0)
			mobVnum = static_cast<std::size_t>(monstercard->GetSocket(1));
		if (mobVnum == 0)
			mobVnum = PickRandomMonsterCardRaceFromTable();
	}

	if (mobVnum == 0)
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, LC_STRING("MonsterCard: Invalid card (no monster assigned)."));
		return false;
	}
	auto mobIter = GetOrAddMonstercardInfo(mobVnum);
	// Internal stage is 1..6, so max internal is (stars 5) -> 6.
	if (mobIter->second.m_stage >= static_cast<int>(s_STAR_COUNT + 1) || mobIter->second.m_stage < 1)
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, LC_STRING("This monster has already reached the maximum level!"));
		return false;
	}

	// Client-visible class is (stage-1): 0..5
	int needCards = 9;
	switch (mobIter->second.m_stage - 1)
	{
		case 0: needCards = 9; break;
		case 1: needCards = 21; break;
		case 2: needCards = 30; break;
		case 3: needCards = 60; break;
		case 4: needCards = 90; break;
		default: needCards = 120; break;
	}
	mobIter->second.m_needCards = needCards;
	++mobIter->second.m_collectedCards;
	if (mobIter->second.m_collectedCards > needCards)
		mobIter->second.m_collectedCards = needCards;

	const int stageOut = (mobIter->second.m_stage > 0) ? (mobIter->second.m_stage - 1) : 0;
	if (test_server && mobIter->second.m_collectedCards == needCards)
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO,
			"MC Ready mob=%d cur=%d/%d stageOut=%d internalStage=%d",
			static_cast<int>(mobVnum),
			mobIter->second.m_collectedCards,
			needCards,
			stageOut,
			mobIter->second.m_stage);
	}
	m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ADD_MOB_INFO/%d/%d/%d/%d/%d/%lld/%lld/%lld",
		static_cast<int>(mobVnum), mobIter->second.m_collectedCards,
		mobIter->second.m_killCount, mobIter->second.m_needCards, stageOut,
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(mobIter->second.m_lastTeleport).time_since_epoch().count()),
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(mobIter->second.m_lastPoly).time_since_epoch().count()),
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(mobIter->second.m_lastSpawn).time_since_epoch().count()));
	m_needUpdate = true;
	return true;
}

void kalisto::MonstercardSystem::GiveMonsterCardMission()
{
	const std::array<std::size_t, s_DIFFERENT_MONSTERS_PER_STAGE>& cardList = SetNextMonsterCardLevel();
	std::memcpy(m_actualCards.data(), cardList.data(), m_actualCards.size() * sizeof(int));
	std::shuffle(m_actualCards.begin(), m_actualCards.end(), m_twister);
	std::memset(m_mainMonstercards.data(), 0, sizeof(std::size_t) * m_mainMonstercards.size());
	std::string data = "MONSTERCARDSYSTEM NEW_MISSION/" + boost::algorithm::join(m_actualCards | boost::adaptors::transformed([](std::size_t val) { return std::to_string(val); }), "/");
	m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, data.c_str());
	m_needUpdate = true;
}

void kalisto::MonstercardSystem::CreateRandomMonsterCard()
{
	if (std::accumulate(m_killedMonsters.cbegin(), m_killedMonsters.cend(), 0) != static_cast<int>(s_MAIN_MONSTER_CARD_COUNT))
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM NOT_ALL_MONSTERS_KILLED");
		return;
	}

	LPITEM item = m_associatedCharacter->AutoGiveItem(s_MONSTERCARD_VNUM, 1);
	std::size_t mobVnum = m_mainMonstercards[number(0, static_cast<int>(m_mainMonstercards.size() - 1))];
	static constexpr std::size_t MONSTERCARD_MOBVNUM_SOCKET_INDEX = 1;
	item->SetSocket(MONSTERCARD_MOBVNUM_SOCKET_INDEX, mobVnum);

	std::memset(m_killedMonsters.data(), 0, sizeof(std::size_t) * m_killedMonsters.size());
	std::memset(m_mainMonstercards.data(), 0, sizeof(std::size_t) * m_mainMonstercards.size());
	GiveMonsterCardMission();
	m_needUpdate = true;
}

bool kalisto::MonstercardSystem::ValidateFeatureWaitTime(MonsterFeature feature, std::size_t mobVnum) const
{
	WaitResult result = WaitTimeIsOver(static_cast<WaitEvent>(feature), mobVnum);
	if (result.hasToWait)
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM NO_NEW_ORDER/%lld", result.timeToWait);
		return false;
	}
	return true;
}

void kalisto::MonstercardSystem::SendStateToClient()
{
	if (m_level != 0)
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ADD_DATA/Level/%d", m_level);

	if (std::accumulate(m_actualCards.cbegin(), m_actualCards.cend(), 0) == 0)
		GiveMonsterCardMission();

	std::string data("MONSTERCARDSYSTEM ADD_DATA/Cards/");
	data += boost::algorithm::join(m_cardData | boost::adaptors::transformed([](std::size_t val) { return std::to_string(val); }), "/");
	m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, data.c_str());

	if (m_killCount == static_cast<int>(s_MAIN_MONSTER_CARD_COUNT))
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM SUCCES_MISSION");

	m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM OPEN");
	for (const auto& iter : m_monsterInfos)
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ADD_MOB_INFO/%u/%d/%d/%d/%d/%lld/%lld/%lld",
			static_cast<unsigned>(iter.first), iter.second.m_collectedCards,
			iter.second.m_killCount, iter.second.m_needCards, iter.second.m_stage,
			static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(iter.second.m_lastTeleport).time_since_epoch().count()),
			static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(iter.second.m_lastPoly).time_since_epoch().count()),
			static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(iter.second.m_lastSpawn).time_since_epoch().count()));
	}

	for (const auto& v : m_achievApplied)
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ACHIEV_APPLY/%d/1", static_cast<int>(v));
	for (const auto& it : m_achievRegistRank)
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ACHIEV_REGIST/%d/%d", static_cast<int>(it.first), it.second);
	m_needUpdate = true;
}

void kalisto::MonstercardSystem::DebugCompleteCurrentMission()
{
	// Ensure mission targets are selected
	if (std::accumulate(m_mainMonstercards.cbegin(), m_mainMonstercards.cend(), 0) == 0)
		ReceiveMonsterCardMission();

	if (m_mainMonstercards[0] == 0 || m_mainMonstercards[1] == 0 || m_mainMonstercards[2] == 0)
		return;

	m_killedMonsters[0] = 1;
	m_killedMonsters[1] = 1;
	m_killedMonsters[2] = 1;
	m_killCount = static_cast<int>(s_MAIN_MONSTER_CARD_COUNT);
	m_needUpdate = true;
	SendStateToClient();
}

void kalisto::MonstercardSystem::DebugIncreaseAllCollectionsStage()
{
	// If no entries exist yet, pre-create entries for all races from the mission table
	if (m_monsterInfos.empty())
	{
		const auto& t = GetMonsterCardList();
		for (std::size_t row = 0; row < s_WIKI_MISSION_LEVEL_MAX && row < t.size(); ++row)
		{
			for (std::size_t col = 0; col < t[row].size(); ++col)
			{
				const std::size_t race = t[row][col];
				if (race == 0)
					continue;
				GetOrAddMonstercardInfo(race);
			}
		}
	}
	// Also ensure monsters referenced by Achievements exist so IsAchievUnlocked() can pass in GM tests.
	// (Some achievements may reference mobs not present in the mission table.)
	{
		LoadAchievDefsOnce();
		for (const auto& kv : g_mcAchievDefs)
		{
			const auto& def = kv.second;
			for (std::size_t mv : def.monsters)
			{
				if (mv == 0)
					continue;
				GetOrAddMonstercardInfo(mv);
			}
		}
	}

	auto stageNeed = [](int stage) -> int {
		switch (stage)
		{
			case 1: return static_cast<int>(NeededCardStage::eSTAGE1);
			case 2: return static_cast<int>(NeededCardStage::eSTAGE2);
			case 3: return static_cast<int>(NeededCardStage::eSTAGE3);
			case 4: return static_cast<int>(NeededCardStage::eSTAGE4);
			case 5: return static_cast<int>(NeededCardStage::eSTAGE5);
			default: return static_cast<int>(NeededCardStage::eSTAGE1);
		}
	};

	for (auto& it : m_monsterInfos)
	{
		auto& info = it.second;
		int curStage = info.m_stage;
		if (curStage < 0)
			curStage = 0;
		if (curStage > 5)
			curStage = 5;

		const int nextStage = (curStage >= 5) ? 5 : (curStage + 1);
		const int need = stageNeed(nextStage);
		info.m_collectedCards = need;
		info.m_needCards = need;
		info.m_stage = nextStage;
	}

	m_needUpdate = true;
	SendStateToClient();
}

void kalisto::MonstercardSystem::DebugResetAll()
{
	// Wipe DB state for this account and reset runtime as if new.
	if (m_accountID == 0 || m_associatedCharacter == nullptr)
		return;

	static DBManager& dbmgr = DBManager::instance();

	// Best-effort deletes (tables may not exist on some shards).
	::snprintf(m_szQueryBuffer.data(), m_szQueryBuffer.size(),
		"DELETE FROM player.monstercard_achiev WHERE account_id=%u", static_cast<unsigned>(m_accountID));
	dbmgr.DirectQuery(m_szQueryBuffer.data());
	::snprintf(m_szQueryBuffer.data(), m_szQueryBuffer.size(),
		"DELETE FROM player.monstercard_status WHERE account_id=%u", static_cast<unsigned>(m_accountID));
	dbmgr.DirectQuery(m_szQueryBuffer.data());
	::snprintf(m_szQueryBuffer.data(), m_szQueryBuffer.size(),
		"DELETE FROM player.monstercard_mission WHERE account_id=%u", static_cast<unsigned>(m_accountID));
	dbmgr.DirectQuery(m_szQueryBuffer.data());

	// Reset runtime fields
	m_monsterInfos.clear();
	m_achievApplied.clear();
	m_achievRegistRank.clear();
	m_attackerMob = 0;
	m_killCount = 0;
	m_level = 0;
	m_missionResetWindowStartSec = 0;
	m_missionResetsInWindow = 0;
	std::memset(m_mainMonstercards.data(), 0, m_mainMonstercards.size() * sizeof(std::size_t));
	std::memset(m_actualCards.data(), 0, m_actualCards.size() * sizeof(std::size_t));
	std::memset(m_killedMonsters.data(), 0, m_killedMonsters.size() * sizeof(std::size_t));

	// Recreate base mission row + initial mission deck
	InsertMission();
	GiveMonsterCardMission();
	m_needUpdate = true;
	SendStateToClient();
}

const std::array<std::size_t, kalisto::MonstercardSystem::s_DIFFERENT_MONSTERS_PER_STAGE>& kalisto::MonstercardSystem::SetNextMonsterCardLevel()
{
	static const auto& CARDTABLE = GetMonsterCardList();

	// Official (wiki): wrap back to level 1 after completing level 15.
	if (m_level >= static_cast<int>(s_WIKI_MISSION_LEVEL_MAX))
		m_level = 0;

	// Table may contain extra rows for future; we only expose first 15 to the client.
	KALISTO_ASSERT(m_level + 1 <= static_cast<int>(s_WIKI_MISSION_LEVEL_MAX), "SendMonsterCardList m_level out of Range!");
	m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ADD_DATA/Level/%d", m_level + 1);
	m_needUpdate = true;
	return CARDTABLE[m_level++];
}

const kalisto::MonstercardSystem::CardTableT& kalisto::MonstercardSystem::GetMonsterCardList()
{
	return s_cMonsterVnums;
}

bool kalisto::MonstercardSystem::IsMonsterCardRace(std::size_t race)
{
	static const auto& CARDTABLE = GetMonsterCardList();
	static std::set<std::size_t> races;
	if (races.empty())
	{
		for (const auto& cardrow : CARDTABLE)
			races.insert(cardrow.begin(), cardrow.end());
	}
	return races.find(race) != races.end();
}

std::size_t kalisto::MonstercardSystem::PickRandomMonsterCardRaceFromTable()
{
	const auto& pool = GetMonsterCardUseItemRacePool();
	if (pool.empty())
		return 0;
	std::uniform_int_distribution<std::size_t> dist(0, pool.size() - 1);
	return pool[dist(m_twister)];
}

std::size_t kalisto::MonstercardSystem::RollMonsterCardUseItemRace()
{
	const auto& pool = GetMonsterCardUseItemRacePool();
	if (pool.empty())
		return 0;
	return pool[static_cast<std::size_t>(number(0, static_cast<int>(pool.size() - 1)))];
}

kalisto::MonstercardSystem::InfoIteratorT kalisto::MonstercardSystem::GetOrAddMonstercardInfo(std::size_t race)
{
	auto iter = m_monsterInfos.find(race);
	m_needUpdate = true;
	if (iter != m_monsterInfos.end())
		return iter;

	MonsterCardInfo info;
	m_monsterInfos.insert(std::make_pair(race, info));
	InsertMonsterStatus(race, info);
	return m_monsterInfos.find(race);
}

void kalisto::MonstercardSystem::ResetMonsterCardMission()
{
	if (std::accumulate(m_mainMonstercards.cbegin(), m_mainMonstercards.cend(), 0) == 0)
		return;

	RefreshMissionResetWindow();

	const int nextResetInWindow = m_missionResetsInWindow + 1;
	const int itemCost = GetMissionResetItemCostForNext(nextResetInWindow);
	if (itemCost < 0)
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, LC_STRING("MonsterCard: Mission reset limit reached within this 24-hour period."));
		return;
	}

	if (itemCost > 0)
	{
		if (m_associatedCharacter->CountSpecifyItem(static_cast<DWORD>(s_MISSION_RESET_CONSUMABLE_VNUM)) < itemCost)
		{
			m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, LC_STRING("MonsterCard: Not enough restart cards for mission reset."));
			return;
		}
		m_associatedCharacter->RemoveSpecifyItem(static_cast<DWORD>(s_MISSION_RESET_CONSUMABLE_VNUM), static_cast<DWORD>(itemCost));
	}

	if (m_missionResetsInWindow == 0)
		m_missionResetWindowStartSec = std::chrono::duration_cast<std::chrono::seconds>(ClockT::now().time_since_epoch()).count();
	++m_missionResetsInWindow;

	m_lastMissionReset = ClockT::now();
	m_level = 0;
	std::memset(m_mainMonstercards.data(), 0, m_mainMonstercards.size() * sizeof(std::size_t));
	std::memset(m_killedMonsters.data(), 0, m_killedMonsters.size() * sizeof(std::size_t));
	GiveMonsterCardMission();
	m_needUpdate = true;
}

void kalisto::MonstercardSystem::ResetMonsterCardOrder()
{
	if (std::accumulate(m_mainMonstercards.cbegin(), m_mainMonstercards.cend(), 0) == 0 || std::accumulate(m_killedMonsters.cbegin(), m_killedMonsters.cend(), 0) == static_cast<int>(s_MAIN_MONSTER_CARD_COUNT))
		return;

	if (m_associatedCharacter->CountSpecifyItem(static_cast<DWORD>(s_MISSION_SHUFFLE_CONSUMABLE_VNUM)) < 1)
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM MISSION_FAIL/0/0");
		m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, LC_STRING("MonsterCard: Not enough shuffle cards to reroll mission objectives."));
		return;
	}
	m_associatedCharacter->RemoveSpecifyItem(static_cast<DWORD>(s_MISSION_SHUFFLE_CONSUMABLE_VNUM), 1);

	m_lastOrderReset = ClockT::now();
	std::memset(m_mainMonstercards.data(), 0, m_mainMonstercards.size() * sizeof(std::size_t));
	std::memset(m_killedMonsters.data(), 0, m_killedMonsters.size() * sizeof(std::size_t));
	SendStateToClient();
	m_needUpdate = true;
}

void kalisto::MonstercardSystem::InsertMission()
{
	static constexpr const char* MISSION_QUERY =
		"INSERT INTO player.monstercard_mission (account_id,main_card0,main_card1,main_card2,card0,card1,card2,card3,card4,card5,card6,card7,card8,card9,card10,card11,card12,card13,card14,card15,"
		"main_card0_killed,main_card1_killed,main_card2_killed,last_mission_back,last_order_back,monstercardsystem_level) "
		"VALUES (%d,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,%lld,%lld,1)";

	::snprintf(m_szQueryBuffer.data(), m_szQueryBuffer.size(), MISSION_QUERY, static_cast<int>(m_accountID),
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(m_lastMissionReset).time_since_epoch().count()),
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(m_lastOrderReset).time_since_epoch().count()));
	static DBManager& dbmgr = DBManager::instance();
	dbmgr.DirectQuery(m_szQueryBuffer.data());
}

void kalisto::MonstercardSystem::InsertMonsterStatus(std::size_t race, const MonsterCardInfo& info)
{
	static constexpr const char* STATUS_INFO_QUERY = "INSERT INTO player.monstercard_status VALUES(%d, %d, %d, %d, %d, %d, %lld, %lld, %lld, %lld)";

	::snprintf(m_szQueryBuffer.data(), m_szQueryBuffer.size(), STATUS_INFO_QUERY, static_cast<int>(m_accountID), static_cast<int>(race), info.m_collectedCards, info.m_killCount,
		info.m_needCards, info.m_stage,
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(info.m_lastTeleport).time_since_epoch().count()),
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(info.m_lastPoly).time_since_epoch().count()),
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(info.m_lastSpawn).time_since_epoch().count()),
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(info.m_lastFight).time_since_epoch().count()));
	static DBManager& dbmgr = DBManager::instance();
	dbmgr.DirectQuery(m_szQueryBuffer.data());
}

void kalisto::MonstercardSystem::OnUseMonsterCardFeature(const std::vector<std::string>& clientArguments)
{
	if (clientArguments.size() < 2)
	{
		KALISTO_ASSERT(false, "too few clientArguments!");
		return;
	}
	enum
	{
		FEATURE_INDEX = 1,
		MOB_INDEX = 2,
		VICTIM_NAME_INDEX = 3
	};

	std::size_t mobVnum = 0;
	MonsterFeature feature = MonsterFeature::eFEATURE_UNKNOWN;
	try
	{
		feature = static_cast<MonsterFeature>(boost::lexical_cast<std::size_t>(clientArguments[FEATURE_INDEX]));
		mobVnum = boost::lexical_cast<std::size_t>(clientArguments[MOB_INDEX]);
	}
	catch (const boost::bad_lexical_cast&)
	{
		KALISTO_ASSERT(false, "OnUseMonsterCardFeature a client argument wasnt a number!!");
		return;
	}

	if (feature == MonsterFeature::eFEATURE_REKRUTE && clientArguments.size() < 3)
	{
		KALISTO_ASSERT(false, "too few clientArguments!");
		return;
	}

	auto mobIter = m_monsterInfos.find(mobVnum);
	if (mobIter == m_monsterInfos.end())
	{
		KALISTO_ASSERT(false, "Invalid mobVnum how can MonsterCardFeature be used???");
		return;
	}

	static const std::string EmptyString("");
	const std::string& victimName = (feature == MonsterFeature::eFEATURE_REKRUTE) ? clientArguments[VICTIM_NAME_INDEX] : EmptyString;
	DispatchFeature(feature, mobVnum, mobIter, victimName);
}

kalisto::MonstercardSystem::WaitResult kalisto::MonstercardSystem::WaitTimeIsOver(WaitEvent event, std::size_t mobVnum) const
{
	auto now = ClockT::now();
	auto mobInfoLambda = [mobVnum, this]() -> const MonsterCardInfo* {
		auto mobIter = m_monsterInfos.find(mobVnum);
		if (mobIter == m_monsterInfos.end())
		{
			KALISTO_ASSERT(false, __FUNCTION__ " unknown mobVnum !!");
			return static_cast<const MonsterCardInfo*>(nullptr);
		}
		return &mobIter->second;
	};

	WaitResult result {};
	switch (event)
	{
	case WaitEvent::eEVENT_RESET_MISSION:
		// Mission full reset uses per-24h item costs (official); no fixed cooldown here.
		result.hasToWait = false;
		result.timeToWait = 0;
		break;
	case WaitEvent::eEVENT_RESET_ORDER:
		// Mission target shuffle consumes New Order card (official); no fixed cooldown here.
		result.hasToWait = false;
		result.timeToWait = 0;
		break;
	case WaitEvent::eEVENT_FEATURE_WARP:
		result.timeToWait = std::chrono::duration_cast<std::chrono::seconds>(now - mobInfoLambda()->m_lastTeleport).count() - static_cast<DurationTypeT>(WaitTime::eWAIT_FEATURE_WARP);
		break;
	case WaitEvent::eEVENT_FEATURE_POLY:
		result.timeToWait = std::chrono::duration_cast<std::chrono::seconds>(now - mobInfoLambda()->m_lastPoly).count() - static_cast<DurationTypeT>(WaitTime::eWAIT_FEATURE_POLY);
		break;
	case WaitEvent::eEVENT_FEATURE_SPAWN:
		result.timeToWait = std::chrono::duration_cast<std::chrono::seconds>(now - mobInfoLambda()->m_lastSpawn).count() - static_cast<DurationTypeT>(WaitTime::eWAIT_FEATURE_SPAWN);
		break;
	case WaitEvent::eEVENT_FEATURE_REKRUTE:
		result.timeToWait = std::chrono::duration_cast<std::chrono::seconds>(now - mobInfoLambda()->m_lastFight).count() - static_cast<DurationTypeT>(WaitTime::eWAIT_FEATURE_REKRUTE);
		break;
	default:
		result.timeToWait = std::numeric_limits<DurationTypeT>::max();
		result.hasToWait = true;
		KALISTO_ASSERT(false, "__FUNCTION__ unknown WaitEvent !!!!");
		break;
	}
	result.hasToWait = result.timeToWait < 0;
	result.timeToWait = std::abs(result.timeToWait);
	return result;
}

void kalisto::MonstercardSystem::OnWarpToRekutedMonster() const
{
	LPCHARACTER mob = CHARACTER_MANAGER::instance().Find(static_cast<DWORD>(m_attackerMob));
	if (mob == nullptr)
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM MOB_IS_ALREADY_DEAD");
		return;
	}
	m_associatedCharacter->WarpSet(mob->GetX() + 100, mob->GetY() + 100);
}

void kalisto::MonstercardSystem::DispatchFeature(MonsterFeature feature, std::size_t mobVnum, const InfoIteratorT& mobIter, const std::string& victimName)
{
	auto checkStarLambda = [this, &mobIter](int needStar) -> bool {
		const int curStar = (mobIter->second.m_stage > 0) ? (mobIter->second.m_stage - 1) : 0;
		if (curStar < needStar)
		{
			m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM NO_NEED_STAGE/%d", needStar);
			return false;
		}
		return true;
	};

	switch (feature)
	{
	case MonsterFeature::eFEATURE_WARP:
		if (!ValidateFeatureWaitTime(feature, mobVnum) || !checkStarLambda(4))
			break;
		UseFeatureWarp(mobVnum, mobIter);
		break;
	case MonsterFeature::eFEATURE_POLY:
		if (!ValidateFeatureWaitTime(feature, mobVnum) || !checkStarLambda(3))
			break;
		UseFeaturePoly(mobVnum, mobIter);
		break;
	case MonsterFeature::eFEATURE_DROPPGUI:
		if (!checkStarLambda(1))
			break;
		UseFeatureDroppGui(mobVnum, mobIter);
		break;
	case MonsterFeature::eFEATURE_SPAWN:
		if (!ValidateFeatureWaitTime(feature, mobVnum) || !checkStarLambda(5))
			break;
		UseFeatureSpawn(mobVnum, mobIter);
		break;
	case MonsterFeature::eFEATURE_REKRUTE:
		if (!ValidateFeatureWaitTime(feature, mobVnum) || !checkStarLambda(5))
			break;
		UseFeatureRekrute(mobVnum, mobIter, victimName);
		break;
	case MonsterFeature::eFEATURE_TRADE:
		UseFeatureTrade(mobVnum, mobIter);
		break;
	case MonsterFeature::eFEATURE_PROMOTION:
		UseFeaturePromotion(mobVnum, mobIter);
		break;
	default:
		KALISTO_ASSERT(false, "Invalid monster feature!");
		break;
	}
}

void kalisto::MonstercardSystem::UseFeaturePoly(std::size_t mobVnum, const InfoIteratorT& mobIter)
{
	// Official behavior for Monster Card polymorph: transform the player directly (no item given).
	if (m_associatedCharacter == nullptr)
		return;
	if (m_associatedCharacter->IsPolymorphed())
		return;
	if (m_associatedCharacter->IsRiding())
		return;

	const CMob* pMob = CMobManager::instance().Get(static_cast<DWORD>(mobVnum));
	if (pMob == nullptr)
		return;

	// Reuse base polymorph safety: don't allow transforming into much higher-level mobs.
	const int iPolymorphLevelLimit = MAX(0, 20 - m_associatedCharacter->GetLevel() * 3 / 10);
	if (pMob->m_table.bLevel >= m_associatedCharacter->GetLevel() + iPolymorphLevelLimit)
		return;

	// Skill id for polymorph is stable in this codebase (see polymorph.h) but we avoid including it here.
	static constexpr int s_POLYMORPH_SKILL_ID_LOCAL = 129;
	int iDuration = m_associatedCharacter->GetSkillLevel(s_POLYMORPH_SKILL_ID_LOCAL) == 0 ? 5 : (5 + (5 + m_associatedCharacter->GetSkillLevel(s_POLYMORPH_SKILL_ID_LOCAL) / 40 * 25));
	iDuration *= 60;

	// Add polymorph affect (no attack bonus for Monster Card polymorph).
	m_associatedCharacter->AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, static_cast<long>(mobVnum), AFF_POLYMORPH, iDuration, 0, true);
	m_associatedCharacter->SetPolymorph(static_cast<DWORD>(mobVnum));

	mobIter->second.m_lastPoly = ClockT::now();
	m_needUpdate = true;
}

void kalisto::MonstercardSystem::UseFeatureWarp(std::size_t mobVnum, const InfoIteratorT& mobIter)
{
	// Warp: target map from data; landing near authored hunt coords, snapped to walkable ground.
	auto infoIter = std::find_if(s_cMonsterWarpInfos.begin(), s_cMonsterWarpInfos.end(), [mobVnum = mobIter->first](const MonsterWarpInfo& info) { return info.vnum == mobVnum; });
	if (infoIter == s_cMonsterWarpInfos.end())
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, LC_STRING("No teleport information available."));
		return;
	}

	PIXEL_POSITION mapBasePos {};
	bool mapExits = SECTREE_MANAGER::instance().GetMapBasePositionByMapIndex(infoIter->mapIndex, OUT mapBasePos);
	if (!mapExits)
	{
		sys_err("player %s wants to warp to an invalid map with index %ld!", m_associatedCharacter->GetName(), infoIter->mapIndex);
		return;
	}

	const long anchorX = mapBasePos.x + (infoIter->x * 100);
	const long anchorY = mapBasePos.y + (infoIter->y * 100);
	// Prefer walkable ground near authored coords (GetRandomLocation skips ATTR_BLOCK|ATTR_OBJECT).
	static constexpr int kWarpRandomMaxDistance = 2000;
	PIXEL_POSITION warpPos {};
	if (SECTREE_MANAGER::instance().GetRandomLocation(infoIter->mapIndex, warpPos, static_cast<DWORD>(anchorX), static_cast<DWORD>(anchorY), kWarpRandomMaxDistance))
		m_associatedCharacter->WarpSet(warpPos.x, warpPos.y);
	else
		m_associatedCharacter->WarpSet(anchorX, anchorY);
	mobIter->second.m_lastTeleport = ClockT::now();
	m_needUpdate = true;
}

void kalisto::MonstercardSystem::UseFeatureRekrute(std::size_t mobVnum, const InfoIteratorT& mobIter, const std::string& victimName)
{
	LPCHARACTER victim = CHARACTER_MANAGER::instance().FindPC(victimName.c_str());
	if (victim == nullptr)
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM PLAYER_DONT_EXIST");
		return;
	}

	int x = victim->GetX();
	int y = victim->GetY();
	int map_index = victim->GetMapIndex();
	LPCHARACTER mob = CHARACTER_MANAGER::instance().SpawnMobRange(mobVnum, map_index, x - number(200, 750), y - number(200, 750), x + number(200, 750), y + number(200, 750), true);
	if (mob == nullptr)
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, LC_STRING("Monster was not spawned!"));
		return;
	}

	char szNameBuffer[128];
	::snprintf(szNameBuffer, 128, "%s's %s", mob->GetName(), LC_STRING("Attacker"));
	mob->SetName(szNameBuffer);
	mob->Follow(victim, 0.0f);
	mob->BeginFight(victim);
	mobIter->second.m_lastFight = ClockT::now();
	m_needUpdate = true;
	m_attackerMob = static_cast<std::size_t>(mob->GetVID());
}

void kalisto::MonstercardSystem::UseFeatureTrade(std::size_t mobVnum, const InfoIteratorT& /*mobIter*/)
{
	int cardCount = m_associatedCharacter->CountSpecifyItemBySocket(
		static_cast<DWORD>(s_MONSTERCARD_VNUM),
		static_cast<int>(s_ITEM_MONSTERCARD_SOCKET_INDEX),
		static_cast<int>(mobVnum)
	);
	if (cardCount < static_cast<int>(s_NEEDED_CARDCOUNT_TRADE))
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM NOT_ENOUGH_FOR_TRADE");
		return;
	}
	m_associatedCharacter->RemoveSpecifyItemBySocket(
		static_cast<DWORD>(s_MONSTERCARD_VNUM),
		static_cast<DWORD>(s_NEEDED_CARDCOUNT_TRADE),
		static_cast<int>(s_ITEM_MONSTERCARD_SOCKET_INDEX),
		static_cast<int>(mobVnum)
	);

	LPITEM tradeableCard = m_associatedCharacter->AutoGiveItem(static_cast<DWORD>(s_MONSTERCARD_TRADEABLE_VNUM));
	tradeableCard->SetSocket(static_cast<int>(s_ITEM_MONSTERCARD_SOCKET_INDEX), static_cast<long>(mobVnum));
}

void kalisto::MonstercardSystem::UseFeatureDroppGui(std::size_t mobVnum, const InfoIteratorT& /*mobIter*/)
{
	static constexpr DurationTypeT WaitDroppGuiSeconds = 3;
	auto nowStamp = ClockT::now();
	auto timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(nowStamp - m_lastDroppGui).count();
	if (timeElapsed < WaitDroppGuiSeconds)
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, LC_STRING("You can only do this every 3 seconds."));
		return;
	}

	CDropItemGroup* pDropGroup = ITEM_MANAGER::instance().FindDropItemGroupByMobVnum(static_cast<DWORD>(mobVnum));
	if (pDropGroup == nullptr)
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, LC_STRING("This monster has no drops!"));
		return;
	}

	const DropItemGroupInfoVector& drops = pDropGroup->GetVector();
	if (drops.empty())
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, LC_STRING("This monster has no drops!"));
		return;
	}

	m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM NEW_DROPP_GUI/%d", static_cast<int>(mobVnum));
	for (const auto& dropp : drops)
	{
		const double droppChance = (std::min)(100.0, static_cast<double>(dropp.dwPct) * 4.0);
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ADD_DROPP/%d/%d/%.2f", static_cast<int>(dropp.dwVnum), dropp.iCount, droppChance);
	}
	m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM OPEN_DROPP_GUI");
	m_lastDroppGui = nowStamp;
}

void kalisto::MonstercardSystem::UseFeatureSpawn(std::size_t mobVnum, const InfoIteratorT& mobIter)
{
	CHARACTER_MANAGER::instance().SpawnMobRange(mobVnum, m_associatedCharacter->GetMapIndex(), m_associatedCharacter->GetX() - number(200, 750),
		m_associatedCharacter->GetY() - number(200, 750), m_associatedCharacter->GetX() + number(200, 750), m_associatedCharacter->GetY() + number(200, 750), true);
	mobIter->second.m_lastSpawn = ClockT::now();
	m_needUpdate = true;
}

void kalisto::MonstercardSystem::UseFeaturePromotion(std::size_t mobVnum, const InfoIteratorT& mobIter)
{
	if (mobIter->second.m_stage >= static_cast<int>(s_STAR_COUNT + 1))
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_INFO, LC_STRING("This monster has already reached the maximum level!"));
		return;
	}

	// Need current stage requirement to be fulfilled
	int needCards = 9;
	switch (mobIter->second.m_stage - 1)
	{
		case 0: needCards = 9; break;
		case 1: needCards = 21; break;
		case 2: needCards = 30; break;
		case 3: needCards = 60; break;
		case 4: needCards = 90; break;
		default: needCards = 120; break;
	}
	if (mobIter->second.m_collectedCards < needCards)
	{
		m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM NO_PROMOTION");
		return;
	}

	// Promote to next star class; reset current-stage count.
	++mobIter->second.m_stage;
	mobIter->second.m_collectedCards = 0;

	// Update next requirement
	switch (mobIter->second.m_stage - 1)
	{
		case 0: mobIter->second.m_needCards = 9; break;
		case 1: mobIter->second.m_needCards = 21; break;
		case 2: mobIter->second.m_needCards = 30; break;
		case 3: mobIter->second.m_needCards = 60; break;
		case 4: mobIter->second.m_needCards = 90; break;
		default: mobIter->second.m_needCards = 120; break;
	}

	const int stageOut = (mobIter->second.m_stage > 0) ? (mobIter->second.m_stage - 1) : 0;
	m_associatedCharacter->ChatPacket(CHAT_TYPE_COMMAND, "MONSTERCARDSYSTEM ADD_MOB_INFO/%d/%d/%d/%d/%d/%lld/%lld/%lld",
		static_cast<int>(mobVnum), mobIter->second.m_collectedCards,
		mobIter->second.m_killCount, mobIter->second.m_needCards, stageOut,
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(mobIter->second.m_lastTeleport).time_since_epoch().count()),
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(mobIter->second.m_lastPoly).time_since_epoch().count()),
		static_cast<long long>(std::chrono::time_point_cast<std::chrono::seconds>(mobIter->second.m_lastSpawn).time_since_epoch().count()));

	m_needUpdate = true;
}

float kalisto::MonstercardSystem::GetAdditonalMonsterDamagePercent(std::size_t vnum) const
{
	auto infoIter = m_monsterInfos.find(vnum);
	if (infoIter == m_monsterInfos.end())
		return 1.0f;

	return (infoIter->second.m_collectedCards == static_cast<int>(NeededCardStage::eSTAGE5)) ? 2.0f : (100.0f / static_cast<float>(infoIter->second.m_collectedCards)) + 1.0f;
}

#endif // ENABLE_MONSTER_CARD

