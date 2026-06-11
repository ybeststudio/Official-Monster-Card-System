#pragma once

#if defined(ENABLE_MONSTER_CARD)

#include "StdAfx.h"

namespace kalisto {

class InterfaceModelRenderer
{
public:
	static void Initialize(DWORD /*width*/, DWORD /*height*/) {}
	static void Destroy() {}
	static void RenderAll() {}
};

} // namespace kalisto

#endif // ENABLE_MONSTER_CARD

