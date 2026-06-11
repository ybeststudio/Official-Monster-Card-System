# Add the following to this file:
if app.ENABLE_MONSTER_CARD:
	window["height"] = window["height"]
	window["children"][0]["height"] = window["children"][0]["height"]
	window["children"][0]["children"] = window["children"][0]["children"] + [
		{ "name" : "Main", "type" : "text", "text" : uiScriptLocale.KEYCHANGE_MONSTER_CARD_WINDOW, "text_horizontal_align" : "left", "x" : 28 + 540 + 35, "y" : 75 + 360, },
	]
