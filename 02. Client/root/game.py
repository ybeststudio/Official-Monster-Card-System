# In `Open`, extend the elif-statement with:
		if app.ENABLE_MONSTER_CARD:
			app.IllustratedCreate()
			net.SendIllustrationMessage( net.REQUEST_ILLUSTRATION )


# In `OpenWindow`, extend the if-statement with:
			if app.ENABLE_MONSTER_CARD:
				if type == player.KEY_MONSTER_CARD:
					self.interface.ToggleMonsterCardWindow()

# Add anywhere in the GameWindow class:
	if app.ENABLE_MONSTER_CARD:
		def RefreshMissionPage(self):
			self.interface.RefreshMissionPage()

		def ReciveMission(self):
			self.interface.ReciveMission()

		def MonsterCardMissionFail(self, type, data):
			self.interface.MonsterCardMissionFail(type, data)

		def MonsterCardIllustrationFail(self, type, data):
			self.interface.MonsterCardIllustrationFail(type, data)

		def MonsterCardIllustrationRefresh(self):
			self.interface.MonsterCardIllustrationRefresh()

		def MonsterCardAchievRefresh(self):
			self.interface.MonsterCardAchievRefresh()
