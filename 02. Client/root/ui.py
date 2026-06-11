# Add anywhere in the Button class:
	if app.ENABLE_MONSTER_CARD:
		def SetAlwaysToolTip(self, bFlag):
			wndMgr.SetAlwaysToolTip(self.hWnd, bFlag)


# Add anywhere inside the `PythonScriptLoader` class:
	if app.ENABLE_MONSTER_CARD or app.ENABLE_MYSHOP_DECO or app.ENABLE_MINI_GAME_YUTNORI:
		RENDER_TARGET_KEY_LIST = ( "index", )


# In `LoadChildren`, extend the elif-statement with:
			elif Type == "render_target":
				if app.ENABLE_MONSTER_CARD or app.ENABLE_MYSHOP_DECO or app.ENABLE_MINI_GAME_YUTNORI:
					parent.Children[Index] = RenderTarget()
					parent.Children[Index].SetParent(parent)
					self.LoadElementRenderTarget(parent.Children[Index], ElementValue, parent)


# In `LoadElementImage`, extend the if-statement with:
		if app.ENABLE_MONSTER_CARD:
			if True == value.has_key("alpha"):
				window.SetAlpha( float(value["alpha"]) )


# In `LoadElementExpandedImage`, extend the if-statement with:
		if app.ENABLE_MONSTER_CARD:
			if True == value.has_key("alpha"):
				window.SetAlpha( float(value["alpha"]) )


# Add anywhere in the PythonScriptLoader class:
	if app.ENABLE_MONSTER_CARD or app.ENABLE_MYSHOP_DECO or app.ENABLE_MINI_GAME_YUTNORI:
		def LoadElementRenderTarget(self, window, value, parentWindow):

			if False == self.CheckKeyList(value["name"], value, self.RENDER_TARGET_KEY_LIST):
				return False

			window.SetSize(value["width"], value["height"])

			if True == value.has_key("style"):
				for style in value["style"]:
					window.AddFlag(style)

			self.LoadDefaultData(window, value, parentWindow)

			if value.has_key("index"):
				window.SetRenderTarget(int(value["index"]))

			return True


# Add anywhere in the ReadingWnd class:
if app.ENABLE_MONSTER_CARD or app.ENABLE_MYSHOP_DECO or app.ENABLE_MINI_GAME_YUTNORI:
	class RenderTarget(Window):

		def __init__(self, layer = "UI"):
			Window.__init__(self, layer)

			self.number = -1

		def __del__(self):
			Window.__del__(self)

		def RegisterWindow(self, layer):
			self.hWnd = wndMgr.RegisterRenderTarget(self, layer)

		def SetRenderTarget(self, number):
			self.number = number
			wndMgr.SetRenderTarget(self.hWnd, self.number)
