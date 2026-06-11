// Find this line:
DEFAULT_ORTHO_CAMERA,

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
		DEFAULT_ILLUSTRATED_CAMERA,
#endif
