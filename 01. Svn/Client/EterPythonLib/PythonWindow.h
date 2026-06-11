// Add the following declaration/member block related section:
#if defined(ENABLE_MONSTER_CARD)
namespace kalisto {
	class InterfaceModelRenderTarget;
}
#endif

// Find this line:
void __RemoveReserveChildren();

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
		inline void SetModelParent(kalisto::InterfaceModelRenderTarget* ptr) { m_pModelInstance = ptr; }
#endif

// Find this line:
TWindowContainer m_pChildList;

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
		kalisto::InterfaceModelRenderTarget* m_pModelInstance = nullptr;
#endif
