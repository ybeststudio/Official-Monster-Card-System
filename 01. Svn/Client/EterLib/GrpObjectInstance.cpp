// Add the following `CGraphicObjectInstance::HideAndSetToZero` function anywhere in this file:
#if defined(ENABLE_MONSTER_CARD)
void CGraphicObjectInstance::HideAndSetToZero()
{
	Hide();
	SetScale(0.0f, 0.0f, 0.0f);
}
#endif
