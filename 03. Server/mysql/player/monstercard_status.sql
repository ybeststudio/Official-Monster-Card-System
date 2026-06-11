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

 Date: 10/06/2026 23:51:04
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for monstercard_status
-- ----------------------------
DROP TABLE IF EXISTS `monstercard_status`;
CREATE TABLE `monstercard_status`  (
  `account_id` bigint NOT NULL,
  `vnum` bigint NOT NULL,
  `collected_monstercards` int NOT NULL,
  `killcount` int NOT NULL,
  `needcards` int NOT NULL,
  `stage` int NOT NULL,
  `last_teleport` bigint NOT NULL DEFAULT 0,
  `last_poly` bigint NOT NULL DEFAULT 0,
  `last_spawn` bigint NOT NULL DEFAULT 0,
  `last_fight` bigint NOT NULL DEFAULT 0
) ENGINE = InnoDB CHARACTER SET = latin1 COLLATE = latin1_swedish_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of monstercard_status
-- ----------------------------

SET FOREIGN_KEY_CHECKS = 1;
