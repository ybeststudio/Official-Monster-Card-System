#include "StdAfx.h"

#if defined(ENABLE_MONSTER_CARD)

#include <algorithm>
#include <cstdlib>

#include "../EterPack/EterPackManager.h"

#include "PythonMonsterCardManager.h"
#include "Locale.h"

CPythonMonsterCardManager& CPythonMonsterCardManager::Instance()
{
	static CPythonMonsterCardManager s;
	return s;
}

void CPythonMonsterCardManager::Initialize()
{
	m_fileLoaded = false;
	m_missionLoaded = false;
	m_illustrationLoaded = false;
	m_groupEntries.clear();
	m_byVnum.clear();
	m_mobInfo.clear();
	m_deck.clear();
	m_achievApplied.clear();
	m_achievRegistRank.clear();

	m_stage = 0;
	m_mainCards[0] = m_mainCards[1] = m_mainCards[2] = 0;
	m_killed[0] = m_killed[1] = m_killed[2] = 0;
}

void CPythonMonsterCardManager::Destroy()
{
	Initialize();
}

static bool SplitTabs(const std::string& line, std::vector<std::string>& out)
{
	out.clear();
	std::string cur;
	for (std::size_t i = 0; i < line.size(); ++i)
	{
		char c = line[i];
		if (c == '\t' || c == ' ')
		{
			if (!cur.empty())
			{
				out.push_back(cur);
				cur.clear();
			}
			continue;
		}
		cur.push_back(c);
	}
	if (!cur.empty())
		out.push_back(cur);
	return !out.empty();
}

static int ParseInt10(const std::string& s)
{
	return static_cast<int>(std::strtol(s.c_str(), nullptr, 10));
}

static DWORD ParseDword10(const std::string& s)
{
	return static_cast<DWORD>(std::strtoul(s.c_str(), nullptr, 10));
}

bool CPythonMonsterCardManager::LoadTable()
{
	if (m_fileLoaded)
		return true;

	CMappedFile file;
	const VOID* pvData;
	// Mission pool is shared across all languages; load from common only.
	std::vector<std::string> candidates;
	candidates.push_back("locale/locale/common/monster_card.txt");
	candidates.push_back("locale/common/monster_card.txt");

	bool loaded = false;
	for (const auto& path : candidates)
	{
		if (CEterPackManager::Instance().Get(file, path.c_str(), &pvData))
		{
			loaded = true;
			break;
		}
	}
	if (!loaded)
		return false;

	CMemoryTextFileLoader loader;
	loader.Bind(file.Size(), pvData);

	std::vector<std::string> cols;
	for (DWORD i = 0; i < loader.GetLineCount(); ++i)
	{
		const std::string& line = loader.GetLineString(i);
		if (line.empty())
			continue;
		if (line[0] == '#')
			continue;

		if (!SplitTabs(line, cols))
			continue;
		if (cols.size() < 7)
			continue;

		SMissionEntry e {};
		const int group = ParseInt10(cols[0]);
		// const int index = ParseInt10(cols[1]); // ordering already file order
		e.vnum = ParseDword10(cols[2]);
		e.type = ParseInt10(cols[3]);
		e.areaIndex = ParseInt10(cols[4]);
		e.mapIndex0 = ParseInt10(cols[5]);
		e.mapIndex1 = ParseInt10(cols[6]);
		e.mapIndex2 = (cols.size() >= 8) ? ParseInt10(cols[7]) : 0;
		e.level = 0;

		m_groupEntries[group].push_back(e);
		if (m_byVnum.find(e.vnum) == m_byVnum.end())
			m_byVnum.emplace(e.vnum, e);
	}

	m_fileLoaded = true;
	return true;
}

void CPythonMonsterCardManager::SetMissionStage(int stage)
{
	m_stage = stage;
	m_missionLoaded = true;
}

void CPythonMonsterCardManager::SetMissionMainCards(DWORD c0, DWORD c1, DWORD c2)
{
	m_mainCards[0] = c0;
	m_mainCards[1] = c1;
	m_mainCards[2] = c2;
	m_missionLoaded = true;
}

void CPythonMonsterCardManager::SetMissionKilledFlags(int k0, int k1, int k2)
{
	m_killed[0] = k0;
	m_killed[1] = k1;
	m_killed[2] = k2;
	m_missionLoaded = true;
}

void CPythonMonsterCardManager::SetMissionDeck(const std::vector<DWORD>& deck)
{
	m_deck = deck;
	m_missionLoaded = true;
}

void CPythonMonsterCardManager::UpsertMobInfo(DWORD vnum, const SMobInfo& info)
{
	m_mobInfo[vnum] = info;
}

bool CPythonMonsterCardManager::GetMobInfo(DWORD vnum, SMobInfo& out) const
{
	const auto it = m_mobInfo.find(vnum);
	if (it == m_mobInfo.end())
		return false;
	out = it->second;
	return true;
}

void CPythonMonsterCardManager::SetAchievApplied(DWORD vnum, bool applied)
{
	if (applied)
		m_achievApplied.insert(vnum);
	else
		m_achievApplied.erase(vnum);
}

bool CPythonMonsterCardManager::IsAchievApplied(DWORD vnum) const
{
	return m_achievApplied.find(vnum) != m_achievApplied.end();
}

void CPythonMonsterCardManager::SetAchievRegistRank(DWORD vnum, int rank)
{
	if (rank <= 0)
		m_achievRegistRank.erase(vnum);
	else
		m_achievRegistRank[vnum] = rank;
}

int CPythonMonsterCardManager::GetAchievRegistRank(DWORD vnum) const
{
	const auto it = m_achievRegistRank.find(vnum);
	if (it == m_achievRegistRank.end())
		return 0;
	return it->second;
}

bool CPythonMonsterCardManager::GetMissionInfo(int& outStage, std::vector<DWORD>& outMainCards, std::vector<int>& outClears, long long& outResetTime, int& outResetCount, int& outShuffleCount) const
{
	outStage = m_stage;
	outMainCards.assign(&m_mainCards[0], &m_mainCards[0] + 3);
	outClears.assign(&m_killed[0], &m_killed[0] + 3);

	// Server-side implementation currently does not expose these as official client expects.
	outResetTime = 0;
	outResetCount = 0;
	outShuffleCount = 0;

	return true;
}

void CPythonMonsterCardManager::GetMissionVec(int group, std::vector<SMissionEntry>& out) const
{
	out.clear();
	const auto it = m_groupEntries.find(group);
	if (it == m_groupEntries.end())
		return;

	// UI has 8*2 slots; keep it bounded.
	out = it->second;
	if (out.size() > 16)
		out.resize(16);
}

bool CPythonMonsterCardManager::GetMobEmergenceAreaIndex(DWORD vnum, std::vector<int>& outMapIndices) const
{
	outMapIndices.clear();
	const auto it = m_byVnum.find(vnum);
	if (it == m_byVnum.end())
		return false;
	outMapIndices.push_back(it->second.mapIndex0);
	outMapIndices.push_back(it->second.mapIndex1);
	outMapIndices.push_back(it->second.mapIndex2);
	return true;
}

int CPythonMonsterCardManager::GetIllustrationSoloPageMax() const
{
	// Official field collection order: 56 slots (7 pages), last 2 are locked => expose 54 cards.
	static const DWORD kOfficialFieldVnums[] = {
		151, 153, 152, 154, 391, 155, 393, 191,
		192, 394, 193, 194, 431, 491, 432, 403,
		434, 492, 435, 493, 436, 591, 494, 533,
		534, 691, 791, 2191, 1901, 2206, 1304, 2306,
		3910, 3891, 3491, 3591, 3291, 3091, 3191, 6392,
		3791, 3596, 6407, 6699, 6705, 6706, 6723, 6724,
		6749, 6764, 6776, 6783, 6920, 6922,
	};
	const int count = static_cast<int>(sizeof(kOfficialFieldVnums) / sizeof(kOfficialFieldVnums[0]));
	return (count + 7) / 8;
}

int CPythonMonsterCardManager::GetIllustrationPartyPageMax() const
{
	// Official dungeon collection order. Last UI slot is locked (empty), so we expose 47 cards (6 pages).
	static const DWORD kOfficialDungeonVnums[] = {
		5161, 5162, 5163, 1091, 1093, 2092, 2402, 1192,
		2597, 2492, 6405, 2493, 6109, 6191, 6207, 6009,
		6091, 6408, 6192, 2752, 2762, 2772, 2782, 2792,
		2802, 2812, 2822, 2832, 2842, 2852, 2862, 6805,
		6815, 6820, 6856, 7609, 7610, 7611, 7612, 7613,
		7614, 6789, 6797, 6791, 6937, 6938, 6939,
	};
	const int count = static_cast<int>(sizeof(kOfficialDungeonVnums) / sizeof(kOfficialDungeonVnums[0]));
	return (count + 7) / 8;
}

static void BuildIllustrationPage(const std::map<int, std::vector<CPythonMonsterCardManager::SMissionEntry>>& groups, int wantType, int page, std::vector<CPythonMonsterCardManager::SMissionEntry>& out)
{
	out.clear();
	std::vector<CPythonMonsterCardManager::SMissionEntry> all;
	for (const auto& g : groups)
		for (const auto& e : g.second)
			if (e.type == wantType)
				all.push_back(e);

	if (page <= 0)
		return;
	const int start = (page - 1) * 8;
	if (start >= static_cast<int>(all.size()))
		return;
	const int end = (std::min)(start + 8, static_cast<int>(all.size()));
	out.assign(all.begin() + start, all.begin() + end);
}

void CPythonMonsterCardManager::GetIllustrationSoloPageData(int page, std::vector<SMissionEntry>& out) const
{
	out.clear();
	if (page <= 0)
		return;

	static const DWORD kOfficialFieldVnums[] = {
		151, 153, 152, 154, 391, 155, 393, 191,
		192, 394, 193, 194, 431, 491, 432, 403,
		434, 492, 435, 493, 436, 591, 494, 533,
		534, 691, 791, 2191, 1901, 2206, 1304, 2306,
		3910, 3891, 3491, 3591, 3291, 3091, 3191, 6392,
		3791, 3596, 6407, 6699, 6705, 6706, 6723, 6724,
		6749, 6764, 6776, 6783, 6920, 6922,
	};
	const int total = static_cast<int>(sizeof(kOfficialFieldVnums) / sizeof(kOfficialFieldVnums[0]));
	const int start = (page - 1) * 8;
	if (start >= total)
		return;
	const int end = (std::min)(start + 8, total);
	out.reserve(end - start);
	for (int i = start; i < end; ++i)
	{
		const DWORD vnum = kOfficialFieldVnums[i];
		SMissionEntry e {};
		const auto it = m_byVnum.find(vnum);
		if (it != m_byVnum.end())
			e = it->second;
		else
		{
			e.vnum = vnum;
			e.type = 0;
			e.areaIndex = 0;
			e.mapIndex0 = e.mapIndex1 = e.mapIndex2 = 0;
			e.level = 0;
		}
		out.push_back(e);
	}
}

void CPythonMonsterCardManager::GetIllustrationPartyPageData(int page, std::vector<SMissionEntry>& out) const
{
	out.clear();
	if (page <= 0)
		return;

	static const DWORD kOfficialDungeonVnums[] = {
		5161, 5162, 5163, 1091, 1093, 2092, 2402, 1192,
		2597, 2492, 6405, 2493, 6109, 6191, 6207, 6009,
		6091, 6408, 6192, 2752, 2762, 2772, 2782, 2792,
		2802, 2812, 2822, 2832, 2842, 2852, 2862, 6805,
		6815, 6820, 6856, 7609, 7610, 7611, 7612, 7613,
		7614, 6789, 6797, 6791, 6937, 6938, 6939,
	};
	const int total = static_cast<int>(sizeof(kOfficialDungeonVnums) / sizeof(kOfficialDungeonVnums[0]));
	const int start = (page - 1) * 8;
	if (start >= total)
		return;
	const int end = (std::min)(start + 8, total);
	out.reserve(end - start);
	for (int i = start; i < end; ++i)
	{
		const DWORD vnum = kOfficialDungeonVnums[i];
		SMissionEntry e {};
		const auto it = m_byVnum.find(vnum);
		if (it != m_byVnum.end())
			e = it->second;
		else
		{
			// If not present in monster_card.txt, still expose vnum so UI can render the card image.
			e.vnum = vnum;
			e.type = 1;
			e.areaIndex = 0;
			e.mapIndex0 = e.mapIndex1 = e.mapIndex2 = 0;
			e.level = 0;
		}
		out.push_back(e);
	}
}

#endif // ENABLE_MONSTER_CARD

