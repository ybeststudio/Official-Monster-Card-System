# Add to the imports:
if app.ENABLE_MONSTER_CARD:
	import uiMonsterCard


# In `__init__`, extend the if-statement with:
		if app.ENABLE_MONSTER_CARD:
			self.wndMonsterCardWindow = None


# In `__MakeTaskBar`, extend the if-statement with:
			if app.ENABLE_MONSTER_CARD:
				self.wndExpandedTaskBar.SetToggleButtonEvent(uiTaskBar.ExpandedTaskBar.BUTTON_MONSTER_CARD_WINDOW, ui.__mem_func__(self.ToggleMonsterCardWindow))

# In `__MakeWindows`, extend the if-statement with:
		if app.ENABLE_MONSTER_CARD:
			self.wndMonsterCardWindow = uiMonsterCard.MonsterCardWindow()


# In `Close`, extend the if-statement with:
		if app.ENABLE_MONSTER_CARD:
			if self.wndMonsterCardWindow:
				self.wndMonsterCardWindow.Destroy()
				del self.wndMonsterCardWindow


# In `HideAllWindows`, extend the if-statement with:
		if app.ENABLE_MONSTER_CARD:
			if self.wndMonsterCardWindow:
				self.wndMonsterCardWindow.Hide()


# Add anywhere in the Interface class:
	if app.ENABLE_MONSTER_CARD:
		def ToggleMonsterCardWindow(self):
			if False == player.IsObserverMode() and self.wndMonsterCardWindow:
				if not self.wndMonsterCardWindow.IsShow():
					self.wndMonsterCardWindow.Show()
				else:
					self.wndMonsterCardWindow.Close()


# In `__HideWindows`, extend the if-statement with:
		if app.ENABLE_MONSTER_CARD:
			if self.wndMonsterCardWindow:
				hideWindows += self.wndMonsterCardWindow,


# Add anywhere in the Interface class:
	if app.ENABLE_MONSTER_CARD:
		def RefreshMissionPage(self):
			if self.wndMonsterCardWindow:
				self.wndMonsterCardWindow.RefreshMissionPage()

		def ReciveMission(self):
			if self.wndMonsterCardWindow:
				self.wndMonsterCardWindow.ReciveMission()

		def MonsterCardMissionFail(self, type, data):
			if self.wndMonsterCardWindow:
				self.wndMonsterCardWindow.MonsterCardMissionFail(type, data)

		def MonsterCardIllustrationFail(self, type, data):
			if self.wndMonsterCardWindow:
				self.wndMonsterCardWindow.MonsterCardIllustrationFail(type, data)

		def MonsterCardIllustrationRefresh(self):
			if self.wndMonsterCardWindow:
				self.wndMonsterCardWindow.MonsterCardIllustrationRefresh()

		def MonsterCardAchievRefresh(self):
			if self.wndMonsterCardWindow:
				self.wndMonsterCardWindow.MonsterCardAchievRefresh()
