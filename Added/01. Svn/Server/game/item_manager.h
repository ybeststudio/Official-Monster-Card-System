#ifndef __INC_ITEM_MANAGER_H__
#define __INC_ITEM_MANAGER_H__

#include "../common/stl.h"

#if defined(__ITEM_APPLY_RANDOM__)
#	include "item_apply_random_table.h"
#endif

#if defined(__GEM_SYSTEM__)
struct SGemRefineInfo
{
	DWORD dwRefineItemVNum;
	WORD wRefineItemCount;
	BYTE bRefinePct;
	WORD wRefineCost;
	WORD wRefineResultCount;
#	if defined(__CONQUEROR_LEVEL__)
	BYTE bRefinePctSpecial;
	int iRefineSpecialMapIndex;
#	endif
};

struct SGemShopInfo
{
	BYTE bDefaultOpenedSlots;
	DWORD dwRefreshTime;
	DWORD dwRefreshItemVNum;
	DWORD dwAddSlotItemVNum;
};

#	if defined(__GEM_SHOP__)
struct SGemShopItemInfo
{
	DWORD dwVNum;
	BYTE bCount;
	WORD wPrice;
	SGemShopItemInfo(const DWORD c_dwVNum, const BYTE c_bCount, const WORD c_wPrice)
		: dwVNum(c_dwVNum), bCount(c_bCount), wPrice(c_wPrice)
	{}
};
using GemShopItemVector = std::vector<SGemShopItemInfo>;

class CGemShopItemGroup
{
public:
	CGemShopItemGroup(const BYTE c_bRow) : m_bRow(c_bRow) {}

	void AddItem(const DWORD dwVNum, const BYTE c_bCount, const WORD c_wPrice)
	{
		if (!dwVNum || !c_bCount)
			return;

		m_vecGemShopItem.emplace_back(SGemShopItemInfo(dwVNum, c_bCount, c_wPrice));
	}

	bool IsEmpty() const { return m_vecGemShopItem.empty(); }

	DWORD GetVNum(const BYTE c_bRow) const { return m_vecGemShopItem[c_bRow].dwVNum; }
	BYTE GetCount(const BYTE c_bRow) const { return m_vecGemShopItem[c_bRow].bCount; }
	WORD GetPrice(const BYTE c_bRow) const { return m_vecGemShopItem[c_bRow].wPrice; }

	bool Contains(const DWORD c_dwVNum) const
	{
		for (std::size_t nIndex = 0; nIndex < m_vecGemShopItem.size(); nIndex++)
		{
			if (m_vecGemShopItem[nIndex].dwVNum == c_dwVNum)
				return true;
		}
		return false;
	}

	std::size_t GetGroupSize() const { return m_vecGemShopItem.size(); }

	BYTE m_bRow;
	GemShopItemVector m_vecGemShopItem;
};
using GemShopItemGroupMap = std::map<BYTE, CGemShopItemGroup*>;
using GemShopAddSlotItemGroupMap = std::map<BYTE, BYTE>;
#	endif
#endif

#if defined(__LUCKY_BOX__)
class CLuckyBoxGroup
{
public:
	CLuckyBoxGroup(int iPrice, BYTE bMaxTryCount);

	struct SLuckyBoxItemInfo
	{
		DWORD dwVNum;
		BYTE bCount;
	};

	int GetPrice() const;
	BYTE GetMaxTryCount() const;
	void AddItem(DWORD dwBoxVNum, DWORD dwVNum, BYTE bCount);

	bool ContainsItems(DWORD dwBoxVNum) const;
	const SLuckyBoxItemInfo& GetRandomItem(DWORD dwBoxVNum) const;
	std::size_t GetItemCount(DWORD dwBoxVNum) const;

private:
	int m_iPrice;
	BYTE m_bMaxTryCount;
	std::unordered_map<DWORD, std::vector<SLuckyBoxItemInfo>> m_ItemsVec;
};
#endif

struct CSpecialAttrInfo
{
	CSpecialAttrInfo(DWORD _apply_type, DWORD _apply_value)
		: apply_type(_apply_type), apply_value(_apply_value)
	{}
	DWORD apply_type;
	DWORD apply_value;
};

typedef std::vector<CSpecialAttrInfo> SpecialAttrInfoVector;

// special_item_group.txt???? ??????? ??? ???
// type attr?? ?????? ?? ???.
// ?? ??? ????? ????? ?? ??? ???? special_item_group.txt???? Special type???? ????? ??Äî ???? UNIQUE ITEM???.
class CSpecialAttrGroup
{
public:
	CSpecialAttrGroup(DWORD vnum)
		: m_dwVnum(vnum)
	{}

#if defined(__EXTENDED_RELOAD__)
	void Clear()
	{
		if (!m_vecAttrs.empty())
			m_vecAttrs.clear();
	}
#endif

	DWORD m_dwVnum;
	std::string m_stEffectFileName;
	SpecialAttrInfoVector m_vecAttrs;
};

struct CSpecialItemInfo
{
	DWORD vnum;
	int count;
	int rare;

	CSpecialItemInfo(DWORD _vnum, int _count, int _rare)
		: vnum(_vnum), count(_count), rare(_rare)
	{}
};

typedef std::vector<CSpecialItemInfo> ItemsVector;

class CSpecialItemGroup
{
public:
	enum EGiveType
	{
		NONE,
		GOLD,
		EXP,
		MOB,
		SLOW,
		DRAIN_HP,
		POISON,
		MOB_GROUP,
		BLEEDING,
	};

	// QUEST ????? ????? ?????????? vnum.sig_use?? ????? ?? ??? ??????.
	// ??, ?? ??Äî ????? ??????? ITEM ????? TYPE?? QUEST???? ???.
	// SPECIAL ????? idx, item_vnum, attr_vnum?? ??????. attr_vnum?? ???? CSpecialAttrGroup?? Vnum???.
	// ?? ??Äî ?????? ???????? ???? ?????? ?? ????.
	enum ESIGType { NORMAL, PCT, QUEST, SPECIAL };

	CSpecialItemGroup(DWORD vnum, BYTE type = 0)
		: m_dwVnum(vnum), m_bType(type)
	{}

	void AddItem(DWORD vnum, int count, int prob, int rare)
	{
		if (!prob)
			return;

		if (!m_vecProbs.empty())
			prob += m_vecProbs.back();

		m_vecProbs.push_back(prob);
		m_vecItems.push_back(CSpecialItemInfo(vnum, count, rare));
	}

	bool IsEmpty() const
	{
		return m_vecProbs.empty();
	}

	// Type Multi, ?? m_bType == PCT ?? ???,
	// ????? ??????? ???, ?????????? ?????? ???????? ???????.
	// ???? ???? ???? ???????? ?????? ?? ???.
	// by rtsummit
	int GetMultiIndex(std::vector<int>& idx_vec) const
	{
		idx_vec.clear();
		if (m_bType == PCT)
		{
			int count = 0;
			if (number(1, 100) <= m_vecProbs[0])
			{
				idx_vec.push_back(0);
				count++;
			}
			for (uint i = 1; i < m_vecProbs.size(); i++)
			{
				if (number(1, 100) <= m_vecProbs[i] - m_vecProbs[i - 1])
				{
					idx_vec.push_back(i);
					count++;
				}
			}
			return count;
		}
		else
		{
			idx_vec.push_back(GetOneIndex());
			return 1;
		}
	}

	int GetOneIndex() const
	{
		int n = number(1, m_vecProbs.back());
		auto it = lower_bound(m_vecProbs.begin(), m_vecProbs.end(), n);
		return std::distance(m_vecProbs.begin(), it);
	}

	int GetVnum(int idx) const
	{
		return m_vecItems[idx].vnum;
	}

	int GetCount(int idx) const
	{
		return m_vecItems[idx].count;
	}

	int GetRarePct(int idx) const
	{
		return m_vecItems[idx].rare;
	}

	bool Contains(DWORD dwVnum) const
	{
		for (DWORD i = 0; i < m_vecItems.size(); i++)
		{
			if (m_vecItems[i].vnum == dwVnum)
				return true;
		}
		return false;
	}

	// Group?? Type?? Special?? ??Äî
	// dwVnum?? ?????? AttrVnum?? return?????.
	DWORD GetAttrVnum(DWORD dwVnum) const
	{
		if (CSpecialItemGroup::SPECIAL != m_bType)
			return 0;

		for (auto it = m_vecItems.begin(); it != m_vecItems.end(); it++)
		{
			if (it->vnum == dwVnum)
			{
				return it->count;
			}
		}
		return 0;
	}

	// Group?? Size?? return?????.
	int GetGroupSize() const
	{
		return m_vecProbs.size();
	}

#if defined(__EXTENDED_RELOAD__)
	void Clear()
	{
		if (!m_vecProbs.empty())
			m_vecProbs.clear();

		if (!m_vecItems.empty())
			m_vecItems.clear();
	}
#endif

	DWORD m_dwVnum;
	BYTE m_bType;
	std::vector<int> m_vecProbs;
	ItemsVector m_vecItems; // vnum, count
};

struct SMobItemGroupInfo
{
	DWORD dwItemVnum;
	int iCount;
	int iRarePct;

	SMobItemGroupInfo(DWORD dwItemVnum, int iCount, int iRarePct)
		: dwItemVnum(dwItemVnum),
		iCount(iCount),
		iRarePct(iRarePct)
	{
	}
};

typedef std::vector<int> ProbsVector;
typedef std::vector<SMobItemGroupInfo> ItemGroupInfoVector;

class CMobItemGroup
{
public:
	CMobItemGroup(DWORD dwMobVnum, int iKillDrop, const std::string& r_stName)
		:
		m_dwMobVnum(dwMobVnum),
		m_iKillDrop(iKillDrop),
		m_stName(r_stName)
	{
	}

	int GetKillPerDrop() const
	{
		return m_iKillDrop;
	}

	void AddItem(DWORD dwItemVnum, int iCount, int iPartPct, int iRarePct)
	{
		if (!m_vecProbs.empty())
			iPartPct += m_vecProbs.back();
		m_vecProbs.push_back(iPartPct);
		m_vecItems.push_back(SMobItemGroupInfo(dwItemVnum, iCount, iRarePct));
	}

	// MOB_DROP_ITEM_BUG_FIX
	bool IsEmpty() const
	{
		return m_vecProbs.empty();
	}

	int GetOneIndex() const
	{
		int n = number(1, m_vecProbs.back());
		auto it = lower_bound(m_vecProbs.begin(), m_vecProbs.end(), n);
		return std::distance(m_vecProbs.begin(), it);
	}
	// END_OF_MOB_DROP_ITEM_BUG_FIX

	const SMobItemGroupInfo& GetOne() const
	{
		return m_vecItems[GetOneIndex()];
	}

private:
	DWORD m_dwMobVnum;
	int m_iKillDrop;
	std::string m_stName;
	ProbsVector m_vecProbs;
	ItemGroupInfoVector m_vecItems;
};

struct SDropItemGroupInfo
{
	DWORD dwVnum;
	DWORD dwPct;
	int iCount;

	SDropItemGroupInfo(DWORD dwVnum, DWORD dwPct, int iCount)
		: dwVnum(dwVnum), dwPct(dwPct), iCount(iCount)
	{}
};

typedef std::vector<SDropItemGroupInfo> DropItemGroupInfoVector;

class CDropItemGroup
{
public:
	CDropItemGroup(DWORD dwVnum, DWORD dwMobVnum, const std::string& r_stName)
		:
		m_dwVnum(dwVnum),
		m_dwMobVnum(dwMobVnum),
		m_stName(r_stName)
	{
	}

	const DropItemGroupInfoVector& GetVector()
	{
		return m_vec_items;
	}

	void AddItem(DWORD dwItemVnum, DWORD dwPct, int iCount)
	{
		m_vec_items.push_back(SDropItemGroupInfo(dwItemVnum, dwPct, iCount));
	}

private:
	DWORD m_dwVnum;
	DWORD m_dwMobVnum;
	std::string m_stName;
	DropItemGroupInfoVector m_vec_items;
};

struct SLevelItemGroupInfo
{
	DWORD dwVNum;
	DWORD dwPct;
	int iCount;

	SLevelItemGroupInfo(DWORD dwVnum, DWORD dwPct, int iCount)
		: dwVNum(dwVnum), dwPct(dwPct), iCount(iCount)
	{ }
};

typedef std::vector<SLevelItemGroupInfo> LevelItemGroupInfoVector;

class CLevelItemGroup
{
public:
	CLevelItemGroup(DWORD dwLevelLimit)
		: m_dwLevelLimit(dwLevelLimit)
	{}

	DWORD GetLevelLimit() { return m_dwLevelLimit; }

	const LevelItemGroupInfoVector& GetVector()
	{
		return m_vec_items;
	}

	void AddItem(DWORD dwItemVnum, DWORD dwPct, int iCount)
	{
		m_vec_items.push_back(SLevelItemGroupInfo(dwItemVnum, dwPct, iCount));
	}

private:
	DWORD m_dwLevelLimit;
	std::string m_stName;
	LevelItemGroupInfoVector m_vec_items;
};

struct SThiefGroupInfo
{
	DWORD dwVnum;
	DWORD dwPct;
	int iCount;

	SThiefGroupInfo(DWORD dwVnum, DWORD dwPct, int iCount)
		: dwVnum(dwVnum), dwPct(dwPct), iCount(iCount)
	{}
};

typedef std::vector<SThiefGroupInfo> ItemThiefGroupInfoVector;

class CBuyerThiefGlovesItemGroup
{
public:
	CBuyerThiefGlovesItemGroup(DWORD dwVnum, DWORD dwMobVnum, const std::string& r_stName)
		:
		m_dwVnum(dwVnum),
		m_dwMobVnum(dwMobVnum),
		m_stName(r_stName)
	{
	}

	const ItemThiefGroupInfoVector& GetVector()
	{
		return m_vec_items;
	}

	void AddItem(DWORD dwItemVnum, DWORD dwPct, int iCount)
	{
		m_vec_items.push_back(SThiefGroupInfo(dwItemVnum, dwPct, iCount));
	}

private:
	DWORD m_dwVnum;
	DWORD m_dwMobVnum;
	std::string m_stName;
	ItemThiefGroupInfoVector m_vec_items;
};

class ITEM;

// temp
typedef std::map<DWORD, CMobItemGroup*> TempMobItemMap;
typedef std::map<DWORD, CDropItemGroup*> TempDropItemMap;
typedef std::map<DWORD, CLevelItemGroup*> TempLevelItemMap;
typedef std::map<DWORD, CBuyerThiefGlovesItemGroup*> TempGloveItemMap;

//
typedef std::map<DWORD, int> ItemToSpecialGroupMap;
typedef std::map<DWORD, CDropItemGroup*> pkDropItemGroupMap;
typedef std::map<DWORD, CLevelItemGroup*> pkLevelItemGroupMap;
typedef std::map<DWORD, CMobItemGroup*> pkMobItemGroupMap;
typedef std::map<DWORD, CBuyerThiefGlovesItemGroup*> pkGloveItemGroupMap;
typedef std::map<DWORD, DWORD> EtcItemDropProbMap;
typedef std::map<DWORD, DWORD> ItemRefineFromMap;
typedef std::map<DWORD, CSpecialItemGroup*> SpecialItemGroupMap;
typedef std::map<DWORD, CSpecialAttrGroup*> SpecialAttrGroupMap;
typedef std::map<DWORD, LPITEM> ItemMap;

#if defined(__SEND_TARGET_INFO__)
typedef std::unordered_map<DWORD, BYTE> MonsterItemDropMap;
#endif

class ITEM_MANAGER : public singleton<ITEM_MANAGER>
{
public:
	ITEM_MANAGER();
	virtual ~ITEM_MANAGER();

	bool Initialize(TItemTable* table, int size);
	void Destroy();
	void Update(); // ?? ???????? ?¥è???.
	void GracefulShutdown();

	DWORD GetNewID();
	bool SetMaxItemID(TItemIDRangeTable range); // ??? ???? ????? ????
	bool SetMaxSpareItemID(TItemIDRangeTable range);

	// DelayedSave: ???? ??? ?????? ?????? ??? ?? ???? ???? ??? ????
	// ?????? ??? ????????? "?????? ???" ??? ??©ª? ???? ???
	// (??: 1 frame) ?¨¨? ????????.
	void DelayedSave(LPITEM item);
	void FlushDelayedSave(LPITEM item); // Delayed ??????? ???? ????? ???????. ???? ????? ??? ??.
	void SaveSingleItem(LPITEM item);

	LPITEM CreateItem(DWORD vnum, DWORD count = 1, DWORD dwID = 0, bool bTryMagic = false, int iRarePct = -1, bool bSkipSave = false, bool bSkilAddon = false);
#ifndef DEBUG_ALLOC
	void DestroyItem(LPITEM item);
#else
	void DestroyItem(LPITEM item, const char* file, size_t line);
#endif
	void RemoveItem(LPITEM item, const char* c_pszReason = NULL); // ?????? ???? ???????? ????

	LPITEM Find(DWORD id);
	LPITEM FindByVID(DWORD vid);
	TItemTable* GetTable(DWORD vnum);
	bool GetVnum(const char* c_pszName, DWORD& r_dwVnum);
	bool GetVnumByOriginalName(const char* c_pszName, DWORD& r_dwVnum);

	bool GetDropPct(LPCHARACTER pkChr, LPCHARACTER pkKiller, OUT int& iDeltaPercent, OUT int& iRandRange);
	bool CreateDropItem(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM>& vec_item);

#if defined(__SEND_TARGET_INFO__)
	void GetMonsterItemDropMap(LPCHARACTER pkChr, LPCHARACTER pkKiller, MonsterItemDropMap& rItemDropMap, bool& bDropMetinStone);
	bool CreateDropItemVector(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM>& rVecItem);
#endif

	bool ReadCommonDropItemFile(const char* c_pszFileName);
	bool ReadEtcDropItemFile(const char* c_pszFileName);
	bool ReadDropItemGroup(const char* c_pszFileName);
	bool ReadMonsterDropItemGroup(const char* c_pszFileName);
	bool ReadSpecialDropItemFile(const char* c_pszFileName);


#if defined(__EXTENDED_RELOAD__)
	bool ReloadMobDropItemGroup(const char* c_pszFileName);
	bool ReloadSpecialItemGroup(const char* c_pszFileName);
#endif

	// convert name -> vnum special_item_group.txt
	bool ConvSpecialDropItemFile();
	// convert name -> vnum special_item_group.txt

	DWORD GetRefineFromVnum(DWORD dwVnum);

	static void CopyAllAttrTo(LPITEM pkOldItem, LPITEM pkNewItem); // pkNewItem???? ??? ????? ???? ?????? ?????? ???.

	const CSpecialItemGroup* GetSpecialItemGroup(DWORD dwVnum);
	const CSpecialAttrGroup* GetSpecialAttrGroup(DWORD dwVnum);
#if defined(ENABLE_MONSTER_CARD)
	CDropItemGroup* FindDropItemGroupByMobVnum(DWORD dwMobVnum);
#endif

	const std::vector<TItemTable>& GetTable() { return m_vec_prototype; }

	// CHECK_UNIQUE_GROUP
	int GetSpecialGroupFromItem(DWORD dwVnum) const { auto it = m_ItemToSpecialGroup.find(dwVnum); return (it == m_ItemToSpecialGroup.end()) ? 0 : it->second; }
	// END_OF_CHECK_UNIQUE_GROUP

#if defined(__GEM_SYSTEM__)
public:
	const SGemRefineInfo& GetGemRefineInfo() const { return m_GemRefineInfo; }
protected:
	SGemRefineInfo m_GemRefineInfo;

#	if defined(__GEM_SHOP__)
public:
	const SGemShopInfo& GetGemShopInfo() const { return m_GemShopInfo; }

	const CGemShopItemGroup* GetGemShopItemGroup(const BYTE c_bRow);
	std::size_t GetGemShopItemGroupSize() { return m_map_pGemShopItemGroup.size(); }
	bool ReadGemShopItemGroup(const char* c_pszFileName);

	BYTE GetGemShopAddSlotItemCount(const BYTE c_bSlotIndex) const;
protected:
	SGemShopInfo m_GemShopInfo;
	GemShopItemGroupMap m_map_pGemShopItemGroup;
	GemShopAddSlotItemGroupMap m_mapGemShopAddSlotItemGroup;
#	endif
#endif

#if defined(__SET_ITEM__)
public:
	bool LoadSetItemTable(const char* szFileName);

	using ItemSetApplyVector = std::vector<std::pair<POINT_TYPE, POINT_VALUE>>;
	using ItemSetCountMap = std::unordered_map<BYTE, ItemSetApplyVector>;

	// <SetValue, <WearCount, <<ApplyType, ApplyValue>>>>
	using ItemSetValueMap = std::unordered_map<BYTE, ItemSetCountMap>;

	// <SetValue, <<ItemType, <MinItemVnum, MaxItemVnum>>>>
	using ItemSetItemVnumVector = std::vector<std::pair<BYTE, std::tuple<DWORD, DWORD, bool>>>;
	using ItemSetItemMap = std::unordered_map<BYTE, ItemSetItemVnumVector>;

	const ItemSetValueMap& GetItemSetValueMap() const;
	const ItemSetItemMap& GetItemSetItemMap() const;

protected:
	ItemSetValueMap m_ItemSetValueMap;
	ItemSetItemMap m_ItemSetItemMap;
#endif

protected:
	int RealNumber(DWORD vnum);
	void CreateQuestDropItem(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM>& vec_item, int iDeltaPercent, int iRandRange);

protected:
	typedef std::map<DWORD, LPITEM> ITEM_VID_MAP;

	std::vector<TItemTable> m_vec_prototype;
	std::vector<TItemTable*> m_vec_item_vnum_range_info;
	std::map<DWORD, DWORD> m_map_ItemRefineFrom;
	int m_iTopOfTable;

	ITEM_VID_MAP m_VIDMap; ///< m_dwVIDCount ?? ???????? ???????? ???????.
	DWORD m_dwVIDCount; ///< ??? VID?? ???? ??? ???¥ì??? ???? ????? ?????.
	DWORD m_dwCurrentID;
	TItemIDRangeTable m_ItemIDRange;
	TItemIDRangeTable m_ItemIDSpareRange;

	std::unordered_set<LPITEM> m_set_pkItemForDelayedSave;
	ItemMap m_map_pkItemByID;

	EtcItemDropProbMap m_map_dwEtcItemDropProb;
	pkDropItemGroupMap m_map_pkDropItemGroup;
	SpecialItemGroupMap m_map_pkSpecialItemGroup;
	SpecialItemGroupMap m_map_pkQuestItemGroup;
	SpecialAttrGroupMap m_map_pkSpecialAttrGroup;
	pkMobItemGroupMap m_map_pkMobItemGroup;
	pkLevelItemGroupMap m_map_pkLevelItemGroup;
	pkGloveItemGroupMap m_map_pkGloveItemGroup;

	// CHECK_UNIQUE_GROUP
	std::map<DWORD, int> m_ItemToSpecialGroup;
	// END_OF_CHECK_UNIQUE_GROUP

private:
	// ??????? ???? ©¦?? ??????? ??????, ??? ?????? ©¦?? ???????? ?????? ???,
	// ???????? ??????? ??? ???? ?¡À???? ?????? ???¥ï? ????????? ?????,
	// ???¥ï? ?????? ?—ª?? ????????.
	// ?????? ???¥ï? ??????? ???????? ??????? ???? ????? ????????,
	// ??????, ????, vnum ??????? ??????
	// ???¥ï? vnum?? ??? ?????? ???? ?? ??????? ?????? ????? ?¢¥???.
	// ????? ?? vnum?? ?????????, ???????? ????? ???? ???????? ?????? vnum???? ??? ???? ???,
	// ?????? ???? ???? vnum???? ???????? ???.
	// ??? ???? ???????? vnum?? ???¥ï? vnum?? ?????????? ???? ????.
	typedef std::map <DWORD, DWORD> TMapDW2DW;
	TMapDW2DW m_map_new_to_ori;

public:
	DWORD GetMaskVnum(DWORD dwVnum);
	std::map<DWORD, TItemTable> m_map_vid;
	std::map<DWORD, TItemTable>& GetVIDMap() { return m_map_vid; }
	std::vector<TItemTable>& GetVecProto() { return m_vec_prototype; }

	const static int MAX_NORM_ATTR_NUM = ITEM_ATTRIBUTE_NORM_NUM;
	const static int MAX_RARE_ATTR_NUM = ITEM_ATTRIBUTE_RARE_NUM;

	bool ReadItemVnumMaskTable(const char* c_pszFileName);

#if defined(__ITEM_APPLY_RANDOM__)
public:
	bool ReadApplyRandomTableFile(const char* c_pszFileName);
	bool GetApplyRandom(BYTE bIndex, BYTE bLevel, POINT_TYPE& wApplyType, POINT_VALUE& lApplyValue, BYTE& bPath);
	POINT_VALUE GetApplyRandomValue(BYTE bIndex, BYTE bLevel, BYTE bPath, POINT_TYPE wApplyType);
private:
	CApplyRandomTable* m_pApplyRandomTable;
#endif

#if defined(__LUCKY_BOX__)
public:
	bool ReadLuckyBoxFile(const char* c_pszFileName);
	CLuckyBoxGroup* GetLuckyBoxGroup();
private:
	CLuckyBoxGroup* m_pLuckyBox;
	std::map<DWORD, std::string> m_mapLuckyBoxMapper;
#endif
};

#ifndef DEBUG_ALLOC
#define M2_DESTROY_ITEM(ptr) ITEM_MANAGER::instance().DestroyItem(ptr)
#else
#define M2_DESTROY_ITEM(ptr) ITEM_MANAGER::instance().DestroyItem(ptr, __FILE__, __LINE__)
#endif

#endif // __INC_ITEM_MANAGER_H__
