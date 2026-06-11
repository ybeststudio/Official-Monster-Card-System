# In `__UseItem`, extend the if-statement with:
		if app.ENABLE_MONSTER_CARD:
			if constInfo.IS_MONSTER_CARD_COLLECTION_ITEM(ItemVNum):
				self.questionDialog = uiCommon.QuestionDialog()
				self.questionDialog.SetText(localeInfo.MC_USE_ITEM_QUESTION)
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemMonsterCardOnAccept))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemMonsterCardOnCancel))
				self.questionDialog.Open()
				self.questionDialog.slotIndex = slotIndex
				self.questionDialog.slotWindow= slotWindow
				constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
				return


# Add anywhere in the InventoryWindow class:
	if app.ENABLE_MONSTER_CARD:
		def __UseItemMonsterCardOnAccept(self):
			self.__SendUseItemPacket(self.questionDialog.slotIndex, self.questionDialog.slotWindow)
			self.OnCloseQuestionDialog()
			
		def __UseItemMonsterCardOnCancel(self):
			self.OnCloseQuestionDialog()
