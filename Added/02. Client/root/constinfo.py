# option
IN_GAME_SHOP_ENABLE = 1
CONSOLE_ENABLE = 0

# AutoSave related
ENABLE_AUTOSAVE = 1
ENABLE_AUTOSAVE_KEYS = 1

PVPMODE_ENABLE = 1
#PVPMODE_TEST_ENABLE = 0
PVPMODE_ACCELKEY_ENABLE = 1
PVPMODE_ACCELKEY_DELAY = 0.5
PVPMODE_PROTECTED_LEVEL = 30

FOG_LEVEL0 = 4800.0
FOG_LEVEL1 = 9600.0
FOG_LEVEL2 = 12800.0
FOG_LEVEL = FOG_LEVEL0
FOG_LEVEL_LIST = [FOG_LEVEL0, FOG_LEVEL1, FOG_LEVEL2]

import systemSetting
CAMERA_MAX_DISTANCE_SHORT = 2500.0
CAMERA_MAX_DISTANCE_LONG = 3500.0 * 2
CAMERA_MAX_DISTANCE_LIST = [CAMERA_MAX_DISTANCE_SHORT, CAMERA_MAX_DISTANCE_LONG]
CAMERA_MAX_DISTANCE = CAMERA_MAX_DISTANCE_LIST[systemSetting.GetCameraMode()]

CHRNAME_COLOR_INDEX = 0

ENVIRONMENT_NIGHT="d:/ymir work/environment/moonlight04.msenv"

# constant
HIGH_PRICE = 500000
MIDDLE_PRICE = 50000
ERROR_METIN_STONE = 28960
SUB2_LOADING_ENABLE = 1
EXPANDED_COMBO_ENABLE = 1
CONVERT_EMPIRE_LANGUAGE_ENABLE = 1
USE_ITEM_WEAPON_TABLE_ATTACK_BONUS = 0
ADD_DEF_BONUS_ENABLE = 1
LOGIN_COUNT_LIMIT_ENABLE = 0

USE_SKILL_EFFECT_UPGRADE_ENABLE = 1

VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD = 1
GUILD_MONEY_PER_GSP = 100
GUILD_WAR_TYPE_SELECT_ENABLE = 1
TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE = 0

HAIR_COLOR_ENABLE = 1
ARMOR_SPECULAR_ENABLE = 1
WEAPON_SPECULAR_ENABLE = 1
SEQUENCE_PACKET_ENABLE = 1
KEEP_ACCOUNT_CONNETION_ENABLE = 1
MINIMAP_POSITIONINFO_ENABLE = 0
CONVERT_EMPIRE_LANGUAGE_ENABLE = 0
USE_ITEM_WEAPON_TABLE_ATTACK_BONUS = 0
ADD_DEF_BONUS_ENABLE = 0
LOGIN_COUNT_LIMIT_ENABLE = 0
PVPMODE_PROTECTED_LEVEL = 15
TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE = 10

isItemQuestionDialog = 0

ENABLE_MCP_SERVER = True # DEV/LOCAL ONLY: enable the localhost-authenticated in-client MCP server
MCP_SERVER_HOST = "127.0.0.1"
MCP_SERVER_PORT = 8765
# UI debug (outline + optional DebugWindow login). Requires client built with ENABLE_UI_DEBUG_WINDOW.
ENABLE_UI_DEBUG_WINDOW = False

def GET_ITEM_QUESTION_DIALOG_STATUS():
	global isItemQuestionDialog
	return isItemQuestionDialog

def SET_ITEM_QUESTION_DIALOG_STATUS(flag):
	global isItemQuestionDialog
	isItemQuestionDialog = flag

import app
import net

IS_ENABLE_TREASURE_HUNT_EVENT = False

# Monster Card item vnums (must match server kalisto::MonstercardSystem in MonstercardSystem.h).
MONSTERCARD_ITEM_VNUM = 50283
MONSTERCARD_TRADEABLE_ITEM_VNUM = 50284
MONSTERCARD_MISSION_RESET_ITEM_VNUM = 72322
MONSTERCARD_MISSION_SHUFFLE_ITEM_VNUM = 72323

########################

def SET_DEFAULT_FOG_LEVEL():
	global FOG_LEVEL
	app.SetMinFog(FOG_LEVEL)

def SET_FOG_LEVEL_INDEX(index):
	global FOG_LEVEL
	global FOG_LEVEL_LIST
	try:
		FOG_LEVEL=FOG_LEVEL_LIST[index]
	except IndexError:
		FOG_LEVEL = FOG_LEVEL_LIST[0]

	app.SetMinFog(FOG_LEVEL)

def GET_FOG_LEVEL_INDEX():
	global FOG_LEVEL
	global FOG_LEVEL_LIST
	return FOG_LEVEL_LIST.index(FOG_LEVEL)

########################

def SET_DEFAULT_CAMERA_MAX_DISTANCE():
	global CAMERA_MAX_DISTANCE
	app.SetCameraMaxDistance(CAMERA_MAX_DISTANCE)

def SET_CAMERA_MAX_DISTANCE_INDEX(index):
	global CAMERA_MAX_DISTANCE
	global CAMERA_MAX_DISTANCE_LIST
	try:
		CAMERA_MAX_DISTANCE=CAMERA_MAX_DISTANCE_LIST[index]
	except:
		CAMERA_MAX_DISTANCE=CAMERA_MAX_DISTANCE_LIST[0]

	app.SetCameraMaxDistance(CAMERA_MAX_DISTANCE)

def GET_CAMERA_MAX_DISTANCE_INDEX():
	global CAMERA_MAX_DISTANCE
	global CAMERA_MAX_DISTANCE_LIST
	return CAMERA_MAX_DISTANCE_LIST.index(CAMERA_MAX_DISTANCE)

########################

import chrmgr
import player
import app

def SET_DEFAULT_CHRNAME_COLOR():
	global CHRNAME_COLOR_INDEX
	chrmgr.SetEmpireNameMode(CHRNAME_COLOR_INDEX)

def SET_CHRNAME_COLOR_INDEX(index):
	global CHRNAME_COLOR_INDEX
	CHRNAME_COLOR_INDEX=index
	chrmgr.SetEmpireNameMode(index)

def GET_CHRNAME_COLOR_INDEX():
	global CHRNAME_COLOR_INDEX
	return CHRNAME_COLOR_INDEX

def SET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD(index):
	global VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD
	VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD = index

def GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD():
	global VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD
	return VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD

def SET_DEFAULT_CONVERT_EMPIRE_LANGUAGE_ENABLE():
	global CONVERT_EMPIRE_LANGUAGE_ENABLE
	net.SetEmpireLanguageMode(CONVERT_EMPIRE_LANGUAGE_ENABLE)

def SET_DEFAULT_USE_ITEM_WEAPON_TABLE_ATTACK_BONUS():
	global USE_ITEM_WEAPON_TABLE_ATTACK_BONUS
	player.SetWeaponAttackBonusFlag(USE_ITEM_WEAPON_TABLE_ATTACK_BONUS)

def SET_DEFAULT_USE_SKILL_EFFECT_ENABLE():
	global USE_SKILL_EFFECT_UPGRADE_ENABLE
	app.SetSkillEffectUpgradeEnable(USE_SKILL_EFFECT_UPGRADE_ENABLE)

def SET_TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE():
	global TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE
	app.SetTwoHandedWeaponAttSpeedDecreaseValue(TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE)

########################
import item

ACCESSORY_MATERIAL_LIST = [50623, 50624, 50625, 50626, 50627, 50628, 50629, 50630, 50631, 50632, 50633, 50634, 50635, 50636, 50637, 50638, 50639]
#ACCESSORY_MATERIAL_LIST = [50623, 50623, 50624, 50624, 50625, 50625, 50626, 50627, 50628, 50629, 50630, 50631, 50632, 50633, 
#			    50623, 50623, 50624, 50624, ]
if app.ENABLE_CONQUEROR_LEVEL:
	ACCESSORY_MATERIAL_LIST += [50661, 50662, 50663, 50664]

JewelAccessoryInfos = [
		# jewel		wrist	neck	ear
		[ 50634,	14420,	16220,	17220 ],
		[ 50635,	14500,	16500,	17500 ],
		[ 50636,	14520,	16520,	17520 ],
		[ 50637,	14540,	16540,	17540 ],
		[ 50638,	14560,	16560,	17560 ],
		[ 50639,	14570,	16570,	0     ],# ?? (?? ?)
	]

if app.ENABLE_CONQUEROR_LEVEL:
	JewelAccessoryInfos += [
		# jewel wrist neck ear
		[ 50661, 14590, 16590, 17580 ],
		[ 50662, 14600, 16600, 17590 ],
		[ 50663, 14610, 16610, 17600 ],
		[ 50664, 14580, 16580, 17570 ],
	]

def GET_ACCESSORY_MATERIAL_VNUM(vnum, subType):
	ret = vnum
	item_base = (vnum / 10) * 10
	for info in JewelAccessoryInfos:
		if item.ARMOR_WRIST == subType:
			if info[1] == item_base:
				return info[0]
		elif item.ARMOR_NECK == subType:
			if info[2] == item_base:
				return info[0]
		elif item.ARMOR_EAR == subType:
			if info[3] == item_base:
				return info[0]

	if vnum >= 16210 and vnum <= 16219:
		return 50625

	if item.ARMOR_WRIST == subType:
		WRIST_ITEM_VNUM_BASE = 14000
		ret -= WRIST_ITEM_VNUM_BASE
	elif item.ARMOR_NECK == subType:
		NECK_ITEM_VNUM_BASE = 16000
		ret -= NECK_ITEM_VNUM_BASE
	elif item.ARMOR_EAR == subType:
		EAR_ITEM_VNUM_BASE = 17000
		ret -= EAR_ITEM_VNUM_BASE

	type = ret/20

	if type < 0 or type >= len(ACCESSORY_MATERIAL_LIST):
		type = (ret - 170) / 20
		if type < 0 or type >= len(ACCESSORY_MATERIAL_LIST):
			return 0

	return ACCESSORY_MATERIAL_LIST[type]

##################################################################
## The newly added'belt' item type and the item to be plugged into the belt socket...
## Since the belt socket system is the same as the accessory, it has to be done in this way like the hard coding related to the accessory above.

def GET_BELT_MATERIAL_VNUM(vnum, subType = 0):
	# Currently, only one item (#18900) can be inserted on all belts.
	return 18900

##################################################################
## Automatic Potion (HP: #72723 ~ #72726, SP: #72727 ~ #72730)

# Is the vnum an automatic potion?
def IS_AUTO_POTION(itemVnum):
	return IS_AUTO_POTION_HP(itemVnum) or IS_AUTO_POTION_SP(itemVnum)

def IS_AUTO_POTION_HP(itemVnum):
	if 72723 <= itemVnum and 72726 >= itemVnum:
		return 1
	elif itemVnum >= 76021 and itemVnum <= 76022:		## New Gift Fire Dragon's Blessing
		return 1
	elif itemVnum == 79012:
		return 1

	return 0

# Is the vnum an SP automatic potion?
def IS_AUTO_POTION_SP(itemVnum):
	if 72727 <= itemVnum and 72730 >= itemVnum:
		return 1
	elif itemVnum >= 76004 and itemVnum <= 76005:		## New Gift Blue Dragon's Blessing
		return 1
	elif itemVnum == 79013:
		return 1

	return 0

def IS_MONSTER_CARD_COLLECTION_ITEM(itemVnum):
	if not hasattr(app, 'ENABLE_MONSTER_CARD') or not app.ENABLE_MONSTER_CARD:
		return 0
	return itemVnum in (MONSTERCARD_ITEM_VNUM, MONSTERCARD_TRADEABLE_ITEM_VNUM)

def IS_MONSTER_CARD_CONSUMABLE_ITEM(itemVnum):
	if not hasattr(app, 'ENABLE_MONSTER_CARD') or not app.ENABLE_MONSTER_CARD:
		return 0
	return itemVnum in (MONSTERCARD_MISSION_RESET_ITEM_VNUM, MONSTERCARD_MISSION_SHUFFLE_ITEM_VNUM)

##################################################################
# No official BEGIN

if app.ENABLE_GROWTH_PET_SYSTEM:
	## Growth Pet
	def IS_PET_ITEM(itemVnum):
		if itemVnum == 0:
			return 0

		item.SelectItem(itemVnum)
		itemType = item.GetItemType()

		if itemType == item.ITEM_TYPE_PET:
			return 1

		return 0

if app.ENABLE_UI_DEBUG_WINDOW:
    # UI inspector: F12 cycle level in game (uidebug.py)
    UI_DEBUG_LEVEL = 2
    OUTLINE_DEBUG_ENABLE = 0

    def SetUiDebugLevel(level):
        global UI_DEBUG_LEVEL
        try:
            import wndMgr
            wndMgr.SetUiDebugLevel(int(level))
            UI_DEBUG_LEVEL = int(wndMgr.GetUiDebugLevel())
        except:
            UI_DEBUG_LEVEL = 0
        return UI_DEBUG_LEVEL

    def GetUiDebugLevel():
        try:
            import wndMgr
            return int(wndMgr.GetUiDebugLevel())
        except:
            return 0

    def CycleUiDebugLevel():
        try:
            import wndMgr
            return int(wndMgr.CycleUiDebugLevel())
        except:
            return 0

    def SetOutlineDebug(flag):
        global OUTLINE_DEBUG_ENABLE
        try:
            iFlag = int(flag)
        except:
            iFlag = 0
        iFlag = 1 if iFlag else 0
        OUTLINE_DEBUG_ENABLE = iFlag
        if iFlag:
            SetUiDebugLevel(UI_DEBUG_LEVEL if UI_DEBUG_LEVEL > 0 else 2)
        else:
            SetUiDebugLevel(0)
        return OUTLINE_DEBUG_ENABLE

    def ornek(flag):
        return SetOutlineDebug(flag)

    try:
        SetOutlineDebug(OUTLINE_DEBUG_ENABLE)
    except:
        pass

# No official END
