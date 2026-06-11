#pragma once

#if defined(ENABLE_MONSTER_CARD)

#include <map>
#include <set>
#include <string>
#include <vector>

class CPythonMonsterCardManager
{
public:
	struct SMissionEntry
	{
		DWORD vnum;
		int level;
		int type; // 0 solo, 1 party
		int areaIndex;
		int mapIndex0;
		int mapIndex1;
		int mapIndex2;
	};

	struct SMobInfo
	{
		int collectedCards;
		int killCount;
		int needCards;
		int stage;
		long long lastTeleport;
		long long lastPoly;
		long long lastSpawn;
	};

	static CPythonMonsterCardManager& Instance();

	void Initialize();
	void Destroy();

	bool LoadTable();
	bool IsFileLoaded() const { return m_fileLoaded; }

	// Runtime state (fed from server command strings)
	void SetMissionStage(int stage);
	void SetMissionMainCards(DWORD c0, DWORD c1, DWORD c2);
	void SetMissionKilledFlags(int k0, int k1, int k2);
	void SetMissionDeck(const std::vector<DWORD>& deck);
	void SetMissionLoaded(bool loaded) { m_missionLoaded = loaded; }
	bool IsMissionLoaded() const { return m_missionLoaded; }

	// Illustration runtime loaded flag (fed from server command strings)
	void SetIllustrationLoaded(bool loaded) { m_illustrationLoaded = loaded; }
	bool IsIllustrationLoaded() const { return m_illustrationLoaded; }

	void UpsertMobInfo(DWORD vnum, const SMobInfo& info);
	bool GetMobInfo(DWORD vnum, SMobInfo& out) const;

	// Achievements runtime state (fed from server command strings)
	void SetAchievApplied(DWORD vnum, bool applied);
	bool IsAchievApplied(DWORD vnum) const;
	void SetAchievRegistRank(DWORD vnum, int rank);
	int GetAchievRegistRank(DWORD vnum) const;

	// Query API for python wrappers
	bool GetMissionInfo(int& outStage, std::vector<DWORD>& outMainCards, std::vector<int>& outClears, long long& outResetTime, int& outResetCount, int& outShuffleCount) const;
	void GetMissionVec(int group, std::vector<SMissionEntry>& out) const;
	bool GetMobEmergenceAreaIndex(DWORD vnum, std::vector<int>& outMapIndices) const;

	// Illustration page API (from table)
	int GetIllustrationSoloPageMax() const;
	int GetIllustrationPartyPageMax() const;
	void GetIllustrationSoloPageData(int page, std::vector<SMissionEntry>& out) const;
	void GetIllustrationPartyPageData(int page, std::vector<SMissionEntry>& out) const;

private:
	CPythonMonsterCardManager() = default;

private:
	bool m_fileLoaded = false;
	bool m_missionLoaded = false;
	bool m_illustrationLoaded = false;

	// Parsed from monster_card.txt (common or per-locale; see LoadTable()).
	std::map<int, std::vector<SMissionEntry>> m_groupEntries; // group -> entries
	std::map<DWORD, SMissionEntry> m_byVnum; // first match for emergence

	// Runtime
	int m_stage = 0;
	DWORD m_mainCards[3] = { 0, 0, 0 };
	int m_killed[3] = { 0, 0, 0 };
	std::vector<DWORD> m_deck;
	std::map<DWORD, SMobInfo> m_mobInfo;

	std::set<DWORD> m_achievApplied;
	std::map<DWORD, int> m_achievRegistRank;
};

#endif // ENABLE_MONSTER_CARD

