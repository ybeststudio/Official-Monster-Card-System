# Add to the imports:
if app.ENABLE_MONSTER_CARD:
	import uiMonsterCard



# In `AddItemData`, extend the if-statement with:
		if app.ENABLE_MONSTER_CARD:
			if itemVnum == constInfo.MONSTERCARD_TRADEABLE_ITEM_VNUM:
				mobVnumForCard = metinSlot[0]
				if mobVnumForCard == 0:
					mobVnumForCard = metinSlot[1]
				self.__AppendMonsterCardItemIcon(mobVnumForCard)
			elif itemVnum == constInfo.MONSTERCARD_ITEM_VNUM:
				mobVnumForCard = metinSlot[1]
				if mobVnumForCard == 0:
					mobVnumForCard = metinSlot[0]
				self.__AppendMonsterCardItemIcon(mobVnumForCard)


# Add anywhere in the ItemToolTip class:
	if app.ENABLE_MONSTER_CARD:
		def __AppendMonsterCardItemIcon(self, mobVnum):
			itemImage = ui.ImageBox()
			itemImage.SetParent(self)
			itemImage.Show()
			
			path = ""
			if uiMonsterCard.CARD_IMG_DICT.has_key(mobVnum):
				path = uiMonsterCard.CARD_IMG_DICT[mobVnum]
			else:
				path = "d:/ymir work/ui/game/monster_card/empty_card.sub"
				
			itemImage.LoadImage( path )
			itemImage.SetPosition((self.toolTipWidth/2)-itemImage.GetWidth()/2, self.toolTipHeight)
			self.toolTipHeight += itemImage.GetHeight()
			self.childrenList.append(itemImage)
			self.ResizeToolTip()
