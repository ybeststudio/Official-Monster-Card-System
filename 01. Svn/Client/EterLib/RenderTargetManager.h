// Find this line:
enum ERENDERTARGETINDEX

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
		RENDER_TARGET_INDEX_ILLUSTRATED,
#endif
