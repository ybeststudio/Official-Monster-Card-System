# In `IS_MONSTER_CARD_COLLECTION_ITEM`, extend the elif-statement with:
	if not hasattr(app, 'ENABLE_MONSTER_CARD') or not app.ENABLE_MONSTER_CARD:
		return 0

# In `IS_MONSTER_CARD_CONSUMABLE_ITEM`, extend the if-statement with:
	if not hasattr(app, 'ENABLE_MONSTER_CARD') or not app.ENABLE_MONSTER_CARD:
		return 0
