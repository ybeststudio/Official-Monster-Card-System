// Find this line:
void initpack();

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
namespace kalisto {
	void initInterfaceModleModule();
}
#endif
