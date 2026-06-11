// Find this line:
AddCamera(DEFAULT_ORTHO_CAMERA);

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	AddCamera(DEFAULT_ILLUSTRATED_CAMERA);
#endif
