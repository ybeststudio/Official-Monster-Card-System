// Find this line:
class CPetSystem;

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
#	include <memory>
namespace kalisto {
	class MonstercardSystem;
}
#endif

// Find this line:
BYTE GetCharType() const;

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	inline const std::unique_ptr<kalisto::MonstercardSystem>& GetMonsterCardSystem() const noexcept { return m_pMonsterCardSystem; }
	inline bool IsMonstercardTarget() const noexcept { return m_isMonsterCardTarget; }
#endif

// Find this line:
);

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	int CountSpecifyItemBySocket(DWORD vnum, int socketIndex, int socketValue, int iExceptionCell = -1) const;
	void RemoveSpecifyItemBySocket(DWORD vnum, DWORD count, int socketIndex, int socketValue, int iExceptionCell = -1);
#endif

// Find this line:
DWORD m_dwRefineNPCVID;

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	bool m_isMonsterCardTarget;
	std::unique_ptr<kalisto::MonstercardSystem> m_pMonsterCardSystem;
#endif
