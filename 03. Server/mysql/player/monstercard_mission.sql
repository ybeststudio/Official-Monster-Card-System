/*
 Navicat Premium Dump SQL

 Source Server         : OfficialFiles
 Source Server Type    : MySQL
 Source Server Version : 80041 (8.0.41)
 Source Host           : 192.168.1.163:3306
 Source Schema         : player

 Target Server Type    : MySQL
 Target Server Version : 80041 (8.0.41)
 File Encoding         : 65001

 Date: 10/06/2026 23:50:59
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for monstercard_mission
-- ----------------------------
DROP TABLE IF EXISTS `monstercard_mission`;
CREATE TABLE `monstercard_mission`  (
  `account_id` bigint NOT NULL,
  `main_card0` int NOT NULL,
  `main_card1` int NOT NULL,
  `main_card2` int NOT NULL,
  `card0` int NOT NULL,
  `card1` int NOT NULL,
  `card2` int NOT NULL,
  `card3` int NOT NULL,
  `card4` int NULL DEFAULT NULL,
  `card5` int NOT NULL,
  `card6` int NOT NULL,
  `card7` int NOT NULL,
  `card8` int NOT NULL,
  `card9` int NOT NULL,
  `card10` int NOT NULL,
  `card11` int NOT NULL,
  `card12` int NOT NULL,
  `card13` int NOT NULL,
  `card14` int NOT NULL,
  `card15` int NOT NULL,
  `main_card0_killed` int NOT NULL DEFAULT 0,
  `main_card1_killed` int NOT NULL DEFAULT 0,
  `main_card2_killed` int NOT NULL DEFAULT 0,
  `last_mission_back` bigint NOT NULL DEFAULT 0,
  `last_order_back` bigint NOT NULL DEFAULT 0,
  `monstercardsystem_level` smallint NOT NULL,
  `mission_reset_window_start` bigint NOT NULL DEFAULT 0,
  `mission_resets_in_window` tinyint NOT NULL DEFAULT 0,
  PRIMARY KEY (`account_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of monstercard_mission
-- ----------------------------

SET FOREIGN_KEY_CHECKS = 1;
