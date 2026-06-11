// Add the following declaration/member block related section:
#if defined(ENABLE_MONSTER_CARD)
namespace kalisto {
	class InterfaceModel;
}
#endif

// Find this line:
void SetAlpha(float fAlpha);

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	void SetInterfaceModel(kalisto::InterfaceModel* pInterfaceModel) { m_pInterfaceModel = pInterfaceModel; }
#endif

// Find this line:
void UpdateTextTailLevel(DWORD level);

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	inline void DisableTextTail() { m_textTailDisable = true; }
#endif

// Find this line:
float GetDegreeFromDirection(int dir);

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	void EnableAlwaysRender();
	void DisableAlwaysRender();
#endif

// Find this line:
DWORD m_dwEmoticonTime;

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	kalisto::InterfaceModel* m_pInterfaceModel = nullptr;
	bool m_textTailDisable = false;
#endif
