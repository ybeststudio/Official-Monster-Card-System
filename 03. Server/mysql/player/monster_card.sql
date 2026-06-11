-- Monster Card System tables
-- Mevcut tablolar varsa silmez; eksikse olusturur.

CREATE TABLE IF NOT EXISTS `monstercard_mission` (
  `account_id` int(10) unsigned NOT NULL,
  `main_card0` int(10) unsigned NOT NULL DEFAULT 0,
  `main_card1` int(10) unsigned NOT NULL DEFAULT 0,
  `main_card2` int(10) unsigned NOT NULL DEFAULT 0,
  `card0` int(10) unsigned NOT NULL DEFAULT 0,
  `card1` int(10) unsigned NOT NULL DEFAULT 0,
  `card2` int(10) unsigned NOT NULL DEFAULT 0,
  `card3` int(10) unsigned NOT NULL DEFAULT 0,
  `card4` int(10) unsigned NOT NULL DEFAULT 0,
  `card5` int(10) unsigned NOT NULL DEFAULT 0,
  `card6` int(10) unsigned NOT NULL DEFAULT 0,
  `card7` int(10) unsigned NOT NULL DEFAULT 0,
  `card8` int(10) unsigned NOT NULL DEFAULT 0,
  `card9` int(10) unsigned NOT NULL DEFAULT 0,
  `card10` int(10) unsigned NOT NULL DEFAULT 0,
  `card11` int(10) unsigned NOT NULL DEFAULT 0,
  `card12` int(10) unsigned NOT NULL DEFAULT 0,
  `card13` int(10) unsigned NOT NULL DEFAULT 0,
  `card14` int(10) unsigned NOT NULL DEFAULT 0,
  `card15` int(10) unsigned NOT NULL DEFAULT 0,
  `main_card0_killed` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `main_card1_killed` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `main_card2_killed` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `last_mission_back` bigint(20) NOT NULL DEFAULT 0,
  `last_order_back` bigint(20) NOT NULL DEFAULT 0,
  `monstercardsystem_level` int(10) unsigned NOT NULL DEFAULT 1,
  `mission_reset_window_start` bigint(20) NOT NULL DEFAULT 0,
  `mission_resets_in_window` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`account_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `monstercard_status` (
  `account_id` int(10) unsigned NOT NULL,
  `vnum` int(10) unsigned NOT NULL,
  `collected_monstercards` int(10) unsigned NOT NULL DEFAULT 0,
  `killcount` int(10) unsigned NOT NULL DEFAULT 0,
  `needcards` int(10) unsigned NOT NULL DEFAULT 0,
  `stage` int(10) unsigned NOT NULL DEFAULT 0,
  `last_teleport` bigint(20) NOT NULL DEFAULT 0,
  `last_poly` bigint(20) NOT NULL DEFAULT 0,
  `last_spawn` bigint(20) NOT NULL DEFAULT 0,
  `last_fight` bigint(20) NOT NULL DEFAULT 0,
  PRIMARY KEY (`account_id`, `vnum`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS `monstercard_achiev` (
  `account_id` int(10) unsigned NOT NULL,
  `achiev_vnum` int(10) unsigned NOT NULL,
  `applied` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `regist_rank` tinyint(3) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`account_id`, `achiev_vnum`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
