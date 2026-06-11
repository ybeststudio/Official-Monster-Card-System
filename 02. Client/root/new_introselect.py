# Add anywhere in the CharacterRenderer class:
	if app.ENABLE_MONSTER_CARD:
		def SetIllustrationInit(self):
			import player
			player.SetIllustrationDataLoad(False)
