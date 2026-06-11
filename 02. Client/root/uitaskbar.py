# Add anywhere inside the `ExpandedTaskBar` class:
	if app.ENABLE_MONSTER_CARD:
		BUTTON_MONSTER_CARD_WINDOW = 3


# In `LoadWindow`, extend the if-statement with:
		if app.ENABLE_MONSTER_CARD:
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_MONSTER_CARD_WINDOW] = self.GetChild("MonsterCardWindow")
			self.toggleButtonDict[ExpandedTaskBar.BUTTON_MONSTER_CARD_WINDOW].SetParent(self)


# In `RePositionButton`, extend the if-statement with:
		if app.ENABLE_MONSTER_CARD:
			if not ExpandedTaskBar.BUTTON_MONSTER_CARD_WINDOW in self.exclusion_list:
				button_order.append( ExpandedTaskBar.BUTTON_MONSTER_CARD_WINDOW )
