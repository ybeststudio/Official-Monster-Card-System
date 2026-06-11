// Add to includes:
#if defined(ENABLE_MONSTER_CARD)
#	include "InterfaceModelRenderer.h"
#endif

// Before
	if (!PERF_CHECKER_RENDER_GAME)
	{
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().RenderBackground();
#endif

		float fAspect = m_kWndMgr.GetAspect();
		float fFarClip = m_pyBackground.GetFarClip();

#if defined(ENABLE_FOV_OPTION)
		m_pyGraphic.SetPerspective(CPythonSystem::Instance().GetFOV(), fAspect, 100.0, fFarClip);
#else
		m_pyGraphic.SetPerspective(30.0f, fAspect, 100.0, fFarClip);
#endif

		CCullingManager::Instance().Process();

		m_kChrMgr.Deform();
		//m_kEftMgr.Update();

#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().DeformModel();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().DeformModel();
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
		CPythonYutnoriManager::Instance().DeformModel();
#endif

		m_pyBackground.RenderCharacterShadowToTexture();

		m_pyGraphic.SetGameRenderState();
		m_pyGraphic.PushState();

		{
			long lx, ly;
			m_kWndMgr.GetMousePosition(lx, ly);
			m_pyGraphic.SetCursorPosition(lx, ly);
		}

		m_pyBackground.RenderSky();

		m_pyBackground.RenderBeforeLensFlare();

		m_pyBackground.RenderCloud();

		m_pyBackground.BeginEnvironment();
		m_pyBackground.Render();

		m_pyBackground.SetCharacterDirLight();

#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().RenderModel();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().RenderModel();
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
		CPythonYutnoriManager::Instance().RenderModel();
#endif

		m_kChrMgr.Render();

		m_pyBackground.SetBackgroundDirLight();
		m_pyBackground.RenderWater();
		m_pyBackground.RenderSnow();
		m_pyBackground.RenderEffect();

		m_pyBackground.EndEnvironment();

		m_kEftMgr.Render();
		m_pyItem.Render();
		m_FlyingManager.Render();

		m_pyBackground.BeginEnvironment();
		m_pyBackground.RenderPCBlocker();
		m_pyBackground.EndEnvironment();

		m_pyBackground.RenderAfterLensFlare();

		return;
	}

// After
	if (!PERF_CHECKER_RENDER_GAME)
	{
#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().RenderBackground();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().RenderBackground();
#endif

		float fAspect = m_kWndMgr.GetAspect();
		float fFarClip = m_pyBackground.GetFarClip();

#if defined(ENABLE_FOV_OPTION)
		m_pyGraphic.SetPerspective(CPythonSystem::Instance().GetFOV(), fAspect, 100.0, fFarClip);
#else
		m_pyGraphic.SetPerspective(30.0f, fAspect, 100.0, fFarClip);
#endif

		CCullingManager::Instance().Process();

		m_kChrMgr.Deform();
		//m_kEftMgr.Update();

#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().DeformModel();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().DeformModel();
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
		CPythonYutnoriManager::Instance().DeformModel();
#endif

		m_pyBackground.RenderCharacterShadowToTexture();

		m_pyGraphic.SetGameRenderState();
		m_pyGraphic.PushState();

		{
			long lx, ly;
			m_kWndMgr.GetMousePosition(lx, ly);
			m_pyGraphic.SetCursorPosition(lx, ly);
		}

		m_pyBackground.RenderSky();

		m_pyBackground.RenderBeforeLensFlare();

		m_pyBackground.RenderCloud();

		m_pyBackground.BeginEnvironment();
		m_pyBackground.Render();

		m_pyBackground.SetCharacterDirLight();

#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().RenderModel();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().RenderModel();
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
		CPythonYutnoriManager::Instance().RenderModel();
#endif

		m_kChrMgr.Render();

		m_pyBackground.SetBackgroundDirLight();
		m_pyBackground.RenderWater();
		m_pyBackground.RenderSnow();
		m_pyBackground.RenderEffect();

		m_pyBackground.EndEnvironment();

		m_kEftMgr.Render();
		m_pyItem.Render();
		m_FlyingManager.Render();

		m_pyBackground.BeginEnvironment();
		m_pyBackground.RenderPCBlocker();
		m_pyBackground.EndEnvironment();

		m_pyBackground.RenderAfterLensFlare();

		return;
	}

// Before
	if (!PERF_CHECKER_RENDER_GAME)
	{
#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().RenderBackground();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().RenderBackground();
#endif

		float fAspect = m_kWndMgr.GetAspect();
		float fFarClip = m_pyBackground.GetFarClip();

#if defined(ENABLE_FOV_OPTION)
		m_pyGraphic.SetPerspective(CPythonSystem::Instance().GetFOV(), fAspect, 100.0, fFarClip);
#else
		m_pyGraphic.SetPerspective(30.0f, fAspect, 100.0, fFarClip);
#endif

		CCullingManager::Instance().Process();

		m_kChrMgr.Deform();
		//m_kEftMgr.Update();

#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().DeformModel();
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
		CPythonYutnoriManager::Instance().DeformModel();
#endif

		m_pyBackground.RenderCharacterShadowToTexture();

		m_pyGraphic.SetGameRenderState();
		m_pyGraphic.PushState();

		{
			long lx, ly;
			m_kWndMgr.GetMousePosition(lx, ly);
			m_pyGraphic.SetCursorPosition(lx, ly);
		}

		m_pyBackground.RenderSky();

		m_pyBackground.RenderBeforeLensFlare();

		m_pyBackground.RenderCloud();

		m_pyBackground.BeginEnvironment();
		m_pyBackground.Render();

		m_pyBackground.SetCharacterDirLight();

#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().RenderModel();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().RenderModel();
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
		CPythonYutnoriManager::Instance().RenderModel();
#endif

		m_kChrMgr.Render();

		m_pyBackground.SetBackgroundDirLight();
		m_pyBackground.RenderWater();
		m_pyBackground.RenderSnow();
		m_pyBackground.RenderEffect();

		m_pyBackground.EndEnvironment();

		m_kEftMgr.Render();
		m_pyItem.Render();
		m_FlyingManager.Render();

		m_pyBackground.BeginEnvironment();
		m_pyBackground.RenderPCBlocker();
		m_pyBackground.EndEnvironment();

		m_pyBackground.RenderAfterLensFlare();

		return;
	}

// After
	if (!PERF_CHECKER_RENDER_GAME)
	{
#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().RenderBackground();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().RenderBackground();
#endif

		float fAspect = m_kWndMgr.GetAspect();
		float fFarClip = m_pyBackground.GetFarClip();

#if defined(ENABLE_FOV_OPTION)
		m_pyGraphic.SetPerspective(CPythonSystem::Instance().GetFOV(), fAspect, 100.0, fFarClip);
#else
		m_pyGraphic.SetPerspective(30.0f, fAspect, 100.0, fFarClip);
#endif

		CCullingManager::Instance().Process();

		m_kChrMgr.Deform();
		//m_kEftMgr.Update();

#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().DeformModel();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().DeformModel();
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
		CPythonYutnoriManager::Instance().DeformModel();
#endif

		m_pyBackground.RenderCharacterShadowToTexture();

		m_pyGraphic.SetGameRenderState();
		m_pyGraphic.PushState();

		{
			long lx, ly;
			m_kWndMgr.GetMousePosition(lx, ly);
			m_pyGraphic.SetCursorPosition(lx, ly);
		}

		m_pyBackground.RenderSky();

		m_pyBackground.RenderBeforeLensFlare();

		m_pyBackground.RenderCloud();

		m_pyBackground.BeginEnvironment();
		m_pyBackground.Render();

		m_pyBackground.SetCharacterDirLight();

#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().RenderModel();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().RenderModel();
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
		CPythonYutnoriManager::Instance().RenderModel();
#endif

		m_kChrMgr.Render();

		m_pyBackground.SetBackgroundDirLight();
		m_pyBackground.RenderWater();
		m_pyBackground.RenderSnow();
		m_pyBackground.RenderEffect();

		m_pyBackground.EndEnvironment();

		m_kEftMgr.Render();
		m_pyItem.Render();
		m_FlyingManager.Render();

		m_pyBackground.BeginEnvironment();
		m_pyBackground.RenderPCBlocker();
		m_pyBackground.EndEnvironment();

		m_pyBackground.RenderAfterLensFlare();

		return;
	}

// Before
	if (!PERF_CHECKER_RENDER_GAME)
	{
#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().RenderBackground();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().RenderBackground();
#endif

		float fAspect = m_kWndMgr.GetAspect();
		float fFarClip = m_pyBackground.GetFarClip();

#if defined(ENABLE_FOV_OPTION)
		m_pyGraphic.SetPerspective(CPythonSystem::Instance().GetFOV(), fAspect, 100.0, fFarClip);
#else
		m_pyGraphic.SetPerspective(30.0f, fAspect, 100.0, fFarClip);
#endif

		CCullingManager::Instance().Process();

		m_kChrMgr.Deform();
		//m_kEftMgr.Update();

#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().DeformModel();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().DeformModel();
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
		CPythonYutnoriManager::Instance().DeformModel();
#endif

		m_pyBackground.RenderCharacterShadowToTexture();

		m_pyGraphic.SetGameRenderState();
		m_pyGraphic.PushState();

		{
			long lx, ly;
			m_kWndMgr.GetMousePosition(lx, ly);
			m_pyGraphic.SetCursorPosition(lx, ly);
		}

		m_pyBackground.RenderSky();

		m_pyBackground.RenderBeforeLensFlare();

		m_pyBackground.RenderCloud();

		m_pyBackground.BeginEnvironment();
		m_pyBackground.Render();

		m_pyBackground.SetCharacterDirLight();

#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().RenderModel();
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
		CPythonYutnoriManager::Instance().RenderModel();
#endif

		m_kChrMgr.Render();

		m_pyBackground.SetBackgroundDirLight();
		m_pyBackground.RenderWater();
		m_pyBackground.RenderSnow();
		m_pyBackground.RenderEffect();

		m_pyBackground.EndEnvironment();

		m_kEftMgr.Render();
		m_pyItem.Render();
		m_FlyingManager.Render();

		m_pyBackground.BeginEnvironment();
		m_pyBackground.RenderPCBlocker();
		m_pyBackground.EndEnvironment();

		m_pyBackground.RenderAfterLensFlare();

		return;
	}

// After
	if (!PERF_CHECKER_RENDER_GAME)
	{
#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().RenderBackground();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().RenderBackground();
#endif

		float fAspect = m_kWndMgr.GetAspect();
		float fFarClip = m_pyBackground.GetFarClip();

#if defined(ENABLE_FOV_OPTION)
		m_pyGraphic.SetPerspective(CPythonSystem::Instance().GetFOV(), fAspect, 100.0, fFarClip);
#else
		m_pyGraphic.SetPerspective(30.0f, fAspect, 100.0, fFarClip);
#endif

		CCullingManager::Instance().Process();

		m_kChrMgr.Deform();
		//m_kEftMgr.Update();

#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().DeformModel();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().DeformModel();
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
		CPythonYutnoriManager::Instance().DeformModel();
#endif

		m_pyBackground.RenderCharacterShadowToTexture();

		m_pyGraphic.SetGameRenderState();
		m_pyGraphic.PushState();

		{
			long lx, ly;
			m_kWndMgr.GetMousePosition(lx, ly);
			m_pyGraphic.SetCursorPosition(lx, ly);
		}

		m_pyBackground.RenderSky();

		m_pyBackground.RenderBeforeLensFlare();

		m_pyBackground.RenderCloud();

		m_pyBackground.BeginEnvironment();
		m_pyBackground.Render();

		m_pyBackground.SetCharacterDirLight();

#if defined(ENABLE_MONSTER_CARD)
		CPythonIllustratedManager::Instance().RenderModel();
#endif
#if defined(ENABLE_MYSHOP_DECO)
		CPythonMyShopDecoManager::Instance().RenderModel();
#endif
#if defined(ENABLE_MINI_GAME_YUTNORI)
		CPythonYutnoriManager::Instance().RenderModel();
#endif

		m_kChrMgr.Render();

		m_pyBackground.SetBackgroundDirLight();
		m_pyBackground.RenderWater();
		m_pyBackground.RenderSnow();
		m_pyBackground.RenderEffect();

		m_pyBackground.EndEnvironment();

		m_kEftMgr.Render();
		m_pyItem.Render();
		m_FlyingManager.Render();

		m_pyBackground.BeginEnvironment();
		m_pyBackground.RenderPCBlocker();
		m_pyBackground.EndEnvironment();

		m_pyBackground.RenderAfterLensFlare();

		return;
	}

// Find this line:
s.BuildViewFrustum();

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	CPythonIllustratedManager::Instance().UpdateModel();
#endif

// Find this line:
OnMouseRender();

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
				kalisto::InterfaceModelRenderer::RenderAll();
#endif

// Before
	if (FindWindow(NULL, c_szName))
		bAnotherWindow = true;

	m_dwWidth = width;
	m_dwHeight = height;

	// Window
	UINT WindowMode = __GetWindowMode(Windowed ? true : false);


	if (!CMSWindow::Create(c_szName, 4, 0, WindowMode, ::LoadIcon(GetInstance(), MAKEINTRESOURCE(IDI_METIN2)), IDC_CURSOR_NORMAL))
	{
		//PyErr_SetString(PyExc_RuntimeError, "CMSWindow::Create failed");
		TraceError("CMSWindow::Create failed");
		SET_EXCEPTION(CREATE_WINDOW);
		return false;
	}

// After
	if (FindWindow(NULL, c_szName))
		bAnotherWindow = true;

	m_dwWidth = width;
	m_dwHeight = height;

	// Window
	UINT WindowMode = __GetWindowMode(Windowed ? true : false);

#if defined(ENABLE_MONSTER_CARD)
	kalisto::InterfaceModelRenderer::Initialize(width, height);
#endif

	if (!CMSWindow::Create(c_szName, 4, 0, WindowMode, ::LoadIcon(GetInstance(), MAKEINTRESOURCE(IDI_METIN2)), IDC_CURSOR_NORMAL))
	{
		//PyErr_SetString(PyExc_RuntimeError, "CMSWindow::Create failed");
		TraceError("CMSWindow::Create failed");
		SET_EXCEPTION(CREATE_WINDOW);
		return false;
	}

// Before
	if (!m_grpDevice.IsFastTNL())
		CGrannyLODController::SetMinLODMode(true);

	m_pyItem.Create();

	// Other Modules
	DefaultFont_Startup();

	CPythonIME::Instance().Create(GetWindowHandle());
	CPythonIME::Instance().SetText("", 0);
	CPythonTextTail::Instance().Initialize();

	// Light Manager
	m_LightManager.Initialize();

	CGraphicImageInstance::CreateSystem(32);

#if defined(RENDER_TARGET) 

#if defined(ENABLE_MYSHOP_DECO)
	if (!CRenderTargetManager::Instance().CreateA8R8G8B8Texture(m_dwWidth, m_dwHeight, CRenderTargetManager::RENDER_TARGET_INDEX_MYSHOPDECO))
		return false;
#endif

#if defined(ENABLE_MINI_GAME_YUTNORI)
	if (!CRenderTargetManager::Instance().CreateA8R8G8B8Texture(m_dwWidth, m_dwHeight, CRenderTargetManager::RENDER_TARGET_INDEX_YUTNORI))
		return false;
#endif
#endif

	// Backup
	STICKYKEYS sStickKeys;
	memset(&sStickKeys, 0, sizeof(sStickKeys));
	sStickKeys.cbSize = sizeof(sStickKeys);
	SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(sStickKeys), &sStickKeys, 0);
	m_dwStickyKeysFlag = sStickKeys.dwFlags;

	// Settings
	sStickKeys.dwFlags &= ~(SKF_AVAILABLE | SKF_HOTKEYACTIVE);
	SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(sStickKeys), &sStickKeys, 0);

	// SphereMap
	CGrannyMaterial::CreateSphereMap(0, "d:/ymir work/special/spheremap.jpg");
	CGrannyMaterial::CreateSphereMap(1, "d:/ymir work/special/spheremap01.jpg");

#if defined(ENABLE_LOADING_PERFORMANCE)
	// Load Game Data
	m_pyLoading.BeginThreadLoading();
#endif
	return true;
}

void CPythonApplication::SetGlobalCenterPosition(LONG x, LONG y)
{

// After
	if (!m_grpDevice.IsFastTNL())
		CGrannyLODController::SetMinLODMode(true);

	m_pyItem.Create();

	// Other Modules
	DefaultFont_Startup();

	CPythonIME::Instance().Create(GetWindowHandle());
	CPythonIME::Instance().SetText("", 0);
	CPythonTextTail::Instance().Initialize();

	// Light Manager
	m_LightManager.Initialize();

	CGraphicImageInstance::CreateSystem(32);

#if defined(RENDER_TARGET) 
#if defined(ENABLE_MONSTER_CARD)
	if (!CRenderTargetManager::Instance().CreateX8R8G8B8Texture(m_dwWidth, m_dwHeight))
		return false;
#endif

#if defined(ENABLE_MYSHOP_DECO)
	if (!CRenderTargetManager::Instance().CreateA8R8G8B8Texture(m_dwWidth, m_dwHeight, CRenderTargetManager::RENDER_TARGET_INDEX_MYSHOPDECO))
		return false;
#endif

#if defined(ENABLE_MINI_GAME_YUTNORI)
	if (!CRenderTargetManager::Instance().CreateA8R8G8B8Texture(m_dwWidth, m_dwHeight, CRenderTargetManager::RENDER_TARGET_INDEX_YUTNORI))
		return false;
#endif
#endif

	// Backup
	STICKYKEYS sStickKeys;
	memset(&sStickKeys, 0, sizeof(sStickKeys));
	sStickKeys.cbSize = sizeof(sStickKeys);
	SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(sStickKeys), &sStickKeys, 0);
	m_dwStickyKeysFlag = sStickKeys.dwFlags;

	// Settings
	sStickKeys.dwFlags &= ~(SKF_AVAILABLE | SKF_HOTKEYACTIVE);
	SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(sStickKeys), &sStickKeys, 0);

	// SphereMap
	CGrannyMaterial::CreateSphereMap(0, "d:/ymir work/special/spheremap.jpg");
	CGrannyMaterial::CreateSphereMap(1, "d:/ymir work/special/spheremap01.jpg");

#if defined(ENABLE_LOADING_PERFORMANCE)
	// Load Game Data
	m_pyLoading.BeginThreadLoading();
#endif
	return true;
}

void CPythonApplication::SetGlobalCenterPosition(LONG x, LONG y)
{

// Find this line:
m_kRenderTargetManager.Destroy();

// Add after it:
#if defined(ENABLE_MONSTER_CARD)
	m_pyIllustratedManager.Destroy();
#endif

// Add the following `CPythonApplication::IllustratedCreate` function anywhere in this file:
#if defined(ENABLE_MONSTER_CARD)
void CPythonApplication::IllustratedCreate()
{
	CPythonIllustratedManager::Instance().CreateBackground(m_dwWidth, m_dwHeight);
}
#endif
