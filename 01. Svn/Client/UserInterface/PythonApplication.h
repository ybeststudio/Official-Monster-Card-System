// Add the following declaration/member block related section:
#if defined(ENABLE_MONSTER_CARD)
	#include "PythonIllustratedManager.h"
#endif

// Find this line:
void SetUpDirCameraSpeed(float fSpeed);

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	void IllustratedCreate();
	void SetIllustratedCameraPosition();
#endif

// Find this line:
CRenderTargetManager m_kRenderTargetManager;

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	CPythonIllustratedManager m_pyIllustratedManager;
#endif
