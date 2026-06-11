# Before
		if not _AtlasInterfaceTip(localeKey):
			return
		self.__EnsureAtlasBarButtonTooltip()
		except Exception:
			pass
		try:
			button.SetShowToolTipEvent(ui.__mem_func__(self.__OnAtlasBarButtonTooltipOverIn), button, localeKey)
			button.SetHideToolTipEvent(ui.__mem_func__(self.__OnAtlasBarButtonTooltipOverOut))
		except Exception:
			pass

	def __ClearThinToolTipLines(self, tip):
		if not tip:
			return
		for child in tip.childrenList:
			try:
				child.Hide()
			except Exception:
				pass
		tip.ClearToolTip()

	def __PositionAtlasBarButtonTooltip(self, tip):
		# Official style: show a couple of text lines under mouse (not under the button).
		try:
			mx, my = wndMgr.GetMousePosition()
		except Exception:
			return

		try:
			# ToolTip.OnUpdate places top at (yPos - height). To show below cursor,
			# pass yPos as (cursorY + offset + height).
			offsetY = 34
			height = getattr(tip, "toolTipHeight", tip.GetHeight())
			if height <= 0:
				height = 24
			tip.SetFollow(True)
			tip.SetToolTipPosition(mx, my + offsetY + height)
			tip.OnUpdate()
		except Exception:
			pass

	def __OnAtlasBarButtonTooltipOverIn(self, btn, localeKey):
		if not self._atlasBarButtonTooltip:
			return
		text = _AtlasInterfaceTip(localeKey)
		if not text:
			return
		try:
			tip = self._atlasBarButtonTooltip
			self.__ClearThinToolTipLines(tip)
			probe = ui.TextLine()
			try:
				probe.SetFontName(tip.defFontName)
			except Exception:
				pass
			probe.SetText(text)
			tw, th = probe.GetTextSize()
			if tw < 1:
				tw = len(text) * 6
			boardW = max(60, tw + 20)
			tip.SetThinBoardSize(boardW, 12)
			tip.AppendTextLine(text, uiToolTip.ToolTip.FONT_COLOR, True)
			tip.ResizeToolTip()
			self.__PositionAtlasBarButtonTooltip(tip)
			tip.Show()
			tip.SetTop()
			self._atlasBarButtonTooltipVisible = True
		except Exception:
			pass

	def __OnAtlasBarButtonTooltipOverOut(self):
		if self._atlasBarButtonTooltip:
			try:
				self._atlasBarButtonTooltip.HideToolTip()
			except Exception:
				try:
					self._atlasBarButtonTooltip.Hide()
				except Exception:
					pass
		self._atlasBarButtonTooltipVisible = False

	def __AssignPatternBtnChangeImages(self, button):
		pathUp = PATTERN_BTN_CHANGE_PATH + "01.tga"
		if not app.IsExistFile(pathUp):
			return False
		try:
			button.SetUpVisual(pathUp)
			button.SetOverVisual(PATTERN_BTN_CHANGE_PATH + "02.tga")
			button.SetDownVisual(PATTERN_BTN_CHANGE_PATH + "03.tga")
			return True
		except Exception:
			return False

	def __LoadTitleBarExpandBtnImage(self, state):
		if not self.titleBarExpandBtn:
			return False

		stateMap = {
			"default": "default.tga",
			"over": "over.tga",
			"down": "down.tga",
		}

# After
		if not _AtlasInterfaceTip(localeKey):
			return
		self.__EnsureAtlasBarButtonTooltip()
		try:
			if app.ENABLE_MONSTER_CARD:
				button.SetAlwaysToolTip(True)
		except Exception:
			pass
		try:
			button.SetShowToolTipEvent(ui.__mem_func__(self.__OnAtlasBarButtonTooltipOverIn), button, localeKey)
			button.SetHideToolTipEvent(ui.__mem_func__(self.__OnAtlasBarButtonTooltipOverOut))
		except Exception:
			pass

	def __ClearThinToolTipLines(self, tip):
		if not tip:
			return
		for child in tip.childrenList:
			try:
				child.Hide()
			except Exception:
				pass
		tip.ClearToolTip()

	def __PositionAtlasBarButtonTooltip(self, tip):
		# Official style: show a couple of text lines under mouse (not under the button).
		try:
			mx, my = wndMgr.GetMousePosition()
		except Exception:
			return

		try:
			# ToolTip.OnUpdate places top at (yPos - height). To show below cursor,
			# pass yPos as (cursorY + offset + height).
			offsetY = 34
			height = getattr(tip, "toolTipHeight", tip.GetHeight())
			if height <= 0:
				height = 24
			tip.SetFollow(True)
			tip.SetToolTipPosition(mx, my + offsetY + height)
			tip.OnUpdate()
		except Exception:
			pass

	def __OnAtlasBarButtonTooltipOverIn(self, btn, localeKey):
		if not self._atlasBarButtonTooltip:
			return
		text = _AtlasInterfaceTip(localeKey)
		if not text:
			return
		try:
			tip = self._atlasBarButtonTooltip
			self.__ClearThinToolTipLines(tip)
			probe = ui.TextLine()
			try:
				probe.SetFontName(tip.defFontName)
			except Exception:
				pass
			probe.SetText(text)
			tw, th = probe.GetTextSize()
			if tw < 1:
				tw = len(text) * 6
			boardW = max(60, tw + 20)
			tip.SetThinBoardSize(boardW, 12)
			tip.AppendTextLine(text, uiToolTip.ToolTip.FONT_COLOR, True)
			tip.ResizeToolTip()
			self.__PositionAtlasBarButtonTooltip(tip)
			tip.Show()
			tip.SetTop()
			self._atlasBarButtonTooltipVisible = True
		except Exception:
			pass

	def __OnAtlasBarButtonTooltipOverOut(self):
		if self._atlasBarButtonTooltip:
			try:
				self._atlasBarButtonTooltip.HideToolTip()
			except Exception:
				try:
					self._atlasBarButtonTooltip.Hide()
				except Exception:
					pass
		self._atlasBarButtonTooltipVisible = False

	def __AssignPatternBtnChangeImages(self, button):
		pathUp = PATTERN_BTN_CHANGE_PATH + "01.tga"
		if not app.IsExistFile(pathUp):
			return False
		try:
			button.SetUpVisual(pathUp)
			button.SetOverVisual(PATTERN_BTN_CHANGE_PATH + "02.tga")
			button.SetDownVisual(PATTERN_BTN_CHANGE_PATH + "03.tga")
			return True
		except Exception:
			return False

	def __LoadTitleBarExpandBtnImage(self, state):
		if not self.titleBarExpandBtn:
			return False

		stateMap = {
			"default": "default.tga",
			"over": "over.tga",
			"down": "down.tga",
		}
