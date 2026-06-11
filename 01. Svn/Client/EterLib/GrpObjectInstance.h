// Find this line:
void Transform();

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	void HideAndSetToZero();
#endif
