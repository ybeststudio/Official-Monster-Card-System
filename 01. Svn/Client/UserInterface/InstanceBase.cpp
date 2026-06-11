// Find this line:
m_dwEmoticonTime = 0;

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	m_pInterfaceModel = nullptr;
	m_textTailDisable = false;
#endif

// Add the following `CInstanceBase::EnableAlwaysRender` function anywhere in this file:
#if defined(ENABLE_MONSTER_CARD)
void CInstanceBase::EnableAlwaysRender()
{
#if defined(RENDER_TARGET)
	SetAlwaysRender(true);
#endif
}

void CInstanceBase::DisableAlwaysRender()
{
#if defined(RENDER_TARGET)
	SetAlwaysRender(false);
#endif
}
#endif
