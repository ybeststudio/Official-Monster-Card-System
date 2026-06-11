ALTER TABLE `account`
	ADD COLUMN `monstercardsystem_level` SMALLINT NOT NULL DEFAULT 0 AFTER `last_play`;