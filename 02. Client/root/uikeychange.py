# In `__init__`, extend the if-statement with:
		if app.ENABLE_MONSTER_CARD:
			self.KeySlotMax = self.KeySlotMax + 1

# In `LoadKeyInfo`, extend the loop with:
			if count-1 != self.KeySlotMax:
				if app.ENABLE_GROWTH_PET_SYSTEM:
					if not self.KeyUiInfoDick[64]:
						self.KeyUiInfoDick[66] = app.DIK_P
				if app.ENABLE_AUTO_SYSTEM:
					if not self.KeyUiInfoDick[65]:
						self.KeyUiInfoDick[67] = app.DIK_K
				if app.ENABLE_MONSTER_CARD:
					if not self.KeyUiInfoDick[66]:
						self.KeyUiInfoDick[68] = app.DIK_J

				if app.ENABLE_PARTY_MATCH:
					if not self.KeyUiInfoDick[69]:
						self.KeyUiInfoDick[69] = app.DIK_J + app.DIK_LSHIFT + self.ADDKEYBUFFERSHIFT

				if app.ENABLE_DSS_KEY_SELECT:
					if not self.KeyUiInfoDick[70]:
						self.KeyUiInfoDick[70] = app.DIK_C + app.DIK_LCONTROL + self.ADDKEYBUFFERCONTROL
					if not self.KeyUiInfoDick[71]:
						self.KeyUiInfoDick[71] = app.DIK_V + app.DIK_LCONTROL + self.ADDKEYBUFFERCONTROL

				if app.ENABLE_EVENT_BANNER:
					if not self.KeyUiInfoDick[72]:
						self.KeyUiInfoDick[72] = app.DIK_E + app.DIK_LCONTROL + self.ADDKEYBUFFERCONTROL


# In `__BuildKeyInfo`, extend the if-statement with:
		if app.ENABLE_MONSTER_CARD:
			KeyUiInfoDick[68] = app.DIK_J

# In `__BuildKeyFunction`, extend the if-statement with:
		if app.ENABLE_MONSTER_CARD:
			KeyFunctionInfo[68] = player.KEY_MONSTER_CARD #lambda : self.interface.ToggleMonsterCardWindow
